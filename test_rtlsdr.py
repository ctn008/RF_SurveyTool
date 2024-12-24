""" test rtlsdr
"""

from rtlsdr import RtlSdr



# configure device
"""sdr.sample_rate = 2.048e6  # Hz
sdr.center_freq = 70e6     # Hz
sdr.freq_correction = 60   # PPM
sdr.gain = 'auto'

print(sdr.read_samples(512))
"""
from pylab import *
from rtlsdr import *

# configure device
for i in range (10):
    sdr = RtlSdr(0)
    sdr.sample_rate = 2.048e6 #- i*.1e6
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
