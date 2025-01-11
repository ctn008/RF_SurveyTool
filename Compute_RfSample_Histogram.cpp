#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <chrono>
#include <string>
#include <cstring> // For strcmp

int main(int argc, char *argv[]) {
    std::cout << "Calculating histogram..."  << std::endl;
    // Record the start time
    auto start = std::chrono::high_resolution_clock::now();

    std::string filename;
    // Check if the number of arguments is sufficient
    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " -f <filename>" << std::endl;
        return 1;
    }

    // Iterate through the arguments to find the "-f" flag
    bool file_flag = false;
    for (int i = 1; i < argc - 1; ++i) {
        if (std::strcmp(argv[i], "-f") == 0) { // Check if the argument is "-f"
            filename = argv[i + 1]; // Assign the next argument as the filename
            file_flag = true;
        }
    }

    // If the "-f" flag was not found
    if (!file_flag) {
        std::cerr << "Error: Missing or invalid '-f' flag" << std::endl;
        std::cout << "Usage: " << argv[0] << " -f <filename>" << std::endl;
        return 1;
    }


    const std::string inputFileName = filename; // Input binary file
    const std::string outputFileName = filename + "_histogram_int16"; // Output text file
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
