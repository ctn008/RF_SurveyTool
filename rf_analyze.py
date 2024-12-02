"""
- calculate suitable max gain to have the best signal without clipping
- write for files (BYTES) for sampling duration
"""

from rtlsdr import RtlSdr

SAMPLE_RATE = 2048000
SAMPLE_LEN  = 4096 * 400
FFT_SIZE = 4096
NUM_SAMPLES = 20
RTL_GAINS = [0.9, 1.4, 2.7, 3.7, 7.7, 8.7, 12.5, 14.4, 15.7, 16.6, 19.7, 20.7,\
             22.9, 25.4, 28.0, 29.7, 32.8, 33.8, 36.4, 37.2, 38.6, 40.2, 42.1,\
             43.4, 43.9, 44.5, 48.0, 49.6]

""" Tetra TMO Tx frequency ranges: 420-425MHz     390-395MHz   395-400MHz """
FREQ_TABLE = [420.8e6, 422.5e6, 424.2e6, 395.8e6, 397.5e6, 399.2e6]

# get current folder and generate filename
import os 
import sys 
import datetime;

cur_directory = os.getcwd() + '/samples/'
rf_file = 'f397.5mhz_g0.0_s1638400_t1733118420.78.uint8'
#rf_file = 'f397.5mhz_g49.6_s1638400_t1733118425.26.uint8'
#rf_file = 'f397.5mhz_g19.7_s1638400_t1733118422.23.uint8'
from pylab import *
from rtlsdr import *
import numpy as np

iq_bytes = np.fromfile(cur_directory+rf_file, dtype='uint8')
iq_samples = np.array([0]*(len(iq_bytes)//2), dtype='complex64')

i_profile = np.array([0]*256, dtype = 'int16')
q_profile = np.array([0]*256, dtype = 'int16')
for i in range(len(iq_bytes)//2):
    i_profile[ iq_bytes[i*2] ]    += 1
    q_profile[ iq_bytes[i*2 +1] ] += 1

import matplotlib.pyplot as plt

plt.figure("Sample values distribution")
plt.plot(i_profile)
plt.plot(q_profile)
plt.show()

#"""
i_offset = 0
q_offset = 0

for i in range(len(iq_samples)):
    iq_samples[i] = (iq_bytes[i*2]-127.5)/127.5 +1j*(iq_bytes[i*2+1]-127.5)/127.5
    i_offset += (iq_bytes[i*2]-127.5)/127.5
    q_offset += (iq_bytes[i*2+1]-127.5)/127.5

print("DC Offset I, Q:", i_offset/len(iq_samples), q_offset/len(iq_samples))
iq_samples = iq_samples - i_offset/len(iq_samples) - 1j*q_offset/len(iq_samples)

# use matplotlib to estimate and plot the PSD
psd(iq_samples, NFFT=FFT_SIZE, Fs=SAMPLE_RATE/1e6, Fc=0)
xlabel('Frequency (MHz)')
ylabel('Relative power (dB)')

show()

#"""
