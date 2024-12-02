# RF Capture

## 1. Engineering aspects:
a. Manually control rf gain of rtlsdr and detect adc clipping
- Check whether RtlSdr allows / has mechanism to detect adc clipping
- Check difference between auto gain versus manual gain (auto gain seems to provide better signal than max manual gain @ 49.6dB)
- Any difference between read_samples_sync and read_samples_async
- Any data distortion / unstability at intitial data samples. 
