
#include <iostream>
#include <vector>
#include <complex>
#include <fftw3.h>
#include <cmath>
#include <numeric>
#include <filesystem>
#include <fstream>
#include <chrono>

namespace fs = std::filesystem;

// Apply a Hamming window to a complex array
void applyHammingWindow(std::vector<std::complex<double>>& data) {
    size_t N = data.size();
    for (size_t i = 0; i < N; ++i) {
        double hamming = 0.54 - 0.46 * std::cos(2 * M_PI * i / (N - 1));
        data[i] *= hamming;
    }
}

// Process a single block of data
void processBlock(const std::vector<uint8_t>& block, size_t blockSize, size_t subBlockSize) {
    size_t numSubBlocks = blockSize / subBlockSize;
    const size_t fftSize = subBlockSize / 2; // 16384 bytes -> 8192 complex numbers
    std::vector<double> powerAverage(fftSize, 0.0);

    for (size_t subBlockIndex = 0; subBlockIndex < numSubBlocks; ++subBlockIndex) {
        // Extract sub-block
        std::vector<std::complex<double>> complexArray(fftSize);
        for (size_t i = 0; i < fftSize; ++i) {
            complexArray[i] = { (static_cast<double>(block[subBlockIndex * subBlockSize + 2 * i]) - 127.5)/127.5,
                                (static_cast<double>(block[subBlockIndex * subBlockSize + 2 * i + 1]) - 127.5)/127.5 };
        }

        // Apply window function
        applyHammingWindow(complexArray);

        // Perform FFT
        std::vector<std::complex<double>> fftOutput(fftSize);
        fftw_plan plan = fftw_plan_dft_1d(
            fftSize,
            reinterpret_cast<fftw_complex*>(complexArray.data()),
            reinterpret_cast<fftw_complex*>(fftOutput.data()),
            FFTW_FORWARD, FFTW_ESTIMATE);
        fftw_execute(plan);
        fftw_destroy_plan(plan);
        fftw_cleanup();


        // Adjust FFT output (scale for window function)
        double window_correction_factor = 0.54 * fftSize; // Adjust for Hamming window's amplitude scaling
        for (int i = 0; i < fftSize; i++) {
            fftOutput[i] /= window_correction_factor; // both Real & Imaginary parts
        }

        // Compute power spectrum
        std::vector<double> powerSpectrum(fftSize);
        for (size_t i = 0; i < fftSize; ++i) {
            powerSpectrum[i] = std::norm(fftOutput[i]); // Real^2 + Imaginary^2
        }

        // Accumulate power spectrum for averaging
        for (size_t i = 0; i < fftSize; ++i) {
            powerAverage[i] += powerSpectrum[i];
        }
    }

    // Compute average power spectrum
    for (size_t i = 0; i < fftSize; ++i) {
        powerAverage[i] /= numSubBlocks;
    }

    // Output the average power spectrum
    // std::cout << "Average Power Spectrum:" << std::endl;
    // for (size_t i = 0; i < fftSize; ++i) {
    //     std::cout << "Freq[" << i << "] = " << powerAverage[i] << std::endl;
    // }
}

// Process all files in a folder
void iterateAndProcessFolder(const std::string& folderPath) {
    const size_t blockSize = 640 * 4096;
    const size_t subBlockSize = 16384;
    const size_t superBlockCount = 120;
    const size_t superBlockSize = blockSize * superBlockCount ;

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            const std::string filePath = entry.path().string();
            const size_t fileSize = fs::file_size(entry.path());

            if (fileSize == superBlockSize) {
                std::cout << "Processing file: " << filePath << std::endl;

                // Read file into a buffer
                std::ifstream file(filePath, std::ios::binary);
                if (!file.is_open()) {
                    std::cerr << "Error: Could not open file: " << filePath << std::endl;
                    continue;
                }
                for (size_t idx=1; idx <= superBlockCount; ++idx) {
                    std::vector<uint8_t> block(blockSize);
                    file.read(reinterpret_cast<char*>(block.data()), blockSize);

                    // Process the block
                    processBlock(block, blockSize, subBlockSize);
                    std::cout << "Average Power Spectrum: " << idx << std::endl;
                }
                file.close();
            } 
            else {
                std::cout << "Skipping file (size mismatch): " << filePath << std::endl;
            }
        }
    }
}

int main() {
    const std::string folderPath = "/home/ctn008/myrtlsdr/build/abc"; // Replace with your folder path

    // Record the start time
    auto start = std::chrono::high_resolution_clock::now();

    try {
        iterateAndProcessFolder(folderPath);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    // Record the end time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    std::chrono::duration<double> duration = end - start;

    // Display the time taken
    std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;

    return 0;
}
