
#include <fftw3.h>
#include <cmath>
#include <iostream>
#include <vector>

// Generate a Hamming window and apply it to a complex signal
void applyHammingWindow(fftw_complex* signal, int N) {
    for (int i = 0; i < N; i++) {
        double hamming = 0.54 - 0.46 * cos(2 * M_PI * i / (N - 1));
        signal[i][0] *= hamming; // Multiply real part
        signal[i][1] *= hamming; // Multiply imaginary part
    }
}

int main() {
    const int N = 8192;
    fftw_complex signal[N], output[N];
    fftw_plan plan;

    // Generate a complex test signal (combination of two frequencies)
    for (int i = 0; i < N; i++) {
        double t = i / static_cast<double>(N);
        signal[i][0] = cos(2 * M_PI * 50 * t) + 0.5 * cos(2 * M_PI * 120 * t); // Real part
        signal[i][1] = sin(2 * M_PI * 50 * t) + 0.5 * sin(2 * M_PI * 120 * t); // Imaginary part
    }

    // Apply Hamming window
    applyHammingWindow(signal, N);

    // FFTW setup and execution
    plan = fftw_plan_dft_1d(N, signal, output, FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);

    // Adjust FFT output (scale for window function)
    double window_correction_factor = 0.54 * N; // Adjust for Hamming window's amplitude scaling
    for (int i = 0; i < N; i++) {
        output[i][0] /= window_correction_factor; // Real part
        output[i][1] /= window_correction_factor; // Imaginary part
    }

    // Output first 10 results
    for (int i = 0; i < 10; i++) {
        std::cout << "Freq[" << i << "] = "
                  << output[i][0] << " + " << output[i][1] << "i\n";
    }

    // Cleanup
    fftw_destroy_plan(plan);

    return 0;
}
