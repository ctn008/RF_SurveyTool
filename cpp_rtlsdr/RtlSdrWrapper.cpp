#include "RtlSdrCpp.h"
#include <iostream>

RtlSdrCpp::RtlSdrCpp() : device(nullptr) {}

RtlSdrCpp::~RtlSdrCpp() {
    closeDevice();
}

bool RtlSdrCpp::openDevice(int deviceIndex) {
    if (rtlsdr_open(&device, deviceIndex) != 0) {
        std::cerr << "Failed to open device " << deviceIndex << std::endl;
        return false;
    }
    return true;
}

void RtlSdrCpp::closeDevice() {
    if (device) {
        rtlsdr_close(device);
        device = nullptr;
    }
}

bool RtlSdrCpp::setCenterFrequency(uint32_t frequency) {
    if (device == nullptr) return false;
    return rtlsdr_set_center_freq(device, frequency) == 0;
}

uint32_t RtlSdrCpp::getCenterFrequency() const {
    if (device == nullptr) return 0;
    return rtlsdr_get_center_freq(device);
}

bool RtlSdrCpp::setGainMode(bool manual) {
    if (device == nullptr) return false;
    return rtlsdr_set_tuner_gain_mode(device, manual ? 1 : 0) == 0;
}

bool RtlSdrCpp::setGain(int gain) {
    if (device == nullptr) return false;
    return rtlsdr_set_tuner_gain(device, gain) == 0;
}

int RtlSdrCpp::getGain() const {
    if (device == nullptr) return 0;
    return rtlsdr_get_tuner_gain(device);
}

bool RtlSdrCpp::setSampleRate(uint32_t rate) {
    if (device == nullptr) return false;
    return rtlsdr_set_sample_rate(device, rate) == 0;
}

uint32_t RtlSdrCpp::getSampleRate() const {
    if (device == nullptr) return 0;
    return rtlsdr_get_sample_rate(device);
}

int RtlSdrCpp::readSamples(uint8_t* buffer, uint32_t length) {
    if (device == nullptr) return -1;
    int n_read = 0;
    if (rtlsdr_read_sync(device, buffer, length, &n_read) != 0) {
        std::cerr << "Synchronous read failed." << std::endl;
        return -1;
    }
    return n_read;
}

bool RtlSdrCpp::startAsyncRead(void (*callback)(uint8_t* buf, uint32_t len)) {
    if (device == nullptr) return false;

    auto async_callback = [](unsigned char* buf, uint32_t len, void* ctx) {
        auto user_callback = reinterpret_cast<void (*)(uint8_t*, uint32_t)>(ctx);
        user_callback(buf, len);
    };

    if (rtlsdr_read_async(device, async_callback, reinterpret_cast<void*>(callback), 0, 0) != 0) {
        std::cerr << "Failed to start asynchronous read." << std::endl;
        return false;
    }

    return true;
}

void RtlSdrCpp::stopAsyncRead() {
    if (device) {
        rtlsdr_cancel_async(device);
    }
}

int RtlSdrCpp::resetBuffer() {
    return (rtlsdr_reset_buffer(device));
}
