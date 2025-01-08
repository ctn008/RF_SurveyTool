#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>
#include <atomic>
#include <csignal>
#include <rtl-sdr.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#endif
#include <stdexcept>
#include <array>
#include "RtlSdrCpp.h"

std::atomic<bool> keepRunning(true);

#ifdef _WIN32
BOOL WINAPI consoleHandler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        keepRunning = false;
        return TRUE;
    }
    return FALSE;
}
#else
void signalHandler(int signal) {
    if (signal == SIGINT) {
        keepRunning = false;
    }
}
#endif

void readRtlSdrAndWriteData(RtlSdr& rtlSdr, const std::string& baseFilename) {
    const int blockSize = 16384;          // Size of each block (16384 bytes)
    const int blocksPerFile = 160 * 16; //120;  // Number of blocks in one file (160 * 120 blocks)
    std::cout << "Reading by block: " << blockSize << " for: " << blocksPerFile << " blocks per file." << std::endl;

    int blockCount = 0;
    int fileIndex = 1;  // File index for generating sequential filenames

    // Function to generate a timestamped filename with an index
    auto generateFilename = [baseFilename, &fileIndex]() -> std::string {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm = *std::localtime(&now_time);

        std::ostringstream filename;
        filename << baseFilename << "_" << fileIndex << "_"
                 << std::put_time(&now_tm, "%Y-%m-%d_%H-%M-%S") << ".dat";
        fileIndex++;
        return filename.str();
    };

    // Ensure the buffer is reset before starting to read data
    rtlSdr.resetBuffer();

    // Start reading and writing data
    std::ofstream file;
    std::string filename;

    std::vector<int> blockAgcMag;
    std::vector<int> blockAgcIdx;
    blockAgcMag.reserve(blocksPerFile);
    blockAgcIdx.reserve(blocksPerFile);
    std::vector<uint8_t> buffer(blockSize);
    
    while (keepRunning) {
        // Generate a new filename when blockCount is a multiple of blocksPerFile
        // if (blockCount % blocksPerFile == 0 && blockCount > 0) {
        if (blockCount == 0) {
            // Open a new file
            filename = generateFilename();
            file.open(filename, std::ios::binary);

            if (!file.is_open()) {
                std::cerr << "Failed to open file: " << filename << std::endl;
                return;
            }

            std::cout << "Writing to file: " << filename << std::endl;
        }
        // Write block of data to the current file being opened
        rtlSdr.readSamplesAgc(blockSize, buffer, blockAgcMag, blockAgcIdx);
        file.write(reinterpret_cast<char*>(buffer.data()), buffer.size());
        blockCount++;  // Increment block count for each block written


        if (blockCount == blocksPerFile) {
            // Close the current file; reset blockCount
            file.close();
            if (!file) {
                std::cerr << "Error closing file: " << filename << std::endl;
            }
            blockCount = 0;
            // print the AGC values
            // for (int i = 0; i < blockAgcMag.size(); i++) {
            //     std::cout << "Block " << i << " AGC Mag: " << blockAgcMag[i] << " AGC Idx: " << blockAgcIdx[i] << std::endl;
            // }
            std::cout << "Agc Mag size " << blockAgcMag.size() << " AGC Idx size: " << blockAgcIdx.size() << std::endl;            
            blockAgcMag.clear();
            blockAgcIdx.clear();
        }
    }
    if (file.is_open()) {
        // Close the current file, if while loop is terminated while file is still opening.
        file.close();
        if (!file) {
            std::cerr << "Error closing file: " << filename << std::endl;
        }
    }
}



int main(int argc, char* argv[]) {
#ifdef _WIN32
    SetConsoleCtrlHandler(consoleHandler, TRUE);  // Register console handler for Windows
#else
    signal(SIGINT, signalHandler);  // Register signal handler for Ctrl+C (Linux/Unix)
#endif

    uint32_t freq = 391250000;  // Default frequency (100 MHz)
    if (argc > 2 && std::string(argv[1]) == "-f") {
        freq = std::stoul(argv[2]);
    }

    try {
        RtlSdr rtlSdr1;
        RtlSdr rtlSdr2;

        // Open RTL-SDR devices
        rtlSdr1.open(0);
//        rtlSdr2.open(1);

        // Set frequencies for both devices
        rtlSdr1.setCenterFrequency(freq);
//        rtlSdr2.setCenterFrequency(freq);

        // Set sample rate and gain for both devices
        rtlSdr1.setSampleRate(2560000);  // Default sample rate
//        rtlSdr2.setSampleRate(2048000);  // Default sample rate
        rtlSdr1.setGainMode(true);      // Manual gain mode
//        rtlSdr2.setGainMode(true);      // Manual gain mode

        // Run reading and writing data in separate threads for each device
        std::string baseFilename1 = "rtlSdr_device1";
        std::string baseFilename2 = "rtlSdr_device2";

        std::cout << "RtlSdr Tuner Type: " << rtlSdr1.getTunerType() << std::endl;
        std::cout << "Reading rtl-sdr at sampling rate: " << rtlSdr1.getSampleRate() << std::endl;
//        std::thread thread1(readRtlSdrAndWriteData, std::ref(rtlSdr1), baseFilename1);
//        std::thread thread2(readRtlSdrAndWriteData, std::ref(rtlSdr2), baseFilename2);

        readRtlSdrAndWriteData(std::ref(rtlSdr1), baseFilename1);
        // Stop threads and clean up
//        thread1.join();
//        thread2.join();

        // Step 5: Close devices, files, and threads
        std::cout << "Shutting down...\n";

        // Close devices explicitly
        rtlSdr1.close();
//        rtlSdr2.close();

        std::cout << "Devices and files closed. Program terminated.\n";

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}


