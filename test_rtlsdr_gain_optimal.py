"""
- test RtlSdr Blog V4 manual gain adjustment to confirm whether it works
- by using RtlSdr.py wrapper of standard librtlsdr library
"""
#from __future__ import division, print_function
import numpy as np
from rtlsdr import *
from ctypes import *

sdr = RtlSdr()
gain_table = sdr.get_gains()
print("Gain values: ", gain_table)
sdr.sample_rate = sample_rate = 2.048e6
sdr.center_freq = center_freq = 98e6
sdr.gain = gain_table[0]

num_samples = 128 * 4096 # read for 0.12 seconds 

for i in range(len(gain_table)):
    readbytes = sdr.read_bytes(num_samples*2)

    i_profile = np.array([0]*256, dtype = 'int32')
    q_profile = np.array([0]*256, dtype = 'int32')
    for i in range(num_samples):
        i_profile[ readbytes[i*2] ]    += 1
        q_profile[ readbytes[i*2 +1] ] += 1
    excess_i = 0
    excess_q = 0
    for i in range(10):
        excess_i = i_profile[i] + i_profile[255-i]
        excess_q = q_profile[i] + q_profile[255-i]
    if (excess_i > 5) or (excess_q > 5):
        break
    sdr.gain = gain_table[i]
    
print("Optimal rf_gain: ", gain_table[i])
    
sdr.close()

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
plt.ylabel('Relative power (dB), Gain: ' + str(sdr.gain) )

plt.show()
#"""
