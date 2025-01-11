#include <vector>
#include <complex>
#include <cmath>
#include <thread>
#include <future>
#include <mutex>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <cassert>  // For assert

double hamming(int n, int N);
int compute_ntaps(double sampling_rate, double transition_width);
std::vector<double> calculate_filter_coefficients(double sampling_rate, double cutoff_freq, double transition_width);
std::vector<double> designLowPassFilter(double sample_rate, double cutoff_freq, double transition_width);
// Define pi4dqpsk_demod class
class pi4dqpsk_demod {
public:
    // Member variables
    double sampling_rate;       // in samples per second (SPS)
    double down_sampling_rate;  // in SPS
    double symbol_rate;         // in SPS
    double freq_xlating;   // in Hz

    // Constructor to initialize the class
    pi4dqpsk_demod(double samp_rate, double down_samp_rate, double sym_rate, double freq_xlate)
        : sampling_rate(samp_rate), down_sampling_rate(down_samp_rate), symbol_rate(sym_rate), freq_xlating(freq_xlate) {}

    // Function prototypes
    void freq_xlating_decim(const std::vector<std::complex<double>>& input_signal,
                                              std::vector<std::complex<double>>& output_signal);
    void rational_resampler(const std::vector<std::complex<double>>& input_signal,
                            std::vector<std::complex<double>>& output_signal, int interp_factor, int decim_factor);
    void freq_lock_loop(const std::vector<std::complex<double>>& input_signal,
                             std::vector<std::complex<double>>& output_signal);
    void timing_sync(const std::vector<std::complex<double>>& input_signal, std::vector<std::complex<double>>& output_signal);
    void cma_equalizer(const std::vector<std::complex<double>>& input_signal, std::vector<std::complex<double>>& output_signal);
    void diff_phasor_decoder(const std::vector<std::complex<double>>& input_signal, std::vector<int>& decoded_symbols);

    // Integrate the entire pipeline
    void process(const std::vector<std::complex<double>>& input_signal, std::vector<int>& decoded_symbols);

    // Log intermediate results
    void log_intermediate_results(const std::vector<std::complex<double>>& signal_after_decimation,
                                  const std::vector<std::complex<double>>& signal_after_fll,
                                  const std::vector<std::complex<double>>& signal_after_sync,
                                  const std::vector<std::complex<double>>& signal_after_equalization);
private:
    // Private helper methods
    std::vector<std::complex<double>> matched_filter(const std::vector<std::complex<double>>& signal, const std::vector<double>& coefficients);
    std::vector<double> calculate_rrc_filter(double rolloff, double symbol_rate, double sampling_rate, size_t filter_length);
    std::vector<std::vector<double>> polyphase_filter_bank(const std::vector<double>& filter_taps, size_t num_phases);
    std::vector<double> design_filter(size_t num_taps, double symbol_rate, double sampling_rate);
    std::vector<std::complex<double>> initialize_taps(size_t num_taps);
    int decode_dqpsk_symbol(double phase_diff);
};

