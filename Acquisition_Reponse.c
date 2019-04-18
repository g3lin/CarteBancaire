#include "Acquisition.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include "lectureEcriture.h"
#include "message.h"


void *thread_LectureReponse(void *arg){
    arg_thread_A *true_arg = arg;
    int fd_fromAuto = true_arg->fd_fromAuto;
    int* Tab_fd_Term = true_arg->Tab_fd_Term;
    char** tab_cb = true_arg->tab_cb;


    int fd_toTerminal;
    char* rep;
    int err;

    char* emetteur = malloc(sizeof(char)*255);
    char* type = malloc(sizeof(char)*255);
    char* valeur = malloc(sizeof(char)*255);
    int decoupeOk;

    int fd_pas_trouve = 1;
    int i = 0;
    char* cb = malloc(sizeof(char)*255);
    //---------------------------------------------------------------------- 
    printf("thread autorisation\n");
    while(1){
        // 1- On attend la reponse du serveur d'autorisation
        rep = litLigne(fd_fromAuto);
        if (rep == 0) {
            perror("TestLectureEcriture - litLigne");
            exit(0);
            }
        printf("Serveur Acquisition : reponse recu\n");

        printf("lecture reponse: %s",rep);


        // 2- Lire le fd pour renvoyer la reponse
        decoupeOk = decoupe(rep, emetteur, type, valeur);
        if (!decoupeOk) {
            printf("Erreur de découpage!!\n");
            //printf("%s", messageAutorisation);
            exit(0);
        }
        
        while(fd_pas_trouve){
            sem_wait(&(semaphoreTableauCB));
                if(tab_cb[i]!=NULL){
                    strcpy(cb,tab_cb[i]) ;
                }
            sem_post(&(semaphoreTableauCB));

            printf("%s\n", cb);
            if(strcmp(emetteur,cb) == 0){
                //fd_pas_trouve = 0;
                break;
            }  
            i++;
        }
        sem_wait(&(semaphoreTableauTerm));
            fd_toTerminal = Tab_fd_Term[i];
        sem_post(&(semaphoreTableauTerm));


        // 3- On renvoie la reponse au terminal 
        err = ecritLigne(fd_toTerminal, rep);
        if (err == 0) {
            perror("Acquisition : fd_pipeAcquisitionTerminal - ecritLigne");
            exit(0);
        }
        printf("Serveur Acquisition : reponse envoyée\n");
    }
}