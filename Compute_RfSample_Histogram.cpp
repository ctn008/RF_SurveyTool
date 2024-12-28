#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <chrono>

int main() {
    std::cout << "Calculating histogram..."  << std::endl;
    // Record the start time
    auto start = std::chrono::high_resolution_clock::now();

    const std::string inputFileName = "/home/ctn008/myrtlsdr/build/abc/1734852476.uint8"; // Input binary file
    const std::string outputFileName = "/home/ctn008/myrtlsdr/build/abc/1734852476_histogram.int16"; // Output text file
    const size_t chunkSize = 16384; // Length of each chunk
    const int histogramSize = 256; // Covers range [0, 255]
    std::vector<uint8_t> buffer(chunkSize);

    // Open the binary file
    std::ifstream inputFile(inputFileName, std::ios::binary);
    if (!inputFile) {
        std::cerr << "Error: Unable to open file " << inputFileName << std::endl;
        return 1;
    }

    // Open the output file
    std::ofstream outputFile(outputFileName, std::ios::binary);
    if (!outputFile) {
        std::cerr << "Error: Unable to open file " << outputFileName << std::endl;
        return 1;
    }

    // Process the file recursively, chunk by chunk
    std::streamsize bytesRead;
    while (true) {
        // Read up to chunkSize elements from the file
        inputFile.read(reinterpret_cast<char*>(buffer.data()), chunkSize);
        bytesRead = inputFile.gcount();

        if (bytesRead < chunkSize) {
            // Ignore if less than chunkSize elements
            break;
        }

        std::vector<int16_t> histogram(histogramSize, 0); // Initialize all bins to 0
        // Call calculateHistogram on the current chunk
        for (int i=0; i < buffer.size(); ++i) {
            histogram[ buffer[i] ]++;
        }

        // Write the partial histogram to the output file in binary format
        outputFile.write(reinterpret_cast<const char*>(histogram.data()), histogram.size() * sizeof(int16_t));

    }

    // Close the files
    inputFile.close();
    outputFile.close();

    // Record the end time
    auto end = std::chrono::high_resolution_clock::now();

    // Calculate the duration
    std::chrono::duration<double> duration = end - start;

    // Display the time taken
    std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;

    return 0;
}
