/*
COMPILING: firdes.h is in /usr/local/include/gnuradio/filter/firdes.h and in libgnuradio-filter.so
g++ -o testgnufirdes.out testgnufirdes.cpp pi4dqpskDemod.cpp -lfmt -lgnuradio-filter
*/
#include <gnuradio/filter/firdes.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/top_block.h>
#include <vector>
#include <iostream>
#include "pi4dqpskDemod.h"
#include <complex>

int main() {
    // Parameters for the low pass filter
    float gain = 1.0;               // Gain of the filter
    float sampling_rate = 2.56e6;   // Sampling rate in Hz
    float cutoff_freq = 12.5e3;      // Cutoff frequency in Hz
    float transition_width = 2.5e3;  // Transition width in Hz

    // Generate the low pass filter coefficients using firdes.low_pass
    /*static std::vector< float > 	low_pass (double gain, double sampling_freq, double cutoff_freq, double transition_width, 
    fft::window::win_type window=fft::window::win_type::WIN_HAMMING, double param=6.76)*/
    std::vector<float> filter_taps = gr::filter::firdes::low_pass(
        gain, sampling_rate, cutoff_freq, transition_width
    );

    // Output the length of the filter taps
    std::cout << "Length: " << filter_taps.size() << std::endl;

    std::cout << std::endl;


    // Calculate the filter coefficients
    std::vector<double> filter_coefficients = calculate_filter_coefficients(sampling_rate, cutoff_freq, transition_width);
    std::vector<double> filterCoefficients = designLowPassFilter(sampling_rate, cutoff_freq, transition_width);
    
    std::cout << "print ratio for filter_coefficients and filter_taps" << std::endl;
    // print ratio for filter_coefficients and filterCoefficients
    for (int i = 0; i < filter_taps.size(); ++i) {
        double ratio = filter_coefficients[i] / filter_taps[i];
        if (ratio < 0.999 || ratio > 1.001) {
            std::cout << "ratio: " << ratio << " i: " << i << std::endl;
            std::cout << i << ": value: " << filter_coefficients[i] << " " << filter_taps[i] << "\n";
        }   
    }
    std::cout << std::endl << std::endl;
    std::cout << "print ratio for filterCoefficients and filter_taps" << std::endl;
    for (int i = 0; i < filter_taps.size(); ++i) {
        double ratio = filterCoefficients[i] / filter_taps[i];
        if (ratio < 0.999 || ratio > 1.001) {
            std::cout << "ratio: " << ratio << " i: " << i << std::endl;
            std::cout << i << ": value: " << filterCoefficients[i] << " " << filter_taps[i] << "\n";
        }   
    }
    std::cout << std::endl << std::endl;
    // for (int i = 0; i < filter_taps.size(); ++i) {
    //     std::cout << i << ": " << filterCoefficients[i] << " " << filter_coefficients[i] << " " << filter_taps[i] << std::endl;
    // }
    // std::cout << std::endl << std::endl;

    // for (int i = 0; i < filter_taps.size(); ++i) {
    //     std::cout << hamming(i, filter_taps.size()) << " ";
    // }   
    // // Define sample signal (for testing purposes, we simulate some data)
    // std::vector<std::complex<double>> input_signal(16384*160, std::complex<double>(1.0, 0.0));  // Dummy signal
    // std::vector<std::complex<double>> sig_after_decim;

    // pi4dqpsk_demod bb_signal(2.56e6, 80.0e3, 18.0e3, 75.0e3);
    // std::cout << "input_signal @ freq_xlating_decim: " << input_signal.size() << std::endl; 
    // bb_signal.freq_xlating_decim(input_signal, sig_after_decim);
    // std::cout << "After call freq_xlating_decim. sig_after_decim: " << sig_after_decim.size() << std::endl; 

    return 0;
}

