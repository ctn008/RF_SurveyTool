/*
include "RtlSdrCpp.h"
compile to object file, no linking
g++ -c RtlSdrCpp.cpp -o RtlSdrCpp.o -ID:\myrtlsdr\include
*/

#include "RtlSdrCpp.h"
#include <cstring>
#include <algorithm>
#include <iostream>

RtlSdr::RtlSdr() : device(nullptr), sampleRate(2560000), centerFreq(100000000), gain(166), manualAgcIdx(10), devIndex(0), gainMode(1) {}
// default sample rate = 2560000, default center frequency = 100000000, default gain = 166, default device index = 0, default gain mode = 1
RtlSdr::~RtlSdr() {
    if (device) {
        rtlsdr_close(device);
    }
}

void RtlSdr::open(int index) {
    devIndex = index;
    if (rtlsdr_open(&device, devIndex) < 0) {
        throw std::runtime_error("Failed to open RTL-SDR device.");
    }
    if ((getTunerType() == RTLSDR_TUNER_R820T) || (getTunerType() == RTLSDR_TUNER_R828D)) {
        manualAgcEnabled = 1;
    }
    else {
        manualAgcEnabled = 0;
    }
}

void RtlSdr::close() {
    if (rtlsdr_close(device) < 0) {
        throw std::runtime_error("Failed to close RTL-SDR device.");
    }
}

void RtlSdr::setCenterFrequency(uint32_t freq) {
    if (!device) {
        throw std::runtime_error("Device is not opened.");
    }
    centerFreq = freq;
    if (rtlsdr_set_center_freq(device, centerFreq) < 0) {
        throw std::runtime_error("Failed to set center frequency.");
    }
}

uint32_t RtlSdr::getCenterFrequency() const {
    if (!device) {
        throw std::runtime_error("Device is not opened.");
    }
    return rtlsdr_get_center_freq(device);
}

void RtlSdr::setSampleRate(uint32_t rate) {
    if (!device) {
        throw std::runtime_error("Device is not opened.");
    }
    sampleRate = rate;
    if (rtlsdr_set_sample_rate(device, sampleRate) < 0) {
        throw std::runtime_error("Failed to set sample rate.");
    }
}

uint32_t RtlSdr::getSampleRate() const {
    if (!device) {
        throw std::runtime_error("Device is not opened.");
    }
    return rtlsdr_get_sample_rate(device);
}

// Method to set the gain mode (auto or manual)
void RtlSdr::setGainMode(bool automatic) {
    if (!device) {
        throw std::runtime_error("Device is not opened.");
    }

    gainMode = automatic;  // 0 for auto gain, 1 for manual gain

    if (rtlsdr_set_tuner_gain_mode(device, gainMode) < 0) {
        throw std::runtime_error("Failed to set gain mode.");
    }

    if (gainMode) {
        gain = r82xx_gains[manualAgcIdx];
        setGain(gain); // If in manual mode, set the default manual gain
    }
}

// Method to set the manual gain, only effective when gain mode is manual
void RtlSdr::setGain(int gainValue) {
    if (!device) {
        throw std::runtime_error("Device is not opened.");
    }

    if (gainMode == 0) {
        throw std::runtime_error("Cannot set manual gain while in automatic gain mode.");
    }

    if (gainValue < 0 || gainValue > MAX_GAIN) {
        throw std::invalid_argument("Invalid gain value.");
    }

    gain = gainValue;
    if (rtlsdr_set_tuner_gain(device, gain) < 0) {
        throw std::runtime_error("Failed to set tuner gain.");
    }
}

int RtlSdr::getGain() const {
    if (!device) {
        throw std::runtime_error("Device is not opened.");
    }
    return gain;
}
enum rtlsdr_tuner RtlSdr::getTunerType() {
    if (!device) {
        throw std::runtime_error("Device is not opened.");
    }
    return rtlsdr_get_tuner_type(device);
}

void RtlSdr::resetBuffer() {
    if (!device) {
        throw std::runtime_error("Device is not opened.");
    }
    if (rtlsdr_reset_buffer(device) < 0) {
        throw std::runtime_error("Failed to reset buffer.");
    }
}

void RtlSdr::readSamples(int numSamples, std::vector<uint8_t>& buffer) const {
    if (!device) {
        throw std::runtime_error("Device is not opened.");
    }
    int bytesRead = 0;
    if (rtlsdr_read_sync(device, buffer.data(), numSamples, &bytesRead) < 0) {
        throw std::runtime_error("Failed to read samples.");
    }
    if (bytesRead != static_cast<int>(numSamples)) {
        throw std::runtime_error("Incomplete read of samples.");
    }    
    return;
}

void RtlSdr::readSamplesAgc(int numSamples, std::vector<uint8_t>& buffer, std::vector<int>& blockAgcMag, std::vector<int>& blockAgcIdx) {
    if (!manualAgcEnabled) {
        throw std::runtime_error("AGC is only supported for R820T & R828D tuners.");
    }
    const int agcBlockSize = 4096;
    uint8_t* block = buffer.data();

    if (numSamples - agcBlockSize < 0) {
        throw std::runtime_error("Invalid number of samples. Must be multiple of 4096.");
    }
    // read the first agcBlockSize samples to perform agcMagControl
    int bytesRead0 = 0;
    if (rtlsdr_read_sync(device, block, agcBlockSize, &bytesRead0) < 0) {
        throw std::runtime_error("Failed to read samples.");
    }
    blockAgcMag.push_back(agcMagControl(agcBlockSize, block));
    blockAgcIdx.push_back(manualAgcIdx);
    block += agcBlockSize;

    int bytesRead1 = 0;
    // read the rest of the samples
    if (numSamples - agcBlockSize) {
        if (rtlsdr_read_sync(device, block, numSamples - agcBlockSize, &bytesRead1) < 0) {
            throw std::runtime_error("Failed to read samples.");
        }
    }
    if (bytesRead0 != agcBlockSize || bytesRead1 != numSamples - agcBlockSize) {
        throw std::runtime_error("Incomplete read of samples.");
    }
}

int RtlSdr::agcMagControl(const int agcBlockSize, uint8_t* agcBlock) {
    static constexpr int lowerCap = 80;  // 4 dB lower than upperCap
    static constexpr int upperCap = 127;
    // Reset histogram efficiently
    std::array<int, 256> histogram = {0}; 
    
    // Count occurrences of each byte in the agcBlock
    for (int i = 0; i < agcBlockSize; ++i) {
        histogram[agcBlock[i]]++;
    }

    const int zeroPointLow = 127;
    const int zeroPointHigh = 128;
    // Calculate histogram bounds of non-zero values for lower and upper magnitudes
    int lowerMag = 0, upperMag = 0;
    for (int i = 0; i < zeroPointHigh; ++i) {
        if (histogram[i]) {
            lowerMag = i;
            break;
        }
    }
    for (int i = 255; i > zeroPointLow; --i) {
        if (histogram[i]) {
            upperMag = i;
            break;
        }
    }

    // Calculate the average of the lower and upper byte index
    int avgMaxVariation = (zeroPointLow - lowerMag + upperMag - zeroPointHigh) / 2;
    if ((avgMaxVariation < lowerCap) && (manualAgcIdx < r82xx_gains.size()-1)) {
        ++manualAgcIdx;
        setGain(r82xx_gains[manualAgcIdx]);
    }
    if ((avgMaxVariation > upperCap) && (manualAgcIdx)) {
        --manualAgcIdx;
        setGain(r82xx_gains[manualAgcIdx]);
    }
    // Return the histogram average max variation (for debugging)
    return avgMaxVariation;
}

int RtlSdr::getDeviceCount() {
    return rtlsdr_get_device_count();
}

std::string RtlSdr::getDeviceName(uint32_t index) {
    return std::string(rtlsdr_get_device_name(index));
}


