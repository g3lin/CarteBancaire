#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "InterBancaire.h"
#include "lectureEcriture.h"
#include "message.h"

//---------------------------------------------------------------------- 
void *thread_LectureDemande(void *arg){
        // printf("thread Terminal\n");
        arg_threadD *true_args = arg;
        int fd_pipeAcquisitionIB = true_args->fd_pipeAcquisitionIB;
        // int fd_fromTerminal = true_args->fd_fromTerminal;
        // int fd_toTerminal = true_args->fd_toTerminal;
        // int i = true_args->i;
    sem_post(&semaphoreCopyArgs); 

    

    char *rep;
    //int err;

    char* emetteur = malloc(255 * sizeof(char));
    char* type = malloc(255 * sizeof(char));
    char* valeur = malloc(255 * sizeof(char));
    int decoupeOk;
    //tab_Terminaux->terminal[i].CB =malloc(sizeof(char)*255);
    printf("THREAD IB DEMANDE\n");
    //---------------------------------------------------------------------- 
    while(1){
        // 1- On lit la demande du terminal
        rep = litLigne(fd_pipeAcquisitionIB) ;
        if (rep == 0) {
            perror("InterBancaire :: fd_pipeTerminalInterBancaire :- ecritLigne");
            exit(0);
        }
        printf("Serveur InterBancaire :: message recu depuis %d \n", fd_pipeAcquisitionIB);

        printf("lecture demande: %s (depuis %d )\n",rep ,fd_pipeAcquisitionIB);

        // 2- Ecire le fd de sortie dans la memoire partagée
        decoupeOk = decoupe(rep, emetteur, type, valeur);
        if (!decoupeOk) {
            printf("Erreur de découpage!!\n");
            //printf("%s", messageAutorisation);
            exit(0);
        }
        // sem_wait(&(semaphoreTab));
        //     sprintf(tab_Terminaux->terminal[i].CB ,"%s", emetteur);
        //     tab_Terminaux->terminal[i].FileDescriptor = fd_toTerminal;
        // sem_post(&(semaphoreTab));


    //     // 3- On transmet la demande au serveur d'autorisation
    //     err = ecritLigne(fd_toAuto, rep);
    //     if (err == 0) {
    //         perror("InterBancaire :: fd_pipeAcquisitionAutorisation - ecritLigne");
    //         exit(0);
    //     }
    //     printf("Serveur InterBancaire :: message envoyé\n");
    }
}
//---------------------------------------------------------------------- 