#include "thpool.h"
#include "common.h"


void * test_task(int a, int b){
    printf("test_task %d \n", a+b);
    return NULL;
}

int main(int argc, char** argv){
    S_THPOOL pool;
    S_TASK task;
    task.function = test_task;
    int tab[2] = {1, 2};
    task.args = (void*) tab;


    thpool_create(&pool);
    for(int i = 0; i < 10; i++){
        thpool_append_task(&pool, &task);
    }
    thpool_destroy(&pool);
    return 0;
}
