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

0bxxxx	0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	Total
RF	0	9	13	40	38	13	31	22	26	31	26	14	19	5	35	13	335
Mixer	0	5	10	10	19	9	10	25	17	10	8	16	13	6	3	-8	153
VGA	0	26	26	30	42	35	24	13	14	32	36	34	35	37	35	36	455
![image](https://github.com/user-attachments/assets/e263456a-1dcf-48c4-84b4-8abb5c3b0590)
