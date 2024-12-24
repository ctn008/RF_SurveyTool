
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

void processFile(const std::string& filepath) {
    const size_t blockSize = 640 * 4096; // Size of each block
    std::ifstream file(filepath, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << filepath << std::endl;
        return;
    }

    // Allocate memory for a block using uint8_t
    std::vector<uint8_t> block(blockSize);

    // Read the file in blocks and process
    size_t blockNumber = 0;
    while (file.read(reinterpret_cast<char*>(block.data()), blockSize) || file.gcount() > 0) {
        std::cout << "Processing block " << blockNumber << " of file: " << filepath << std::endl;

        // Perform calculations or processing on the block data
        // Example: Calculate the sum of bytes in the block
        size_t sum = 0;
        for (size_t i = 0; i < file.gcount(); ++i) {
            sum += block[i];
        }
        std::cout << "Sum of bytes in block " << blockNumber << ": " << sum << std::endl;

        ++blockNumber;
    }

    file.close();
}

void iterateAndProcessFolder(const std::string& folderPath) {
    const size_t targetFileSize = 640 * 4096 * 120; // Target file size in bytes

    for (const auto& entry : fs::directory_iterator(folderPath)) {
        if (entry.is_regular_file()) {
            const std::string filePath = entry.path().string();
            const size_t fileSize = fs::file_size(entry.path());

            if (fileSize == targetFileSize) {
                std::cout << "Processing file: " << filePath << std::endl;
                processFile(filePath);
            } else {
                std::cout << "Skipping file (size mismatch): " << filePath << std::endl;
            }
        }
    }
}

int main() {
    const std::string folderPath = "/home/ctn008/myrtlsdr/build/abc"; // Replace with your folder path

    try {
        iterateAndProcessFolder(folderPath);
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
