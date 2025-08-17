# RF Survey Tool 

To perform rf survey for tetra radio systems operating in 380-430MHz band with 25KHz channel spacing using RtlSdr devices.
- Obtain RF signal strengths for all Tetra channels in the selected band
- Display rf coverage (signal strength) map for each site and all sites
- Various analysis tasks for rf optimization

## High level software module design
1. RfCapture
- Capture rf signal of all channels & gps location and store to file for offline processing
- Make sure the rf signal is captured at the optimized value (highest gain with no clipping)
2. RfCompute
3. RfAnalyze
4. RfReport

## Hardware design
Module Connectivity Diagram:
<img width="1033" height="769" alt="image" src="https://github.com/user-attachments/assets/060829f5-2ff5-4718-a22d-67a077ffa2d9" />

Actual hardware: consists of 02 RTL-SDR devices, 01 RF LNA amplifier, rechargeable battery and USB-Type C charging port.

<img width="1316" height="330" alt="image" src="https://github.com/user-attachments/assets/dfde1814-8d91-443e-a694-0ad1213cdce9" />
<img width="2302" height="732" alt="image" src="https://github.com/user-attachments/assets/034b4c10-e007-4f6e-8275-198849dcaea3" />

