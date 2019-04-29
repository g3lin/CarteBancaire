#include <semaphore.h>

typedef struct Banque
{
    char codeBanque[5];
    int fd_toAquisition;
    int fd_fromAcquisition;
    int fd_toRepAcquisitionBanque;
    sem_t semaphoreTraitement;
    char CBenTraitement[50];
    int fd_toRepAcquisitionEnTraitement;


} Banque;

typedef struct Tab
{
    int nbBanque;
    Banque * Tab_Banque;
} Tab;










sem_t semaphoreCopyArgs;
sem_t semaphoreTab;
Tab *obj_tabTerminaux;
