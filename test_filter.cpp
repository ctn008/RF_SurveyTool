#include <iostream>
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <complex>
//#include <fftw3.h>
#include "c:\Users\Home\radioconda\Library\include\fftw3.h"
#include <iomanip> // For setting precision in output

// Hamming window function (same as before)
double hamming(int n, int N) {
    return 0.54 - 0.46 * std::cos(2.0 * M_PI * n / (N - 1));
}

// Corrected low-pass filter design with Hamming window and normalization
std::vector<double> designLowPassFilter(double cutoff_freq, double transition_width, size_t filter_length, double sample_rate) {
    if (cutoff_freq <= 0 || transition_width <= 0 || filter_length <= 0 || sample_rate <= 0 || cutoff_freq >= sample_rate / 2.0) {
        std::cerr << "Invalid filter parameters." << std::endl;
        return {};
    }

    if (filter_length % 2 == 0) {
        std::cerr << "Filter length must be odd." << std::endl;
        return {};
    }

    std::vector<double> filter_coeffs(filter_length);
    int center = filter_length / 2;
    double normalized_cutoff = cutoff_freq / sample_rate;

    for (size_t n = 0; n < filter_length; ++n) {
        if (n == center) {
            filter_coeffs[n] = 2.0 * normalized_cutoff;
        } else {
            filter_coeffs[n] = std::sin(2.0 * M_PI * normalized_cutoff * (n - center)) / (M_PI * (n - center));
        }
        filter_coeffs[n] *= hamming(n, filter_length);
    }

    // Normalize to sum of 1 (Unity Gain at DC)
    double sum = std::accumulate(filter_coeffs.begin(), filter_coeffs.end(), 0.0);
    if (sum != 0.0) { // Avoid division by zero
        std::transform(filter_coeffs.begin(), filter_coeffs.end(), filter_coeffs.begin(),
                       [sum](double val) { return val / sum; });
    }

    return filter_coeffs;
}

// ... (polyphaseDecimation and frequencyTranslateFilterAndDecimate functions remain the same)

int main() {
    // Filter specifications
    double cutoff_freq = 12.5e3;    // 12.5 kHz
    double transition_width = 2.5e3; // 2.5 kHz
    double sample_rate = 100e3;      // Example sample rate (100 kHz)

    // Check filter length requirement
    size_t min_filter_length = static_cast<size_t>(std::ceil(3.3 * sample_rate / transition_width));
    size_t filter_length = std::max((size_t)101, min_filter_length);
    if (filter_length % 2 == 0) filter_length++; // Ensure odd length
    std::cout << "Required filter length: " << min_filter_length << std::endl;
    std::cout << "Using filter length: " << filter_length << std::endl;

    std::vector<double> filter_coeffs = designLowPassFilter(cutoff_freq, transition_width, filter_length, sample_rate);

    if (filter_coeffs.empty()) {
        return 1;
    }

    // Frequency Response Analysis using FFTW
    int fft_size = 1024;
    fftw_complex *in, *out;
    fftw_plan p;

    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fft_size);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * fft_size);
    p = fftw_plan_dft_1d(fft_size, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    for (int i = 0; i < fft_size; ++i) {
        in[i][0] = (i < filter_coeffs.size()) ? filter_coeffs[i] : 0.0;
        in[i][1] = 0.0;
    }

    fftw_execute(p);

    std::cout << "\nFrequency Response (Magnitude - dB):\n";
    std::cout << std::fixed << std::setprecision(2); // Set output precision
    for (int i = 0; i < fft_size / 2; ++i) {
        double magnitude = std::sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);
        double magnitude_db = 20.0 * std::log10(std::max(magnitude, 1e-10)); // Use std::max to avoid log of very small numbers
        std::cout << (i * sample_rate / fft_size) << " Hz: " << magnitude_db << " dB" << std::endl;
    }

    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

    // ... (rest of the main function remains the same)
    return 0;
}