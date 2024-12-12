""" testing multiprocessing data sharing
- sharing an np array among main process and a child process 
"""
import multiprocessing as mp
import numpy as np
import time
import ctypes

def pr01(timestamp01, do_exit):
    """
    function to print cube of given num
    """
    while not do_exit.value:
        timestamp01.value = time.time()
        #print('pr01 in while loop')
        time.sleep(0.001)
        #print("update GnSS timestamp: ", timestamp)
    print('pr01: do_exit == True, updateGnss returned')

def pr02(timestamp02, do_exit):
    """
    function to print cube of given num
    """
    while not do_exit.value:
        timestamp02.value = time.time()
        #print('pr02 in while loop')
        time.sleep(0.001)
        #print("update GnSS timestamp: ", timestamp)
    print('pr02: do_exit == True, updateGnss returned')
    
    
do_exit = mp.Value('b', False)
timestamp01 = mp.Value('d', 0.0)
p01 = mp.Process(target=pr01, args=(timestamp01, do_exit))

timestamp02 = mp.Value('d', 0.0)
p02 = mp.Process(target=pr02, args=(timestamp02, do_exit))

p01.start()
p02.start()
#p1.join() # child process p will be automatically killed when main process is done

try:
    while not do_exit.value:
        print(timestamp01.value)
        time.sleep(0.001)
       
except KeyboardInterrupt:
        print ('Interrupted')
        do_exit.value = True

print("Done!")
