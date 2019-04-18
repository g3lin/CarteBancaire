#include "Acquisition.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "lectureEcriture.h"
#include "message.h"

//---------------------------------------------------------------------- 
void *thread_LectureDemande(void *arg){
    sem_wait(&(semaphoreCopyArgs));
        printf("thread Terminal\n");
        arg_thread_T *true_args = arg;
        int fd_toAuto = true_args->fd_toAuto;
        int fd_fromTerminal = true_args->fd_fromTerminal;
        int fd_toTerminal = true_args->fd_toTerminal;
        int* Tab_fd_Term = true_args->Tab_fd_Term;
        char** tab_cb = true_args->tab_cb;
    sem_post(&semaphoreCopyArgs);

    int i = true_args->i;

    char *rep;
    int err;

    char emetteur[255], type[255], valeur[255];
    int decoupeOk;
    printf("FD FROM TERMINAL : %d\n",fd_fromTerminal);
    //---------------------------------------------------------------------- 
    while(1){
        // 1- On lit la demande du terminal
        rep = litLigne(fd_fromTerminal) ;
        if (rep == 0) {
            perror("Acquisition : fd_pipeTerminalAcquisition - ecritLigne");
            exit(0);
        }
        printf("Serveur Acquisition : message recu depuis %d \n", fd_fromTerminal);

        printf("lecture demande: %s (depuis %d )\n",rep ,fd_fromTerminal);

        // 2- Ecire le fd de sortie dans la memoire partagée
        decoupeOk = decoupe(rep, emetteur, type, valeur);
        if (!decoupeOk) {
            printf("Erreur de découpage!!\n");
            //printf("%s", messageAutorisation);
            exit(0);
        }
        sem_wait(&(semaphoreTableauCB));
            tab_cb[i] = emetteur;
        sem_post(&(semaphoreTableauCB));

        sem_wait(&(semaphoreTableauTerm));
            Tab_fd_Term[i] = fd_toTerminal;
        sem_post(&(semaphoreTableauTerm));


        // 3- On transmet la demande au serveur d'autorisation
        err = ecritLigne(fd_toAuto, rep);
        if (err == 0) {
            perror("Acquisition : fd_pipeAcquisitionAutorisation - ecritLigne");
            exit(0);
        }
        printf("Serveur Acquisition : message envoyé\n");
    }
}
//---------------------------------------------------------------------- 