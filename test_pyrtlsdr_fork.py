
from rtlsdr import *


sdr = RtlSdr()
print(sdr.gain_values)
sdr.sample_rate = 2.048e6
sdr.center_freq = 421e6
sdr.gain = 'auto'
num_samples = 512 * 4096 # read for 0.8 seconds 
num_bytes = 2 * num_samples
print(sdr.get_gain())
sdr.set_gain(0.9)
print(sdr.get_gain())
print(sdr.gain)
readsamples = sdr.read_samples(num_samples//4)
readsamples = sdr.read_samples(num_samples)
#readbytes = sdr.read_bytes(num_bytes)

sdr.close()

"""# use matplotlib to estimate and plot the PSD
psd(readsamples, NFFT=4096, Fs=sdr.sample_rate/1e6, Fc=sdr.center_freq/1e6)
xlabel('Frequency (MHz)')
ylabel('Relative power (dB)')

show()

"""
