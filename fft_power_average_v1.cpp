
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
void processBlock(const std::vector<uint8_t>& block, size_t blockSize, size_t subBlockSize, fftw_plan& fft_plan, 
    std::vector<std::complex<double>>& complexArray, std::vector<std::complex<double>>& fftOutput, std::vector<double>& powerAverage) {
    // Declare static to optimize for speed - no need to initialize these variables for every call
    // Assume these variables do not change value for all processBlock call
    
    static const size_t numSubBlocks = blockSize / subBlockSize;
    static const size_t fftSize = subBlockSize / 2; // 16384 bytes -> 8192 complex numbers
    static const double window_correction_factor = 0.54 * fftSize; // Adjust for Hamming window's amplitude scaling
    static std::vector<double> powerSpectrum(fftSize);

    // reset powerAverage value to 0.0 at  begining of any block
    for (size_t i = 0; i < fftSize; ++i) { powerAverage[i] = 0.0; }

    for (size_t subBlockIndex = 0; subBlockIndex < numSubBlocks; ++subBlockIndex) {
        // Extract sub-block to complexArray
        for (int i = 0; i < fftSize; ++i) {
            complexArray[i] = { (static_cast<double>(block[subBlockIndex * subBlockSize + 2 * i]) - 127.5)/127.5,
                                (static_cast<double>(block[subBlockIndex * subBlockSize + 2 * i + 1]) - 127.5)/127.5 };
        }

        // Apply window function
        applyHammingWindow(complexArray);

        // Perform FFT - result is stored in fftOutput
        fftw_execute(fft_plan);

        // Adjust FFT output (scale for window function)
        for (int i = 0; i < fftSize; i++) {
            fftOutput[i] /= window_correction_factor; // both Real & Imaginary parts
        }

        // Compute power spectrum
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

    // Initialize FFT
    const size_t fftSize = subBlockSize / 2; // 16384 bytes -> 8192 complex numbers
    std::vector<std::complex<double>> complexArray(fftSize);
    std::vector<std::complex<double>> fftOutput(fftSize);
    std::vector<double> powerAverage(fftSize);

    fftw_plan fft_plan = fftw_plan_dft_1d(
        fftSize,
        reinterpret_cast<fftw_complex*>(complexArray.data()),
        reinterpret_cast<fftw_complex*>(fftOutput.data()),
        FFTW_FORWARD, FFTW_PATIENT);   //  FFTW_MEASURE or FFTW_PATIENT

    // Check if the folder exists
    if (!fs::exists(folderPath)) {
        std::cerr << "Error: The folder '" << folderPath << "' does not exist." << std::endl;
        return;
    }

    std::string filePath = folderPath;
    // Check if the folder path has a trailing separator, and add it if missing
    if (filePath.back() != '/' && filePath.back() != '\\') {
        filePath += "/";  // Append '/' for Unix-like systems
        // filePath += '\\';  // Uncomment for Windows if needed
    }

    // Combine folder path with the file name to create the full file path
    filePath += "fft_power.double";
    // Open the file to save powerAverage data (overwriting existing content)
    std::ofstream outFile(filePath, std::ios::binary | std::ios::trunc);

    if (!outFile) {
        std::cerr << "Error: Unable to open file for writing fft_power." << std::endl;
        return;
    }


    // Vector to store eligible files
    std::vector<fs::directory_entry> eligibleFiles;

    // Iterate through the files in the folder
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            const auto& path = entry.path();
            // Check file extension and size
            if (path.extension() == ".uint8" && fs::file_size(path) == FILE_SIZE) {
                eligibleFiles.push_back(entry);
            }
        }
    }

    // Sort files by creation time
    std::sort(eligibleFiles.begin(), eligibleFiles.end(),
              [](const fs::directory_entry& a, const fs::directory_entry& b) {
                  return fs::last_write_time(a) < fs::last_write_time(b);
              });

    // Open and read each file in binary mode
    for (const auto& file : eligibleFiles) {
        std::cout << "Processing file: " << file.path() << std::endl;

        // Open file in binary mode
        std::ifstream inFile(file.path(), std::ios::binary);



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
                    processBlock(block, blockSize, subBlockSize, fft_plan, complexArray, fftOutput, powerAverage);
                    std::cout << "Average Power Spectrum: " << idx << std::endl;
                    // Write the powerAverage of each block to fft_power.double file
                    outFile.write(reinterpret_cast<const char*>(powerAverage.data()), powerAverage.size() * sizeof(double));
                }
                file.close();
            } 
            else {
                std::cout << "Skipping file (size mismatch): " << filePath << std::endl;
            }
        }
    }

    // Close fft_power.double file
    outFile.close();
    if (!outFile) {
        std::cerr << "Error: Could not properly close the fft_power output file." << std::endl;
        return;
    }

    std::cout << "Data successfully written to 'data.bin'." << std::endl;

    // Shutdown FFT
    fftw_destroy_plan(fft_plan);
    fftw_cleanup();

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
