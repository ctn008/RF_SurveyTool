/*
g++ -std=c++17 -I/usr/src/gtest/include -L/usr/src/gtest/lib -o test_freq_xlating test_freq_xlat_decim.cpp pi4dqpskDemod.cpp -lgtest -lgtest_main -pthread
*/

#include <gtest/gtest.h>
#include <vector>
#include <complex>
#include <cmath>
#include <fstream>
#include <iostream>
#include "pi4dqpskDemod.h" // Replace with the actual header file containing your functions

// Tolerance for floating-point comparisons
const double EPSILON = 1e-6;


void writeComplexSignalToFile(const std::vector<std::complex<double>>& signal, const std::string& filename) {
    // Convert signal from complex<double> to complex<float>
    std::vector<std::complex<float>> floatSignal(signal.size());
    for (size_t i = 0; i < signal.size(); ++i) {
        floatSignal[i] = std::complex<float>(static_cast<float>(signal[i].real()), static_cast<float>(signal[i].imag()));
    }

    // Open the file for binary output
    std::ofstream outFile(filename, std::ios::binary);
    if (!outFile) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    // Write the data to the file
    outFile.write(reinterpret_cast<const char*>(floatSignal.data()), floatSignal.size() * sizeof(std::complex<float>));
    if (!outFile) {
        std::cerr << "Error: Writing to file " << filename << " failed." << std::endl;
    }

    outFile.close();
}

// Test Case: Valid input signal and parameters
TEST(FreqXlatingDecimTest, ValidInput) {
    // Input parameters
    double sampling_rate = 100e3;    // 100 kHz
    double down_sampling_rate = 20e3; // 10 kHz
    double freq_xlating = 4e3;       // 5 kHz
    int signal_length = 16384;
    int signal_freq = 1e3; // 1 kHz
    std::vector<std::complex<double>> input_signal(signal_length);
    std::vector<std::complex<double>> xlat_signal(signal_length);

    // Generate a simple sinusoidal input signal
    for (size_t n = 0; n < signal_length; ++n) {
        double phase = 2.0 * M_PI * signal_freq * n / sampling_rate; // 1 kHz tone
        double xlat_phase = 2.0 * M_PI * freq_xlating * n / sampling_rate; // 1 kHz tone
        input_signal[n] = std::exp(std::complex<double>(0.0, phase));
        xlat_signal[n] = input_signal[n] * std::exp(std::complex<double>(0.0, xlat_phase));
    }
    writeComplexSignalToFile(input_signal, "input_signal.complex64");
    writeComplexSignalToFile(xlat_signal, "xlat_signal.complex64");

    // Expected output size
    size_t decimation_factor = static_cast<size_t>(sampling_rate / down_sampling_rate);
    ASSERT_GT(decimation_factor, 0);

    // Expeted output signal
    double err = 1.0;
    std::vector<std::complex<double>> xlat_decim_signal(signal_length/decimation_factor);
    for (size_t n = 0; n < xlat_decim_signal.size(); ++n) {
        double phase = 2.0 * M_PI * signal_freq * n / down_sampling_rate; // 1 kHz tone
        xlat_decim_signal[n] = std::complex<double>(std::cos(phase), std::sin(phase)) * std::exp(std::complex<double>(0.0, phase*err));
    }
    writeComplexSignalToFile(xlat_decim_signal, "xlat_decim_signal.complex64");

    // Output signal
    std::vector<std::complex<double>> output_signal;

    // Call the function
    pi4dqpsk_demod demod; // Assuming this is a class containing the function
    demod.sampling_rate = sampling_rate;
    demod.down_sampling_rate = down_sampling_rate;
    demod.freq_xlating = -freq_xlating;
    demod.freq_xlating_decim(input_signal, output_signal);

    writeComplexSignalToFile(output_signal, "decimed_signal.complex64");

    // Check the output size
    ASSERT_EQ(output_signal.size(), input_signal.size() / decimation_factor);

    // Check that output is not all zeros (basic validity check)
    double output_magnitude_sum = 0.0;
    for (const auto& sample : output_signal) {
        output_magnitude_sum += std::abs(sample);
    }
    ASSERT_GT(output_magnitude_sum, EPSILON);

    // Check that output value against expected output value
    std::cout << "Checking output signal against expected output signal" << std::endl;
    // print out contents of output_signal and xlating_decim_signal for comparison
    std::cout << "output_signal.size() = " << output_signal.size() << std::endl;
    std::cout << "xlat_decim_signal.size() = " << xlat_decim_signal.size() << std::endl;
    for (size_t n = 0; n < output_signal.size(); ++n) {
        std::cout << "output_signal[" << n << "] = " << /*out*/input_signal[n] << " ";
        std::cout << "xlat_decim_signal[" << n << "] = " << xlat_decim_signal[n] << std::endl;
    }


    double output_magnitude_diff = 0.0;
    for (size_t n = 0; n < output_signal.size(); ++n) {
        std::cout << "output_signal[" << n << "] = " << std::abs(output_signal[n]) << " ";
        std::cout << "xlat_decim_signal[" << n << "] = " << std::abs(xlat_decim_signal[n]) << std::endl;
        output_magnitude_diff = std::abs(output_signal[n]) - std::abs(xlat_decim_signal[n]);
        ASSERT_GT(output_magnitude_diff, EPSILON);
    }
}

// Test Case: Invalid decimation factor
TEST(FreqXlatingDecimTest, InvalidDecimationFactor) {
    // Input parameters
    double sampling_rate = 100e3;    // 100 kHz
    double down_sampling_rate = 200e3; // Invalid rate
    double freq_xlating = 5e3;       // 5 kHz
    std::vector<std::complex<double>> input_signal(1000, {1.0, 0.0}); // Dummy signal

    // Output signal
    std::vector<std::complex<double>> output_signal;

    // Call the function
    pi4dqpsk_demod demod;
    demod.sampling_rate = sampling_rate;
    demod.down_sampling_rate = down_sampling_rate;
    demod.freq_xlating = freq_xlating;
    demod.freq_xlating_decim(input_signal, output_signal);

    // Check that the output signal remains empty
    ASSERT_TRUE(output_signal.empty());
}

// Test Case: Empty input signal
TEST(FreqXlatingDecimTest, EmptyInputSignal) {
    // Input parameters
    double sampling_rate = 100e3;    // 100 kHz
    double down_sampling_rate = 10e3; // 10 kHz
    double freq_xlating = 5e3;       // 5 kHz
    std::vector<std::complex<double>> input_signal; // Empty signal

    // Output signal
    std::vector<std::complex<double>> output_signal;

    // Call the function
    pi4dqpsk_demod demod;
    demod.sampling_rate = sampling_rate;
    demod.down_sampling_rate = down_sampling_rate;
    demod.freq_xlating = freq_xlating;
    demod.freq_xlating_decim(input_signal, output_signal);

    // Check that the output signal remains empty
    ASSERT_TRUE(output_signal.empty());
}

// Main function for running all tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
