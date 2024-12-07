learning MultiThreading & MultiProcessing

This is not new in programming world, but new for me as what I have learned in programming 35 years before, there is no such concept. Computer is of single CPU and only a single program can run. Ofcourse, later on, there are multi processing OS like windows, but I was personally not involved in and didn't understand the concept from programming standpoint.

Multitasking at the OS level, means that the OS shall manage the CPU times to be shared among different programs or processes. This can be either sharing CPU time in single CPU or sharing proesses among different CPU cores... Whatever being done at OS level, each program instance is called as a separate process. And multiple processes can run concurrently.

For Python, it has a different nature of intepreter language, so each Python program is 1 process. Multithreading in Python has ??? different meaning, in such a way that it is something handled by Python interpreter itself, not by the OS. And to that extend, only single process can run at a time in Python by GIL (Global Interpreter Lock). Therefore, multithreading in Python only works when a thread is not CPU bounded, but idle waiting for IO. 

For multiprocessing program, separate of Python instance are activated in paralell for each process. Because the processes are independent, in order to share data, some mechanism needs to be implemented (global variables are not applicable, as only used within a single process)

