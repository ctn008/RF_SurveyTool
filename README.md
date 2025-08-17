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
