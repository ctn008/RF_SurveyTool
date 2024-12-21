# RfCompute module

Functions:
1- read all files in samples folder: samples file has fixed length, but gnss meta data may have variable length.
2- Compute RF power of signal in each given block in saved sample files.
3- Retrieve gnss coordinates and timing info from gnss messages.
4- Save to output file

Question: if filtering the wideband IQ signal into 36k IQ signal per channel, does the rf channel power is still correctly reflected? And can be calculated in time domain instead of frequency domain?

From data point of view:
2.56 msps BB signal --> 100 channel * 36ksps = 3.6msps --> no saving in storage. Only applicable if number of Tetra channels to be saved is less than 50% (ie 50 channels), that is nomally the case in actual situation. 

SAMPLING RATE: 2.56Msps
BLOCK_SIZE: 4 * 4096 = 3.2ms
BUFFER_LENGTH: 640 * 4096 = 160 * BLOCK_SIZE = 512ms
SAMPLE_FILE: 120 * BUFFER_LENGTH = 120 * 160 BLOCK_SIZE = 61,44 seconds

Signal is evaluated per block (16384 bytes). And gain value is saved for every block ?
What is the delay of gain adjustment?

Save time signal or fft power level if cpu processing power is capable of? Save time signal allows checking of received signal whether correct (continuously) or not, by decoding a tetra channel and try to decode - check for frame sequence...
