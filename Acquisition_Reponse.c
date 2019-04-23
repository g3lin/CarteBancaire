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



    int fd_toTerminal;
    char* rep;
    int err;

    char* emetteur = malloc(sizeof(char)*255);
    char* type = malloc(sizeof(char)*255);
    char* valeur = malloc(sizeof(char)*255);
    int decoupeOk;

    int trouve;

    int i = 0;

    //---------------------------------------------------------------------- 
    printf("thread autorisation\n");
    while(1){

        trouve = 0;

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

        sem_wait(&(semaphoreTableauTerm));
            for(i=0;i<tab_Terminaux->nbClients;i++){ 
                if (strcmp(tab_Terminaux->terminal[i].CB,emetteur) == 0){
                    fd_toTerminal = tab_Terminaux->terminal[i].FileDescriptor;
                    trouve = 1;
                }
                
            }
        sem_post(&(semaphoreTableauTerm));

        if( trouve == 0){
            fprintf(stderr, "CB non trouvée");
        }

        // 3- On renvoie la reponse au terminal 
        err = ecritLigne(fd_toTerminal, rep);
        if (err == 0) {
            perror("Acquisition : fd_pipeAcquisitionTerminal - ecritLigne");
            exit(0);
        }
        printf("Serveur Acquisition : reponse envoyée\n");
    }
}