import time
def calc_square(numbers):
    for n in numbers:
        print(f'\n{n} ^ 2 = {n*n}')
        time.sleep(0.1)

def calc_cube(numbers):
    for n in numbers:
        print(f'\n{n} ^ 3 = {n*n*n}')
        time.sleep(0.1)


numbers = [1, 2, 3, 4, 5, 6, 7, 8]

import threading

start = time.time()
print(threading.enumerate())
print("threading.active_count Step 0: ", threading.active_count())

square_thread = threading.Thread(target=calc_square, args=(numbers,))
cube_thread = threading.Thread(target=calc_cube, args=(numbers,))

print(threading.enumerate())
print("threading.active_count Step 1: ", threading.active_count())

square_thread.start()
print(threading.enumerate())
print("threading.active_count Step 2: ", threading.active_count())
cube_thread.start()
print(threading.enumerate())
print("threading.active_count Step 3: ", threading.active_count())
square_thread.join()
print(threading.enumerate())
print("threading.active_count Step 4: ", threading.active_count())
cube_thread.join()

print(threading.enumerate())
print("threading.active_count Step 5: ", threading.active_count())

end = time.time()

print(threading.enumerate())
print("threading.active_count Step 6: ", threading.active_count())


print('Execution Time: {}'.format(end-start))
