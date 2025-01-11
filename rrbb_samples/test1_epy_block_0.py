"""
Embedded Python Blocks:

Input:
- a block of IQ samples in bytes default 16384 bytes
Output:
- histogram for I & Q signal in float32"""

import numpy as np
from gnuradio import gr


class my_decim_block(gr.decim_block):
    def __init__(self, decim_rate = 32):
        gr.decim_block.__init__(self,
            name="Byte_Samples Histogram",
            in_sig=[np.int8],
            out_sig=[np.float32],
            decim = decim_rate)
        self.set_relative_rate(1.0/decim_rate)
        self.decimation = decim_rate

    def work(self, input_items, output_items):
        in_len = len(input_items[0])
        out_len = 0
        while in_len >= 256 * self.decimation:
            for i in range (256):
                output_items[0][i] = 0
            for i in range (256 * self.decimation):
                output_items[0][out_len + 128 + input_items[0][i]  ] += 1 
            in_len -= 256 * self.decimation
            out_len += 256
        return out_len
