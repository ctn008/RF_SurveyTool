import multiprocessing as mp
import numpy as np
from rtlsdr import *
import ctypes
import time

do_exit = False
BYTES_READ_SIZE = 4096*8

mp_sig0_0 = mp.Array(ctypes.c_ubyte, BYTES_READ_SIZE)
mp_sig0_1 = mp.Array(ctypes.c_ubyte, BYTES_READ_SIZE)
mp_sig0_idx = mp.Value('i', 0)
mp_sig0_flag = mp.Value('b', False)

sig0 = np.array([0]*BYTES_READ_SIZE, dtype='uint8')

mp_sig1_0 = mp.Array(ctypes.c_ubyte, BYTES_READ_SIZE)
mp_sig1_1 = mp.Array(ctypes.c_ubyte, BYTES_READ_SIZE)
mp_sig1_idx = mp.Value('i', 0)
mp_sig1_flag = mp.Value('b', False)

sig1 = np.array([0]*BYTES_READ_SIZE, dtype='uint8')

#@limit_calls(50)
def read_callback0(data, context):
    if(mp_sig0_idx.value == 0):
        arr = np.frombuffer(mp_sig0_0.get_obj(), dtype='uint8')
        arr[:] = data[:]
        mp_sig0_idx.value = 1
        mp_sig0_flag.value = True
    else:
        arr = np.frombuffer(mp_sig0_1.get_obj(), dtype='uint8')
        arr[:] = data[:]
        mp_sig0_idx.value = 0
        mp_sig0_flag.value = True

def read_callback1(data, context):
    if(mp_sig1_idx.value == 0):
        arr = np.frombuffer(mp_sig1_0.get_obj(), dtype='uint8')
        arr[:] = data[:]
        mp_sig1_idx.value == 1
        mp_sig1_flag.value = True
    else:
        arr = np.frombuffer(mp_sig1_1.get_obj(), dtype='uint8')
        arr[:] = data[:]
        mp_sig1_idx.value == 0
        mp_sig1_flag.value = True

assigned_gain = 49.6        
sdr0 = RtlSdr(0)
sdr0.sample_rate = sample_rate = 2.56e6
sdr0.center_freq = center_freq = 98e6
sdr0.gain = assigned_gain

sdr1 = RtlSdr(1)
sdr1.sample_rate = sample_rate = 2.56e6
sdr1.center_freq = center_freq = 98e6
sdr1.gain = assigned_gain

def read_sdr0(mp_sig0_0, mp_sig0_1, mp_sig0_idx, mp_sig0_flag):
    sdr0.read_bytes_async(read_callback0, BYTES_READ_SIZE)

def read_sdr1(mp_sig1_0, mp_sig1_1, mp_sig1_idx, mp_sig1_flag):
    sdr1.read_bytes_async(read_callback1, BYTES_READ_SIZE)

p0 = mp.Process(target=read_sdr0, args=(mp_sig0_0, mp_sig0_1, mp_sig0_idx, mp_sig0_flag))
p1 = mp.Process(target=read_sdr1, args=(mp_sig1_0, mp_sig1_1, mp_sig1_idx, mp_sig1_flag))

p1.start()
p0.start()

run_idx = 0
try:
    while True:
        if do_exit:
            break
        if mp_sig0_flag.value:
            if mp_sig0_idx == 0:
                sig0[:] = np.frombuffer(mp_sig0_1.get_obj(), dtype='uint8')[:]
            else:
                sig0[:] = np.frombuffer(mp_sig0_0.get_obj(), dtype='uint8')[:]
            mp_sig0_flag.value = False
            print('Copy sdr0 completed: ', run_idx)
            run_idx += 1

        if mp_sig1_flag.value:
            if mp_sig1_idx == 0:
                sig1[:] = np.frombuffer(mp_sig1_1.get_obj(), dtype='uint8')[:]
            else:
                sig1[:] = np.frombuffer(mp_sig1_0.get_obj(), dtype='uint8')[:]
            mp_sig1_flag.value = False
            print('Copy sdr1 completed: ', run_idx)
            run_idx += 1
        time.sleep(0.01)
        
except KeyboardInterrupt:
        print ('Interrupted')
        do_exit = True
        
sdr0.close()
sdr1.close()
print("Completed")
