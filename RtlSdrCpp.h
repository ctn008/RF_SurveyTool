#ifndef RTLSDR_DEVICE_H
#define RTLSDR_DEVICE_H

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <array>
#include <rtl-sdr.h>

class RtlSdr {
public:
    RtlSdr();
    ~RtlSdr();

    void open(int index = 0);
    void close();
    void setCenterFrequency(uint32_t freq);
    uint32_t getCenterFrequency() const;
    void setSampleRate(uint32_t rate);
    uint32_t getSampleRate() const;
    void setGainMode(bool isManual);
    void setGain(int gainValue);
    int getGain() const;
    enum rtlsdr_tuner getTunerType();
    void resetBuffer();
    void readSamples(int numSamples, std::vector<uint8_t> &buffer) const;
    void readSamplesAgc(int numSamples, std::vector<uint8_t>& buffer, 
                        std::vector<int>& blockAgcMag, std::vector<int>& blockAgcIdx);
    int agcMagControl(const int agcBlockSize, uint8_t* block);
    static int getDeviceCount();
    static std::string getDeviceName(uint32_t index);

private:
    rtlsdr_dev_t* device = nullptr;
    uint32_t sampleRate;
    uint32_t centerFreq;
    bool gainMode;  // 0 for auto gain, 1 for manual gain
    int gain;
    int devIndex;
    int manualAgcIdx;
    int manualAgcEnabled;

    static constexpr std::array<int, 29> r82xx_gains = {0,9,14,27,37,77,87,125,144,157,166,197,207,229,254,280,297,328,338,364,372,386,402,421,434,439,445,480,496}; 
    static constexpr int MAX_GAIN = 496;

};

#endif // RTLSDR_DEVICE_H
