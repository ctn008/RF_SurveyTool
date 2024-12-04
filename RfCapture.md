# RF Capture

## 1. Engineering aspects:
a. Manually control rf gain of rtlsdr and detect adc clipping
- Check whether RtlSdr allows / has mechanism to detect adc clipping
- Check difference between auto gain versus manual gain (auto gain seems to provide better signal than max manual gain @ 49.6dB)
- Any difference between read_samples_sync and read_samples_async
- Any data distortion / unstability at intitial data samples. 

### 1.1 Understand Rtl-Sdr gains
There are 3 gain stages mentioned in Rtl-Sdr: lna gain, mixer gain & VGA gain. However, in new development https://github.com/Sultan-papagani/sdrpp_rtlsdr_source, there is RTL Gain, Mixer Gain and Lna gain on top of VGA gain. Anh when increasing VGA gain, RTL gain is reduced ?

It seems that VGA gain increases the whole spectrum (and background noise level). And to avoice clipping, when VGA gain increase, then Rtl-Sdr gain is reduced accordingly.
![image](https://github.com/user-attachments/assets/76a4ac53-7b3f-4688-bf34-7334230a7b23)
Tuner gain has 03 stages:
LNA gain, Mixer gain and VGA gain. whereas AGC mode is not recommended for narrowband signals.
LNA Gain and Mixer can be set in register with value 0-15 (04 bits) whereas VGA gain is from -12dB to 40.5dB with 3.5dB step.

RTL gain and Tuner gain ? RTL AGC is auto gain control for RTL2832U chip. Tunner AGC = gain on tuner chip. It is always better to have manual gain adjustment.

https://www.rtl-sdr.com/new-updates-to-the-librtlsdr-rtl-sdr-driver-fork/

![image](https://github.com/user-attachments/assets/e263456a-1dcf-48c4-84b4-8abb5c3b0590)
Per http://superkuh.com/rtlsdr.html, LNA gain is 33dB total, and Mixer gain 15dB, whereas, VGA gain is 3.5dB step, with -12 to 40.5dB. The table above is actual measurement at 900MHz, and -60dBm input signal. Similar measurement can be performed at 400MHz for better alignment. 

Rtl-Sdr tuner gain is established based on the above measurements, in such a way that lna and mixer gain registers are increased in sequence (i.e. the gain index increases gradually)

![image](https://github.com/user-attachments/assets/278d60d0-4072-46e4-b4cc-66b38b9d0844)


**Using librtlsdr fork and pyrtlsdr library to test this fork librtlsdr**  
Tested the forked librtlsdr library ok on NeSdr device. But with blog v4, somehow setting manual gain doesn't function as expected, keep 0.0 unchanged ???

Testing on RtlSdr wrapper with setting sdr.gain to 'auto' and different gains shows that the gain values did impact the signal outputs. Therefore, it is confirmed that the different gain settings work as expected. There is something incorrect with the gain setting code...

from pylab import *  
from rtlsdr import *  

sdr = RtlSdr()  
sdr.sample_rate = 2.048e6  
sdr.center_freq = 98e6  

sdr.gain = 'auto' #28.0  
num_samples = 256 * 4096 # read for 0.12 seconds  
readsamples = sdr.read_samples(num_samples)  

sdr.close()  

psd(readsamples, NFFT=4096, Fs=sdr.sample_rate/1e6, Fc=sdr.center_freq/1e6)  
xlabel('Frequency (MHz)')  
ylabel('Relative power (dB), Gain: auto')  

show()  

As seen on graphs below, auto gain has similar to 49.6 gain (max lna and mixer gain values).
![image](https://github.com/user-attachments/assets/da5d6247-9c3f-4feb-803a-647403424918)

![image](https://github.com/user-attachments/assets/7d97b085-a27d-4b57-ae62-78b4bced48f7)

![image](https://github.com/user-attachments/assets/bd4f90a8-6273-4459-896a-b8d6ead50dcb)

![image](https://github.com/user-attachments/assets/732f8d99-24bf-4071-898b-507ab3472c1b)

There are 02 newer branchs of librtlsdr with advanced features, which are:  
- old_dab: perform more calculation of gains at different frequencies, and provide absolute gain, signal strength  
- Hayati: provide extension mode for setting separate lna, mixer & vga gains for R820T tunner (not sure applicable for R828D & V4 ?)

**CONCLUSION ON GAIN SETTINGS**: RtlSdr can be setted for manual gain with different gain values and this works. There are issues with large signals (or strong interferences) that causes  overloading, as well as the sensitivity / selectivity of RtlSdr devices, that impact the performance, as well as different librtlsdr forks which provide better accurracies. Let 's leave this work for next phase improvements.  
