Program objective

a) RfCapture tool
- Read RtlSdr device to capture RF IQ signal in Tetra band (for downlink) to calculate rf signal level
- Read GPS receiver to capture location coordinate
- Store RF IQ signal into file for each location coordinate

b)RfSignalCalculation tool
- Read IQ signal files, and filter out GPS coordinates
- Perform FFT to calculate signal strength for each Tetra channel (25kHz)
- Store these signal strength datas into a RfSignalStrengh file for each location

c) RfAnalyze
- Tagging channels to sites
- 
d) RfCoverageMapping
- Display signal strength per site, and for all sites of the system in google map.

Technical challenges - RfCollection
- How to measure Rf signals with RtlSdr device which has only 50dB dynamic range and low receiver sensitivity / band selectivity

Need to adjust rtlsdr gain to avoid ADC clipping
+ How to know if clipping is happening ?

+ How to remove DC offset from FFT result ?

==========

OBSERVATIONS
- DC offset is more obvious when there is no signal or signal is weak. Question is whether DC offset is the same? But when signal is large, then you just don't notice DC offset (which become small relatively)
+ DC offset can be removed by taking out DC component in signal samples before performing FFT
+ DC offset seems to be of the same value for different received signal levels

- How to determine signal clipping ?
+ determine largest signal
+ compare with the previous largest signal
+ compare the change in signal value in relation with gain adjustment between the two signals.
+ determine if the signal is reaching clipping threshold
+ search for discussion on signal clipping detection

- Auto gain provide better signal than fixed 49.6 dB gain ?

- psd Power Spectrum Density: what is the graph? Contructed by multiple fft ?

- SYNC & ASYNC are from libusb to PC perspective. For data capture from rtlsdr there is no differences. Data is stored in rtlsdr buffer.
  Therefore, can use read sync to read samples of less than 1 second ?

- Program performance
+ Using rtl_sdr to read samples and write to file can simplify the program code, but performance may be slow.
+ Per testing, reading 0,8second samples requires 1.5 seconds, ie. processing time is 0.7 seconds.
+ May consider / test reading samples in SYNC mode in Python, to see if processing time is shorter?
+ Try testing reading samples in GNURadio to see if performance is better?
