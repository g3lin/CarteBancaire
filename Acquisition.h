#include <semaphore.h>
sem_t semaphoreTableauCB;
sem_t semaphoreTableauTerm;

//---------------------------------------------------------------------- 
typedef struct {
    //infos pour terminal
    int fd_fromTerminal;
    int fd_toTerminal;
    //info pour autorisation
    int fd_toAuto;
    //adresse tableau
    int* Tab_fd_Term;
    char** tab_cb;
    int i; //ligne où écrire
} arg_thread_T;

typedef struct {
    //info pour autorisation
    int fd_fromAuto;
    //adresse tableau
    int* Tab_fd_Term;
    char** tab_cb;
} arg_thread_A;
//---------------------------------------------------------------------- 
