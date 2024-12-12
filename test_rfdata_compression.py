"""
- test rfdata compression using zlib library
- using real RF data for compression, test different windows lengths
"""
#from __future__ import division, print_function
import numpy as np
from rtlsdr import *
from ctypes import *
import io
import matplotlib.pyplot as plt


num_samples = 128 * 4096 # read for 0.12 seconds 

dev_list = RtlSdr.get_device_serial_addresses()
print("RtlSdr devices: ", dev_list)
set_gain = 49.6 #'auto'

# inititate sdr0
sdr0 = RtlSdr(0)
print("RtlSdr", dev_list[0], " Gain values: ", sdr0.get_gains())
sdr0.sample_rate = sample_rate = 2.560e6
sdr0.center_freq = center_freq = 98e6
sdr0.gain = gain = set_gain

# inititate sdr1
sdr1 = RtlSdr(1)
print("RtlSdr", dev_list[1], " Gain values: ", sdr1.get_gains())
sdr1.sample_rate = sample_rate = 2.048e6
sdr1.center_freq = center_freq = 98e6
sdr1.gain = gain = set_gain

readbytes0 = sdr0.read_bytes(2*num_samples)
readbytes1 = sdr1.read_bytes(2*num_samples)

sdr0.close()
sdr1.close()


readbytes = readbytes0

i_profile = np.array([0]*256, dtype = 'int32')
q_profile = np.array([0]*256, dtype = 'int32')
for i in range(num_samples):
    i_profile[ readbytes[i*2] ]    += 1
    q_profile[ readbytes[i*2 +1] ] += 1

plt.figure(dev_list[0]+": IQ Bytes value distribution")
plt.plot(i_profile)
plt.plot(q_profile, '.')

readsamples = np.ctypeslib.as_array(readbytes)
iq = readsamples.astype(np.float64).view(np.complex128)
iq /= 127.5
iq -= (1 + 1j)

#""" use matplotlib to estimate and plot the PSD
plt.figure(dev_list[0]+": Signal Power Distribution")
plt.psd(iq, NFFT=4096, Fs=sample_rate/1e6, Fc=center_freq/1e6)
plt.xlabel('Frequency (MHz)')
plt.ylabel('Relative power (dB), Gain: ' + str(gain) )

readbytes = readbytes1

i_profile = np.array([0]*256, dtype = 'int32')
q_profile = np.array([0]*256, dtype = 'int32')
for i in range(num_samples):
    i_profile[ readbytes[i*2] ]    += 1
    q_profile[ readbytes[i*2 +1] ] += 1

plt.figure(dev_list[1]+": IQ Bytes value distribution")
plt.plot(i_profile)
plt.plot(q_profile, '.')

readsamples = np.ctypeslib.as_array(readbytes)
iq = readsamples.astype(np.float64).view(np.complex128)
iq /= 127.5
iq -= (1 + 1j)

#""" use matplotlib to estimate and plot the PSD
plt.figure(dev_list[1]+": Signal Power Distribution")
plt.psd(iq, NFFT=4096, Fs=sample_rate/1e6, Fc=center_freq/1e6)
plt.xlabel('Frequency (MHz)')
plt.ylabel('Relative power (dB), Gain: ' + str(gain) )

plt.show()
#"""
#np.array(readbytes).tofile('/home/ctn008/RfSurvey/readbytes.uint8')
