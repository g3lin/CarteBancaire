#include <semaphore.h>

typedef struct tab
{
    /* data */
};


typedef struct arg_threadD
{
    int fd_pipeAcquisitionIB;

} arg_threadD;


typedef struct arg_threadR
{
    /* data */
    
} arg_threadR;

sem_t semaphoreCopyArgs;
sem_t semaphoreTab;
