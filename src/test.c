#include "thpool.h"
#include "common.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


void  test_task(void * args){

    int * tab = (int *) args;
    int a = tab[0];
    int b = tab[1];
    //sleep(1);
    printf("test_task id : %lu %d \n", pthread_self(), a+b);
    return;
}

int main(int argc, char** argv){

    time_t t; 
    time(&t); 
    srand(t); 

    S_THPOOL pool;
    S_TASK task;
    task.function = test_task;
    int tab[2] = {1, 2};
    task.args = (void*) tab;

    thpool_init(&pool);
    for(int i = 0; i < 10; i++){
        tab[0] = rand();
        tab[1] = rand();
       // sleep(1);
       thpool_append_task(&pool, &task);
    }

    //sleep(3);
    thpool_destroy(&pool,1<<1);
    return 0;
}
