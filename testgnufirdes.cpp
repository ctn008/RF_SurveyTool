#include <gnuradio/filter/firdes.h>
#include <gnuradio/blocks/null_sink.h>
#include <gnuradio/top_block.h>
#include <vector>
#include <iostream>

int main() {
    // Parameters for the low pass filter
    float gain = 1.0;               // Gain of the filter
    float sampling_rate = 1000.0;   // Sampling rate in Hz
    float cutoff_freq = 200.0;      // Cutoff frequency in Hz
    float transition_width = 50.0;  // Transition width in Hz

    // Generate the low pass filter coefficients using firdes.low_pass
    std::vector<float> filter_taps = gr::filter::firdes::low_pass(
        gain, sampling_rate, cutoff_freq, transition_width
    );

    // Output the filter taps (coefficients) for debugging
    std::cout << "Filter Taps: ";
    for (const auto& tap : filter_taps) {
        std::cout << tap << " ";
    }
    std::cout << std::endl;

    return 0;
}
