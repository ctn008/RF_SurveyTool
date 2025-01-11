#ifndef RTL_SDR_WRAPPER_H
#define RTL_SDR_WRAPPER_H

#include <cstdint>
#include <string>

extern "C" {
#include "rtl-sdr.h" // Assuming this header provides function declarations
}

class RtlSdrCpp {
public:
    // Constructor and Destructor
    RtlSdrCpp();
    ~RtlSdrCpp();

    // Device Initialization
    bool openDevice(int deviceIndex);
    void closeDevice();

    // Frequency Control
    bool setCenterFrequency(uint32_t frequency);
    uint32_t getCenterFrequency() const;

    // Gain Control
    bool setGainMode(bool manual);
    bool setGain(int gain);
    int getGain() const;

    // Sampling Rate
    bool setSampleRate(uint32_t rate);
    uint32_t getSampleRate() const;

    // Read Samples
    int readSamples(uint8_t* buffer, uint32_t length); // Synchronous read

    // Asynchronous Read
    bool startAsyncRead(void (*callback)(uint8_t* buf, uint32_t len));
    void stopAsyncRead();
    int resetBuffer();

private:
    rtlsdr_dev_t* device; // Pointer to the RTL-SDR device
};

#endif // RTL_SDR_WRAPPER_H
