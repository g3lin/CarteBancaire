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
        printf("thread Terminal\n");
        arg_thread_T *true_args = arg;
        int fd_toAuto = true_args->fd_toAuto;
        int fd_fromTerminal = true_args->fd_fromTerminal;
        int fd_toTerminal = true_args->fd_toTerminal;
        int i = true_args->i;
    sem_post(&semaphoreCopyArgs); 

    

    char *rep;
    int err;

    char* emetteur = malloc(255 * sizeof(char));
    char* type = malloc(255 * sizeof(char));
    char* valeur = malloc(255 * sizeof(char));
    int decoupeOk;
    tab_Terminaux->terminal[i].CB =malloc(sizeof(char)*255);
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
        sem_wait(&(semaphoreTableauTerm));
            sprintf(tab_Terminaux->terminal[i].CB ,"%s", emetteur);
            tab_Terminaux->terminal[i].FileDescriptor = fd_toTerminal;
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