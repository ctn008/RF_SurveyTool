"""
- calculate suitable max gain to have the best signal without clipping
- write for files (BYTES) for sampling duration
"""

from rtlsdr import * #RtlSdr

SAMPLE_RATE = 2048000
SAMPLE_LEN  = 4096 * 400
NUM_SAMPLES = 20
RTL_GAINS = [0.9, 1.4, 2.7, 3.7, 7.7, 8.7, 12.5, 14.4, 15.7, 16.6, 19.7, 20.7,\
             22.9, 25.4, 28.0, 29.7, 32.8, 33.8, 36.4, 37.2, 38.6, 40.2, 42.1,\
             43.4, 43.9, 44.5, 48.0, 49.6]

NUM_SAMPLES = 1
RTL_GAINS = [0., 19.7, 29.7, 49.6]

""" Tetra TMO Tx frequency ranges: 420-425MHz     390-395MHz   395-400MHz """
FREQ_TABLE = [420.8e6, 422.5e6, 424.2e6, 395.8e6, 397.5e6, 399.2e6]

# get current folder and generate filename
import os 
import sys 
import datetime;

cur_directory = os.getcwd()

import subprocess
for index in range (NUM_SAMPLES):
    # configure device
    for i in range (len(FREQ_TABLE)):
        for j in range(len(RTL_GAINS)):
            rtl_freq = FREQ_TABLE[i]
            rtl_gain = RTL_GAINS[j]
            rtl_samples = SAMPLE_LEN
            time_stamp = str(datetime.datetime.now().timestamp())
            time_stamp = time_stamp[:13]
            filename = cur_directory + '/samples/f'+str(rtl_freq/1e6)+'mhz_g'\
                       +str(rtl_gain)+'_s'+str(SAMPLE_LEN) +'_t'+time_stamp+'.uint8'
            
            subprocess.run(["rtl_sdr", '-f'+str(rtl_freq), '-g'+str(rtl_gain),\
                            '-n'+str(rtl_samples), filename])

print("Complete !")
"""
from pylab import *
from rtlsdr import *


for index in range (NUM_SAMPLES):
    # configure device
    for i in range (len(FREQ_TABLE)):
        sdr = RtlSdr()
        sdr.sample_rate = 2.048e6 - i*.1e6
        sdr.center_freq = 421e6
        sdr.gain = 49.6
        # 2.56Msps = 512 * 5000 = 1 second 
        #num_samples = 256 * 1024 * 4 = 512 * 2048
        num_samples = 512 * 4096 # read for 0.8 seconds 
        num_bytes = 2 * num_samples
        readsamples = sdr.read_samples(num_samples//4)
        readsamples = sdr.read_samples(num_samples)
    #    readbytes = sdr.read_bytes(num_bytes)

        sdr.close()

        # use matplotlib to estimate and plot the PSD
        psd(readsamples, NFFT=4096, Fs=sdr.sample_rate/1e6, Fc=sdr.center_freq/1e6)
        xlabel('Frequency (MHz)')
        ylabel('Relative power (dB)')

        show()

"""
