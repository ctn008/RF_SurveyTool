# How to detect Clipping in RfCapture ?

## 1. Problem statement  
Can call functions in librtlsdr library to set rf_gain and mixer_gain. There is some delay, but generally it works. Also, some rtlsdr progams demonstrate manual gain control capability.

Issue is how to detect signal clipping to control/adjust gain ?

## 2. rf_sample_signal for testing  
Need to have sample signal for testing purpose. Ideally the signal should simulate multiple tetra channels.
This rf_sample_signal will also be used to compare / evaluate whether the gain control functions operate correctly or not...

### 2.1 How to generate rf_sample baseband signal ?  
1. Using s36kIQ signal of real tetra signal, or generate s36kIQ from predefined random inputs ?
2. Multiple tetra signals to be mixed onto the baseband signal
3. different signals (channels) should have different signal power level, and distributed across the baseband.
4. using pluto Tx to transmit via rf cable to rtlsdr receiver - hopefully the signal is large enough to receive and cause clipping. So that we can observe how clipping occurs and how to detect clipping...
