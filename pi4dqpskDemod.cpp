#include "pi4dqpskDemod.h"
// Implement pi4dqpsk_demod class

// Function to calculate filter coefficients for low-pass filter
/* ctn008 function provided by GPT - verified by comparing code with gr.filter compute_ntaps*/
std::vector<double> calculate_filter_coefficients(double sampling_rate, double cutoff_freq, double transition_width) {

    int filter_length = compute_ntaps(sampling_rate, transition_width); 
    std::vector<double> coefficients(filter_length);
    double normalized_cutoff = 2 * M_PI * cutoff_freq / sampling_rate;
    // Calculate the ideal sinc filter coefficients
    int mid_point = (filter_length -1) / 2; // filter_length is odd
    for (int i = 0; i < filter_length; ++i) {
        if (i == mid_point) {
            coefficients[i] = 2.0 * cutoff_freq / sampling_rate * hamming(i, filter_length);  // Ideal sinc at the center (midpoint)
        } else {
            coefficients[i] = std::sin(normalized_cutoff * (i - mid_point)) / (M_PI * (i - mid_point)) * hamming(i, filter_length);  
        }
    }

    // // Apply windowing (Hamming window)
    // for (int i = 0; i < filter_length; ++i) {
    //     coefficients[i] *= 0.54 - 0.46 * std::cos(2 * M_PI * i / (filter_length - 1));
    // }

    // Normalize the filter coefficients
    double sum = coefficients[mid_point];  
    for (int i = 1; i <= mid_point; ++i) {
        sum += 2 * coefficients[mid_point + i];  // Symmetric coefficients
    }
    double scale = 1.0 / sum;
    for (int i = 0; i < filter_length; ++i) {
        coefficients[i] *= scale;
    }

    return coefficients;
}

// Hamming window function
double hamming(int n, int N) {
    return 0.54 - 0.46 * std::cos(2.0 * M_PI * n / (N - 1));
}

// Low-pass filter design with Hamming window
std::vector<double> designLowPassFilter(double sample_rate, double cutoff_freq, double transition_width) {

    if (cutoff_freq <= 0 || transition_width <= 0 || sample_rate <= 0 || cutoff_freq >= sample_rate / 2.0) {
        std::cerr << "Invalid filter parameters." << std::endl;
        return {};
    }
    int filter_length = compute_ntaps(sample_rate, transition_width);
    std::vector<double> filter_coeffs(filter_length);
    int center = (filter_length-1) / 2;
    double normalized_cutoff = 2 * M_PI * cutoff_freq / sample_rate;
    for (int n = -center; n <= center; ++n) {
        if (n == 0) {
            filter_coeffs[n+center] = normalized_cutoff / M_PI * hamming(n+center, filter_length);
        } else {
            filter_coeffs[n+center] = std::sin(normalized_cutoff * n) / (M_PI * n) * hamming(n+center, filter_length);
        }
    }

    // Normalize to sum of 1 (Unity Gain at DC)
    double sum = filter_coeffs[center];  
    for (size_t i = 1; i <= center; ++i) {
        sum += 2 * filter_coeffs[center + i];  // Symmetric coefficients
    }
    double scale = 1.0 / sum;
    if (sum != 0.0) { // Avoid division by zero
        for (size_t i = 0; i < filter_length; ++i) {
            filter_coeffs[i] *= scale;
        }
   }

    return filter_coeffs;
}

// Polyphase decomposition and filtering for decimation (same as before)
std::vector<std::complex<double>> polyphaseDecimation(const std::vector<std::complex<double>>& input, const std::vector<double>& filterCoeffs, int decimationFactor) {
    int numCoeffs = filterCoeffs.size();
    int numPolyphaseComponents = decimationFactor;

    std::vector<std::vector<double>> polyphaseFilters(numPolyphaseComponents);
    for (int i = 0; i < numCoeffs; ++i) {
        polyphaseFilters[i % numPolyphaseComponents].push_back(filterCoeffs[i]);
    }

    std::vector<std::complex<double>> output;
    int inputLength = input.size();

    for (int n = 0; n < inputLength / decimationFactor; ++n) {
        std::complex<double> outputSample(0.0, 0.0);
        for (int i = 0; i < numPolyphaseComponents; ++i) {
            double sum = 0.0;
            int polyphaseLength = polyphaseFilters[i].size();
            for (int j = 0; j < polyphaseLength; ++j) {
                int inputIndex = n * decimationFactor + i - j;
                if (inputIndex >= 0 && inputIndex < inputLength) {
                    sum += input[inputIndex].real() * polyphaseFilters[i][j];
                }
            }
            outputSample += sum;
        }
        output.push_back(outputSample);
    }

    return output;
}

int compute_ntaps(double sampling_rate, double transition_width) {
    // set window type to Hamming and attenuation_dB to 53, param ???
    // int ntaps = static_cast<int>(3.3 * sampling_rate / transition_width);
    int ntaps = static_cast<int>(53 * sampling_rate / (22.0 * transition_width));
    if ((ntaps & 1) == 0) // if even...
        ntaps++;          // ...make odd
    return ntaps;
    // more accurate calculation from firdes
    /*
    // Based on formula from Multirate Signal Processing for Communications Systems, fredric j harris
    // for Hamming Windows: attenuation_dB = 53
    int ntaps = (int)(attenuation_dB * sampling_freq / (22.0 * transition_width));
    if ((ntaps & 1) == 0) // if even...
        ntaps++;          // ...make odd
    return ntaps;
    */
}
// Implementation of Frequency Translation and Decimation
void pi4dqpsk_demod::freq_xlating_decim(const std::vector<std::complex<double>>& input_signal, std::vector<std::complex<double>>& output_signal) {

    size_t decimation_factor = static_cast<size_t>(sampling_rate / down_sampling_rate);
    std::cout << "decimation_factor =" << decimation_factor << std::endl;

    if (decimation_factor <= 0) {
        std::cerr << "Decimation factor must be positive." << std::endl;
        return;
    }
    output_signal.clear(); // Clear the output vector before filling it. Important!

    int inputLength = input_signal.size();
    std::vector<std::complex<double>> translatedSignal(inputLength);
    for (size_t n = 0; n < inputLength; ++n) {
        double phase = 2.0 * M_PI * freq_xlating / sampling_rate * n;
        translatedSignal[n] = input_signal[n] * std::exp(std::complex<double>(0.0, phase));
    }

    // Filter specifications
    double cutoff_freq = 12.5e3;  // 12.5 kHz (Bandwidth)
    double transition_width = 2.5e3; // 2.5 kHz (Transition width)

    // Calculate the filter coefficients
    // std::vector<double> filter_coefficients = calculate_filter_coefficients(sampling_rate, cutoff_freq, transition_width);
    std::vector<double> filterCoefficients = designLowPassFilter(sampling_rate, cutoff_freq, transition_width);

    // Apply filtering *after* frequency translation
    // Use polyphase decimation
    // output_signal = polyphaseDecimation(translatedSignal, filter_coefficients, decimation_factor);
    output_signal = polyphaseDecimation(translatedSignal, filterCoefficients, decimation_factor);

}

/*     // Decimation factor
    size_t decimation_factor = static_cast<size_t>(sampling_rate / down_sampling_rate);

    std::cout << "decimation_factor =" << decimation_factor << std::endl;

    // Filter specifications
    double cutoff_freq = 12.5e3;  // 12.5 kHz (Bandwidth)
    double transition_width = 2.5e3; // 2.5 kHz (Transition width)
    size_t filter_length = 101;   // Length of the low-pass filter (number of taps)

    // Calculate the filter coefficients
    std::vector<double> filter_coefficients = calculate_filter_coefficients(sampling_rate, cutoff_freq, transition_width, filter_length);

    // Number of polyphase filters needed
    size_t num_phases = decimation_factor;

    // Create polyphase filter bank (split the filter coefficients into phases)
    std::vector<std::vector<double>> polyphase_filters(num_phases, std::vector<double>(filter_length / num_phases));

    for (size_t i = 0; i < filter_length; ++i) {
        polyphase_filters[i % num_phases][i / num_phases] = filter_coefficients[i];
    }

    // Apply frequency translation and decimation using polyphase decomposition
    output_signal.clear();

    // Frequency translation factor (complex exponential)
    double frequency_translation_factor = 2.0 * M_PI * freq_xlating / sampling_rate;

    // Apply filtering, frequency translation, and decimation
    for (size_t i = 0; i < input_signal.size(); ++i) {
        // Frequency translation: multiply the input signal by a complex exponential
        std::complex<double> translated_sample = input_signal[i] * std::exp(std::complex<double>(0, -frequency_translation_factor * i));

        // Determine the phase index for this sample
        size_t phase_index = i % num_phases;

        // Convolve translated signal with the corresponding polyphase filter
        std::complex<double> filtered_sample(0.0, 0.0);
        for (size_t j = 0; j < polyphase_filters[phase_index].size(); ++j) {
            if (i >= j) {
                filtered_sample += translated_sample * polyphase_filters[phase_index][j];
            }
        }

        // Only keep every `decimation_factor`-th sample in the output signal
        if (i % decimation_factor == 0) {
            output_signal.push_back(filtered_sample);
            std::cout << "output_signal.size() = " << output_signal.size() << std::endl;
        }
    }
} */

void pi4dqpsk_demod::rational_resampler(const std::vector<std::complex<double>>& input_signal, std::vector<std::complex<double>>& output_signal, int interp_factor, int decim_factor) {
    // Filter specifications (similar to previous function)
    double cutoff_freq = 12.5e3;  // 12.5 kHz (Bandwidth)
    double transition_width = 5e3; // 5 kHz (Transition width)

    // Calculate filter coefficients (low-pass filter for interpolation)
    std::vector<double> filter_coefficients = calculate_filter_coefficients(sampling_rate, cutoff_freq, transition_width);

    // Create an upsampled (interpolated) signal
    std::vector<std::complex<double>> upsampled_signal;
    size_t input_size = input_signal.size();

    // Interpolation: Upsample by `interp_factor`
    for (size_t i = 0; i < input_size; ++i) {
        // Insert `interp_factor - 1` zero samples between each input sample
        for (int j = 0; j < interp_factor; ++j) {
            if (j == 0) {
                upsampled_signal.push_back(input_signal[i]);  // Copy the original sample
            } else {
                upsampled_signal.push_back(std::complex<double>(0.0, 0.0));  // Insert zero samples
            }
        }
    }

    // Apply low-pass filter to the upsampled signal to remove high-frequency aliasing
    std::vector<std::complex<double>> filtered_signal(upsampled_signal.size(), std::complex<double>(0.0, 0.0));
    for (size_t i = 0; i < filtered_signal.size(); ++i) {
        for (size_t j = 0; j < filter_coefficients.size(); ++j) {
            if (i >= j) {
                filtered_signal[i] += upsampled_signal[i - j] * std::complex<double>(filter_coefficients[j], 0.0);
            }
        }
    }

    // Decimation: Downsample by `decim_factor`
    output_signal.clear();
    for (size_t i = 0; i < filtered_signal.size(); i += decim_factor) {
        output_signal.push_back(filtered_signal[i]);
    }
}

// Implementation of Frequency Lock Loop (FLL) with Band-Edge
void pi4dqpsk_demod::freq_lock_loop(const std::vector<std::complex<double>>& input_signal,
                                         std::vector<std::complex<double>>& output_signal) {
    // Step 1: Matched Filter with Root Raised Cosine (RRC)
    double rolloff = 0.35;  // RRC roll-off factor
    size_t filter_length = 101;  // Length of the RRC filter
    std::vector<double> rrc_filter = calculate_rrc_filter(rolloff, symbol_rate, down_sampling_rate, filter_length);
    std::vector<std::complex<double>> filtered_signal = matched_filter(input_signal, rrc_filter);

    // Step 2: Band-Edge FLL
    double error = 0.0;  // Frequency error
    double loop_gain = 0.01;  // Loop gain for frequency correction
    double integrator = 0.0;  // Phase integrator
    output_signal.resize(input_signal.size());
    double phase = 0.0;  // Phase adjustment term

    for (size_t n = 0; n < filtered_signal.size(); ++n) {
        // Compute band-edge error
        double magnitude = std::abs(filtered_signal[n]);
        error += magnitude * std::sin(2 * M_PI * phase);

        // Update phase integrator and loop phase
        integrator += loop_gain * error;
        phase += integrator;

        // Frequency correction
        std::complex<double> correction = std::polar(1.0, -phase);
        output_signal[n] = input_signal[n] * correction;
    }
}

// Matched Filter Implementation
std::vector<std::complex<double>> pi4dqpsk_demod::matched_filter(const std::vector<std::complex<double>>& signal,
                                                                 const std::vector<double>& coefficients) {
    size_t filter_length = coefficients.size();
    std::vector<std::complex<double>> filtered_signal(signal.size(), std::complex<double>(0.0, 0.0));

    for (size_t n = 0; n < signal.size(); ++n) {
        for (size_t k = 0; k < filter_length; ++k) {
            if (n >= k) {
                filtered_signal[n] += signal[n - k] * coefficients[k];
            }
        }
    }
    return filtered_signal;
}

// Root Raised Cosine Filter Coefficient Calculation
std::vector<double> pi4dqpsk_demod::calculate_rrc_filter(double rolloff, double symbol_rate, double sampling_rate, size_t filter_length) {
    std::vector<double> rrc_filter(filter_length, 0.0);
    double Ts = 1.0 / symbol_rate;  // Symbol period
    double T = 1.0 / sampling_rate;  // Sampling period
    double alpha = rolloff;
    size_t midpoint = filter_length / 2;

    for (size_t i = 0; i < filter_length; ++i) {
        double t = (static_cast<int>(i) - static_cast<int>(midpoint)) * T;

        if (t == 0.0) {
            rrc_filter[i] = 1.0 - alpha + (4 * alpha / M_PI);
        } else if (std::abs(t) == Ts / (4 * alpha)) {
            rrc_filter[i] = (alpha / sqrt(2)) *
                            ((1 + 2 / M_PI) * std::sin(M_PI / (4 * alpha)) +
                             (1 - 2 / M_PI) * std::cos(M_PI / (4 * alpha)));
        } else {
            double numerator = std::sin(M_PI * t * (1 - alpha) / Ts) +
                               4 * alpha * t / Ts * std::cos(M_PI * t * (1 + alpha) / Ts);
            double denominator = M_PI * t * (1 - (4 * alpha * t / Ts) * (4 * alpha * t / Ts));
            rrc_filter[i] = numerator / denominator;
        }
    }
    return rrc_filter;
}


// Implementation of Timing Synchronization
void pi4dqpsk_demod::timing_sync(const std::vector<std::complex<double>>& input_signal, std::vector<std::complex<double>>& output_signal) {
    // Step 1: Design a 32-tap filter for polyphase filtering
    size_t num_taps = 32;  // Filter size
    std::vector<double> filter_taps = design_filter(num_taps, symbol_rate, down_sampling_rate);

    // Step 2: Create a polyphase filter bank with 32 phases
    size_t num_phases = num_taps;
    std::vector<std::vector<double>> polyphase_bank = polyphase_filter_bank(filter_taps, num_phases);

    // Step 3: Timing synchronization loop
    double timing_error = 0.0;
    double loop_gain = 0.05;  // Loop gain for adjusting timing
    double fractional_offset = 0.0;  // Fractional timing offset
    size_t sample_index = 0;
    output_signal.clear();

    while (sample_index + num_phases < input_signal.size()) {
        // Select the appropriate phase based on the fractional offset
        size_t phase_index = static_cast<size_t>(fractional_offset * num_phases) % num_phases;

        // Apply the polyphase filter corresponding to the phase index
        std::complex<double> filtered_sample(0.0, 0.0);
        for (size_t k = 0; k < num_phases; ++k) {
            filtered_sample += input_signal[sample_index + k] * polyphase_bank[phase_index][k];
        }

        // Append the synchronized sample to the output
        output_signal.push_back(filtered_sample);

        // Update the timing error and fractional offset
        timing_error = std::arg(filtered_sample);  // Example: use the phase as the timing error
        fractional_offset += 1.0 - loop_gain * timing_error;

        // Ensure fractional_offset remains in [0, 1)
        if (fractional_offset >= 1.0) fractional_offset -= 1.0;
        if (fractional_offset < 0.0) fractional_offset += 1.0;

        // Increment the sample index
        sample_index += num_phases;
    }
}

// Polyphase Filter Bank Generation
std::vector<std::vector<double>> pi4dqpsk_demod::polyphase_filter_bank(const std::vector<double>& filter_taps, size_t num_phases) {
    std::vector<std::vector<double>> bank(num_phases, std::vector<double>(filter_taps.size() / num_phases, 0.0));
    size_t phase_length = filter_taps.size() / num_phases;

    for (size_t phase = 0; phase < num_phases; ++phase) {
        for (size_t tap = 0; tap < phase_length; ++tap) {
            bank[phase][tap] = filter_taps[phase + tap * num_phases];
        }
    }
    return bank;
}

// Filter Coefficient Design (Low-Pass Filter for Timing Recovery)
std::vector<double> pi4dqpsk_demod::design_filter(size_t num_taps, double symbol_rate, double sampling_rate) {
    std::vector<double> taps(num_taps, 0.0);
    double normalized_cutoff = symbol_rate / (sampling_rate / 2);  // Nyquist normalized cutoff
    double midpoint = static_cast<double>(num_taps - 1) / 2.0;

    for (size_t i = 0; i < num_taps; ++i) {
        double n = static_cast<double>(i) - midpoint;
        if (n == 0.0) {
            taps[i] = 2 * normalized_cutoff;
        } else {
            taps[i] = sin(2 * M_PI * normalized_cutoff * n) / (M_PI * n);
        }
        taps[i] *= 0.42 - 0.5 * cos(2 * M_PI * i / (num_taps - 1)) + 0.08 * cos(4 * M_PI * i / (num_taps - 1));  // Hamming window
    }
    return taps;
}

// Implementation of CMA Equalizer
void  pi4dqpsk_demod::cma_equalizer(const std::vector<std::complex<double>>& input_signal, std::vector<std::complex<double>>& output_signal) {
    const int equalizer_taps = 15;  // local variable, default to 15 taps
    // Assert that the input signal size is larger than the number of equalizer taps
    assert(input_signal.size() > equalizer_taps && "Input signal must be longer than equalizer taps");

    // Initialize the output signal size to match input signal size
    output_signal.resize(input_signal.size());

    // Initialize the equalizer coefficients (filter taps)
    std::vector<std::complex<double>> equalizer_coefficients(equalizer_taps, std::complex<double>(1.0, 0.0));  // Example, can be customized

    // Loop over the input signal for equalizer processing
    for (size_t i = 0; i < input_signal.size(); ++i) {
        std::complex<double> output_sample(0.0, 0.0);
        double modulus;

        // Perform CMA equalizer calculation
        // Apply the filter coefficients to the input signal (simple convolution)
        for (int tap = 0; tap < equalizer_taps; ++tap) {
            if (i - tap >= 0) {
                output_sample += equalizer_coefficients[tap] * input_signal[i - tap];
            }
        }

        // Calculate the modulus (magnitude) of the output sample
        modulus = std::abs(output_sample);  // Or use std::norm(output_sample) for squared magnitude

        // Compute the error based on modulus
        double error = std::norm(output_sample) - modulus;  // Error calculation

        // Update the equalizer coefficients using the CMA rule
        for (int tap = 0; tap < equalizer_taps; ++tap) {
            if (i - tap >= 0) {
                equalizer_coefficients[tap] += 0.1 * error * std::conj(input_signal[i - tap]);  // Example step size (0.1)
            }
        }

        // Store the equalized output sample
        output_signal[i] = output_sample;
    }
}

// Initialize Equalizer Taps
std::vector<std::complex<double>> pi4dqpsk_demod::initialize_taps(size_t num_taps) {
    std::vector<std::complex<double>> taps(num_taps, std::complex<double>(0.0, 0.0));
    taps[num_taps / 2] = std::complex<double>(1.0, 0.0);  // Center tap initialized to 1
    return taps;
}

// Implementation of Differential Phasor Decoder
void pi4dqpsk_demod::diff_phasor_decoder(const std::vector<std::complex<double>>& input_signal, std::vector<int>& decoded_symbols){

    // Ensure the input signal is non-empty
    if (input_signal.empty()) {
        return;
    }

    // Initialize reference phase from the first symbol
    std::complex<double> prev_sample = input_signal[0];
    double prev_phase = std::arg(prev_sample);

    // Iterate over the signal to compute phase differences
    for (size_t i = 1; i < input_signal.size(); ++i) {
        // Current sample
        std::complex<double> curr_sample = input_signal[i];
        double curr_phase = std::arg(curr_sample);

        // Compute phase difference
        double phase_diff = curr_phase - prev_phase;

        // Normalize phase difference to the range [-Pi, Pi]
        while (phase_diff > M_PI) {
            phase_diff -= 2 * M_PI;
        }
        while (phase_diff < -M_PI) {
            phase_diff += 2 * M_PI;
        }

        // Decode phase difference into a symbol
        int symbol = decode_dqpsk_symbol(phase_diff);
        decoded_symbols.push_back(symbol);

        // Update the reference phase
        prev_phase = curr_phase;
    }

    return;
}

// Helper Function: Map phase difference to Pi/4 DQPSK symbol
int pi4dqpsk_demod::decode_dqpsk_symbol(double phase_diff) {
    // DQPSK constellation phase mappings:
    // 00 -> Pi/4, 01 -> 3Pi/4, 11 -> -3Pi/4, 10 -> -Pi/4
    if (phase_diff > 0 && phase_diff <= M_PI / 2) {
        return 0b00;  // Pi/4
    } else if (phase_diff > M_PI / 2 && phase_diff <= M_PI) {
        return 0b01;  // 3Pi/4
    } else if (phase_diff > -M_PI && phase_diff <= -M_PI / 2) {
        return 0b10;  // -Pi/4
    } else {
        return 0b11;  // -3Pi/4
    }
}


// Integrate the entire processing pipeline with parallelism
void pi4dqpsk_demod::process(const std::vector<std::complex<double>>& input_signal, std::vector<int>& decoded_symbols) {
    std::vector<std::complex<double>> sig_after_decim;
    std::vector<std::complex<double>> sig_after_fll;
    std::vector<std::complex<double>> sig_after_sync;
    std::vector<std::complex<double>> signal_after_equalization;

    std::cout << "\nBefore demod::process Step 1." << std::endl;

    // Step 1: Frequency Translation and Decimation
    auto decimation_task = std::async(std::launch::async, &pi4dqpsk_demod::freq_xlating_decim, this, std::ref(input_signal), std::ref(sig_after_decim));
    // Wait for decimation to finish
    decimation_task.get();
    
    std::cout << "\nProcess decimation_task done." << std::endl;

    // Step 2: Frequency Lock Loop (FLL)
    auto fll_task = std::async(std::launch::async, &pi4dqpsk_demod::freq_lock_loop, this, std::ref(sig_after_decim), std::ref(sig_after_fll));
    // Wait for FLL to finish
    fll_task.get();

    std::cout << "\nProcess fll_task done." << std::endl;


    // Step 3: Timing Synchronization
    auto sync_task = std::async(std::launch::async, &pi4dqpsk_demod::timing_sync, this, std::ref(sig_after_fll), std::ref(sig_after_sync));
    // Wait for synchronization to finish
    sync_task.get();

    std::cout << "\nProcess sync_task done." << std::endl;

    // Step 4: CMA Equalization
    auto cma_task = std::async(std::launch::async, &pi4dqpsk_demod::cma_equalizer, this, std::ref(sig_after_sync), std::ref(signal_after_equalization));
    // Wait for equalization to finish
    cma_task.get();

    std::cout << "\nProcess cma_task done." << std::endl;


    // Step 5: Differential Phasor Decoder
    diff_phasor_decoder(signal_after_equalization, decoded_symbols);

    std::cout << "\nProcess diff_phasor_decoder done." << std::endl;

    // Log intermediate results for debugging
    log_intermediate_results(sig_after_decim, sig_after_fll, sig_after_sync, signal_after_equalization);
}

// Log intermediate results for debugging purposes
void pi4dqpsk_demod::log_intermediate_results(const std::vector<std::complex<double>>& sig_after_decim,
                                              const std::vector<std::complex<double>>& sig_after_fll,
                                              const std::vector<std::complex<double>>& sig_after_sync,
                                              const std::vector<std::complex<double>>& signal_after_equalization) {
    std::cout << "Signal after Decimation: " << sig_after_decim.size() << " samples\n";
    std::cout << "Signal after FLL: " << sig_after_fll.size() << " samples\n";
    std::cout << "Signal after Timing Sync: " << sig_after_sync.size() << " samples\n";
    std::cout << "Signal after CMA Equalization: " << signal_after_equalization.size() << " samples\n";
}
