#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //Header file for sleep(). man 3 sleep for details.
#include <signal.h>
#include <errno.h>

static int do_exit = 0;
int thread_cnt01 = 0;
int thread_cnt02 = 0;

static void sighandler(int signum)
{
    signal(SIGPIPE, SIG_IGN);
    fprintf(stderr, "Signal caught, existing!\n");
    do_exit = 1;
}
// A normal C function that is executed as a thread
// when its name is specified in pthread_create()
void* myThread01(void* vargp)
{
    while (!do_exit)
    {
        printf("Printing GeeksQuiz from Thread 01: %i\n", thread_cnt01);
		thread_cnt01++;
        sleep(1);
    }
    return NULL;
}

void* myThread02(void* vargp)
{
    while (!do_exit)
    {
        printf("Printing GeeksQuiz from Thread 02: %i\n", thread_cnt02);
        thread_cnt02++;
        sleep(1);
    }
    return NULL;
}


int main()
{
    struct sigaction sigact;
    sigact.sa_handler = sighandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigaction(SIGINT, &sigact, NULL);
    sigaction(SIGTERM, &sigact, NULL);
    sigaction(SIGQUIT, &sigact, NULL);
    sigaction(SIGPIPE, &sigact, NULL);

    pthread_t thread_id01, thread_id02;
    printf("Before Thread01\n");
    pthread_create(&thread_id01, NULL, myThread01, NULL);
    printf("After thread01 & before Thread02\n");
    pthread_create(&thread_id02, NULL, myThread02, NULL);
    printf("After Thread02\n");

    while (!do_exit)
    {
        printf("Printing GeeksQuiz from main Thread\n");
        sleep(1);
    }
    exit(0);

//    pthread_join(thread_id01, NULL);
//    pthread_join(thread_id02, NULL);
}
