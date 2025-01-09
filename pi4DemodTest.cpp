#include "pi4dqpskDemod.h"
#include <iostream>
#include <vector>
#include <complex>
// #include <cmath>
// #include <thread>
// #include <future>
// #include <mutex>
// #include <iostream>
// #include <algorithm>


int main() {
    // Define sample signal (for testing purposes, we simulate some data)
    std::vector<std::complex<double>> input_signal(16384*160, std::complex<double>(1.0, 0.0));  // Dummy signal
    std::vector<std::complex<double>> sig_after_decim;
    std::vector<std::complex<double>> sig_after_fll;
    std::vector<std::complex<double>> sig_after_sync;
    std::vector<std::complex<double>> sig_after_equalization;

    // Create an instance of the demodulator
    pi4dqpsk_demod bb_signal(2.56e6, 96.0e3, 18.0e3, 75.0e3);

    // Output decoded symbols
    std::vector<int> decoded_symbols;

    // std::cout << "input_signal @ freq_xlating_decim: " << input_signal.size() << std::endl; 
    // bb_signal.freq_xlating_decim(input_signal, sig_after_decim);
    // std::cout << "After call freq_xlating_decim. sig_after_decim: " << sig_after_decim.size() << std::endl; 
  
    std::cout << "Before call demod process." << std::endl;
    // Process the signal
    bb_signal.process(input_signal, decoded_symbols);
 
    // Log the decoded symbols
    for (auto symbol : decoded_symbols) {
        std::cout << symbol << " ";
    }
    std::cout << "Before return from main." << std::endl;

    return 0;
}
