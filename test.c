#include "thpool.h"
#include "common.h"


void  test_task(void * args){

    int * tab = (int *) args;
    int a = tab[0];
    int b = tab[1];
    printf("test_task %d \n", a+b);
    return;
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
    thpool_destroy(&pool,2);
    return 0;
}
