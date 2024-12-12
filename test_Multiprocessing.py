""" testing multiprocessing data sharing
- sharing an np array among main process and a child process 
"""
import multiprocessing as mp
import numpy as np
import time
import ctypes

def updateGnss(timestamp, do_exit):
    """
    function to print cube of given num
    """
    while not do_exit.value:
        timestamp.value = time.time()
        time.sleep(0.10)
        #print("update GnSS timestamp: ", timestamp)
    print('do_exit == True, updateGnss returned')
    
do_exit = mp.Value('b', False)
timestamp = mp.Value('d', 0.0)
p1 = mp.Process(target=updateGnss, args=(timestamp, do_exit))

p1.start()
#p1.join() # child process p will be automatically killed when main process is done

arr_len = 16
def tonumpyarray(mp_arr):
    return np.frombuffer(mp_arr.get_obj(), dtype='uint8')

shared_arr = mp.Array(ctypes.c_ubyte, arr_len)
arr = tonumpyarray(shared_arr)    

for i in range(len(arr)):
    arr[i] = i+1
print('Original shared array:', arr)

def rfCapture(arr, do_exit):
    while not do_exit.value:
        print("rfCapture array length: ", len(arr))
        for i in range (len(arr)):
            print(arr[i], end=',')
            arr[i] += 1
        print('\n')
        time.sleep(0.50)
        
p2 = mp.Process(target=rfCapture, args=(shared_arr, do_exit))
p2.start()

try:
    while not do_exit.value:
        print(timestamp.value)
        time.sleep(0.5)
       
except KeyboardInterrupt:
        print ('Interrupted')
        do_exit.value = True

print('Updated by p2 shared array:', arr)        
print("Done!")
