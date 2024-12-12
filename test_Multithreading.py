import threading
import time

timestamp = 0

def updateGnss(daemon = True):
    global timestamp
    while True:
        timestamp = time.time()
        time.sleep(0.5)
        print("This is a child thread, timestamp =", timestamp)

gnss = threading.Thread(target = updateGnss())
gnss.start()

for i in range(10):
    print("Parent thread, timestamp =", timestamp)
    time.sleep(0.5)
