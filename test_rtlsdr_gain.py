"""
- test RtlSdr Blog V4 manual gain adjustment to confirm whether it works
- by using RtlSdr.py wrapper of standard librtlsdr library
"""
#from __future__ import division, print_function
import numpy as np
from rtlsdr import *
from ctypes import *

sdr = RtlSdr()
print("Gain values: ", sdr.get_gains())
sdr.sample_rate = sample_rate = 2.048e6
sdr.center_freq = center_freq = 98e6
sdr.gain = gain = 49.6 #28.0

num_samples = 128 * 4096 # read for 0.12 seconds 
#readsamples = sdr.read_samples(num_samples)
readbytes = sdr.read_bytes(2*num_samples)

sdr.close()

print("Bytes length: ", len(readbytes))

i_profile = np.array([0]*256, dtype = 'int32')
q_profile = np.array([0]*256, dtype = 'int32')
for i in range(num_samples):
    i_profile[ readbytes[i*2] ]    += 1
    q_profile[ readbytes[i*2 +1] ] += 1

import matplotlib.pyplot as plt

plt.figure("IQ Bytes value distribution")
plt.plot(i_profile)
plt.plot(q_profile, '.')

readsamples = np.ctypeslib.as_array(readbytes)
iq = readsamples.astype(np.float64).view(np.complex128)
iq /= 127.5
iq -= (1 + 1j)

#""" use matplotlib to estimate and plot the PSD
plt.figure("Signal Power Distribution")
plt.psd(iq, NFFT=4096, Fs=sample_rate/1e6, Fc=center_freq/1e6)
plt.xlabel('Frequency (MHz)')
plt.ylabel('Relative power (dB), Gain: ' + str(gain) )

plt.show()
#"""
