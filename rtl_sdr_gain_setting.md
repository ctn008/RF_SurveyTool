# Modifying rtl_sdr gain settings

rtlsdr tuner (t8200x) has 03 gain stages: rf_gain = lna_gain + mix_gain + vga_gain, each represented by a 4 bit registry value (0-15).
librtlsdr has different settings of fixed vga_gain for manual (16.3dB) and auto mode (26.3dB). 
If adjusting to the same vga_gain, tt appears that AUTO AGC for tuner yields a better signal of 6dB better than manual mode (double in signal magnitude as seen on QT GUI time sink).

## 1. Can modify librtlsdr to control rf_gain  
Need to understand the delay of gain setting (ie. how long after the gain setting changes, that you see the output changes).

![image](https://github.com/user-attachments/assets/c68b2abb-7043-45e2-a367-87d46a2e8094)
This plot shows the manual gain setting changes for block 16384 bytes each (gain = 0 vs 490).
![image](https://github.com/user-attachments/assets/80b03f3b-3ea4-422e-b38b-105a0501c453)

This plot shows gain setting changes for block 4096 bytes each.
