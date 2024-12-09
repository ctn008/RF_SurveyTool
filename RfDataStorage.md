# RF Data Storage & Compression  
## Problem statement  
When capturing continuous rf data using 2 rtlsdr, the data storage requirement is double and becomes too large, not practical to store in raw byte format. Need to explore ways to compress the data to reduce storage size.

Also there may be a limitation on number of files can be store in a single folder. Therefore, consider another option of saving multiple samples at different locations in the same file. The file should be indexed. But there should be some meta data (location, rf gain levels) to be associated with the data stream.

## Analysis & Solution  
- Number of files in a folder?
1 second per file = 3600 files per hour and no more than 36000 files per survey trip.
So it is still practical to store seperate sample per file.
- Best filesize for compression?
Check on zlib documentaion: in practice, typically the size of the sliding window can be from several KB to MB, such as 4 KB, 32 KB, 1 MB, or 4 MB.
So if the sample filesize is 150000 samples, then sliding windows 4kb may be practical? Let try to evaluate actual performance with difference window size.
***results***: it turns out that the data can be compressed to reduce only about 20% size. So it may not be worth to perform such process.

So how much diskspce is required, let 's say we have 2 rtrlsdr in paralell?
5Mb per second per rtlsdr ==> 2 rtlsdr = 10Mbytes. 60 * 60 = 36,000 Mbytes per hour, or 36GB
Therefore, it would require 512GB harddisk at least for RfSurvey module.

Another issue is how to store RfSurvey data after processing? There are several options:
+ extract Tetra channel datas (of interest) to save at 36kbps.
  How much saving? 36000 * 8bytes per sample = 288,000 (comparing to 2.56Msps)
+ Calculate dBm and store these values for all channels
  
- How to attach metadata to datastream?

