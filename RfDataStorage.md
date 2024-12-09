# RF Data Storage & Compression  
## Problem statement  
When capturing continuous rf data using 2 rtlsdr, the data storage requirement is double and becomes too large, not practical to store in raw byte format. Need to explore ways to compress the data to reduce storage size.

Also there may be a limitation on number of files can be store in a single folder. Therefore, consider another option of saving multiple samples at different locations in the same file. The file should be indexed. But there should be some meta data (location, rf gain levels) to be associated with the data stream.

## Analysis & Solution  
- Number of files in a folder?
- Best filesize for compression?
- How to attach metadata to datastream?
