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
- How to attach metadata to datastream?

