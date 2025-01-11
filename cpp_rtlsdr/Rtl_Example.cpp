/* 
How to compile: (need to include lrtlsdr library)
g++ -o Rtl_Example Rtl_Example.cpp RtlSdrCpp.cpp -lrtlsdr

*/

#include "RtlSdrCpp.h"
#include <iostream>
#include <vector>

RtlSdrCpp* asyncCallbackDevice;

void asyncCallback(uint8_t* buffer, uint32_t length) {
    
    static int readCount = 0;
    if (readCount < 10) {
        std::cout << "Received async data of length: " << length << std::endl;
        // Process buffer data (e.g., save to file or analyze)
        ++readCount;
    }
    else {
        // Stop asynchronous read
        asyncCallbackDevice.stopAsyncRead();

    }

}

int main() {
    RtlSdrCpp rtlDevice;
    // Open device (index 0 for the first device)
    if (!rtlDevice.openDevice(0)) {
        std::cerr << "Failed to open RTL-SDR device." << std::endl;
        return 1;
    }
    
    // Reference asyncCallbackDevice to rtlDevice in order to be accessed from asyncCallback function.
    asyncCallbackDevice = &rtlDevice;

    // Set center frequency to 100 MHz
    if (!rtlDevice.setCenterFrequency(100000000)) {
        std::cerr << "Failed to set center frequency." << std::endl;
        return 1;
    }
    std::cout << "Center frequency set to: " << rtlDevice.getCenterFrequency() << " Hz" << std::endl;

    // Set sample rate to 2.4 MSPS
    if (!rtlDevice.setSampleRate(2560000)) {
        std::cerr << "Failed to set sample rate." << std::endl;
        return 1;
    }
    std::cout << "Sample rate set to: " << rtlDevice.getSampleRate() << " Hz" << std::endl;

    // Set gain mode to manual and gain to 49.6 dB
    if (!rtlDevice.setGainMode(true) || !rtlDevice.setGain(496)) {
        std::cerr << "Failed to set gain." << std::endl;
        return 1;
    }
    std::cout << "Gain set to: " << rtlDevice.getGain()/10.0 << " dB" << std::endl;

    rtlDevice.resetBuffer();
    // Read synchronously (example with 16 KB buffer)
    std::vector<uint8_t> buffer(16384);
    int bytesRead = rtlDevice.readSamples(buffer.data(), buffer.size());
    if (bytesRead > 0) {
        std::cout << "Synchronously read " << bytesRead << " bytes." << std::endl;
    } else {
        std::cerr << "Synchronous read failed." << std::endl;
    }

    // Start asynchronous read
    std::cout << "Start aynchronous read" << std::endl;
    if (!rtlDevice.startAsyncRead(asyncCallback)) {
        std::cerr << "Failed to start asynchronous read." << std::endl;
        return 1;
    }

    std::cout << "Async read cancelled." << std::endl;

    // Close device
    rtlDevice.closeDevice();
    std::cout << "RTL-SDR device closed." << std::endl;

    return 0;
}
