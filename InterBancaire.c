#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include "lectureEcriture.h"
#include "message.h"
#include "InterB_Demande.h"
#include "InterBancaire.h"


#define R 0
#define W 1



int main(int argc, char **argv) { 
    int nbBanque;
    int nbTerminaux;



    if(argc < 3){
        fprintf(stderr, "Erreur veuillez préciser le nombre de banques et de terminaux.\nUsage: ./InterBancaire 3 3\n");
        exit(0);
    }


    nbBanque = atoi(argv[1]);
    if (nbBanque < 1){
        fprintf(stderr,"Erreur pas de banques\n");
        exit(0);
    }

    nbTerminaux = atoi(argv[2]);
    if (nbTerminaux < 1){
        fprintf(stderr,"Erreur pas de terminaux\n");
        exit(0);
    }
    //---------------------------------------------------------------------- 

    obj_tabTerminaux = malloc(sizeof(Tab));
    Banque *tab_Banque = malloc(nbBanque*sizeof(Banque));
    obj_tabTerminaux->nbBanque = nbBanque;
    obj_tabTerminaux->Tab_Banque = tab_Banque;
    for (int i=0; i<nbBanque;i++){
        sem_init(&obj_tabTerminaux->Tab_Banque[i].semaphoreTraitement,1,1);
    }

    sem_init(&semaphoreCopyArgs,1,1);

    int fd_pipeIBAcquisition[nbBanque][2];
    int fd_pipeAcquisitionIB[nbBanque][2];
    int fd_pipeIBAcquisitionRep[nbBanque][2];

    int fd_pipe_AIB[2];
    int fd_pipe_IBA[2];
    int fd_pipe_IBARep[2];


    int i;
    int p;
    char *rep ;
    char arg1[255], arg2[255], arg3[255], arg4[255],arg5[255]; 


    int err;
    int err_exec;
    if (sem_init(&semaphoreCopyArgs,1,1) == -1){
        perror("Erreur d'initialisation du sémaphore de copie: ");
        exit(0);
    }



    if (sem_init(&semaphoreTab,1,1) == -1){
        perror("Erreur d'initialisation du struct Terminaux: ");
        exit(0);
    }

    



    pthread_t tab_threadD[nbBanque];
    pthread_t tab_threadR[nbBanque];


    
    //---------------------------------------------------------------------- 

    // PIPES ENTRE IB ET ACQ_DEMANDE

    for(i = 0 ; i<nbBanque ; i++){
        // PIPES ENTRE IB ET ACQ_DEMANDE

        if(pipe(fd_pipe_IBA)!= 0) 
            fprintf(stderr,"Erreur creation fd_pipeAcquisitionTerminal\n");
        fd_pipeAcquisitionIB[i][W] = fd_pipe_IBA[W];
        fd_pipeAcquisitionIB[i][R] = fd_pipe_IBA[R];

        
        // Creation des pipes ACQ_DEMANDE ET IB
        if(pipe(fd_pipe_AIB)!= 0) 
            fprintf(stderr,"Erreur creation fd_pipeTerminalAcquisition\n");
        fd_pipeIBAcquisition[i][W] = fd_pipe_AIB[W];
        fd_pipeIBAcquisition[i][R] = fd_pipe_AIB[R];

        // Creation des pipes IB ET ACQ_REPONSE
        if(pipe(fd_pipe_IBARep)!= 0) 
            fprintf(stderr,"Erreur creation fd_pipeTerminalAcquisition\n");
        fd_pipeIBAcquisitionRep[i][W] = fd_pipe_IBARep[W];
        fd_pipeIBAcquisitionRep[i][R] = fd_pipe_IBARep[R];

    }

   
    //---------------------------------------------------------------------- 

    //CONFIG INITIALE
   
    
    
    for(i = 0 ; i<nbBanque ; i++){
        p = fork();
        if(p == -1){
            printf("error");
            exit(0);
        }

        if(p == 0){
            // PROCESSUS FILS
            // Ceci devient le serveur d'acq
            // sprintf(arg1,"%d", fd_pipeTerminalAcquisition[W][i]);
            // sprintf(arg2,"%d", fd_pipeAcquisitionTerminal[R][i]);
            sprintf(arg1,"%d", nbTerminaux);
            sprintf(arg2,"%d", fd_pipeAcquisitionIB[i][W]);
            sprintf(arg3,"%d", fd_pipeIBAcquisition[i][R]);
            sprintf(arg4,"%d", fd_pipeIBAcquisitionRep[i][R]),
            sprintf(arg5,"000%d",i);
            

            
            err_exec = execl( "/usr/bin/xterm", "/usr/bin/xterm",  "-e", "./Acquisition", arg1, arg2, arg3, arg4,arg5, (char *)NULL );
            //err_exec = execl("./Acquisition", arg1, arg2, arg3, (char *)NULL );

            perror("Acquisition - initialisation : L'acquition s'est mal initialisé: ");    
        }

        // SETUP COTÉ SERVEUR

        sprintf(obj_tabTerminaux->Tab_Banque[i].codeBanque, "000%d",i);
        obj_tabTerminaux->Tab_Banque[i].fd_toAquisition = fd_pipeIBAcquisition[i][W];
        obj_tabTerminaux->Tab_Banque[i].fd_fromAcquisition = fd_pipeAcquisitionIB[i][R];
        sleep(5);
    }

    //----------------------------------------------------------------------  
    // PROCESSUS PERE 
    // Ceci reste le serveur IB
    printf("Avant la création des args du thread.\n");


    for (i = 0 ; i<nbBanque ; i++){
        //THREADS LECTURE ACQ DEMANDE i
        sem_wait(&semaphoreCopyArgs);
            
            // args_term->fd_toTerminal = fd_pipeAcquisitionTerminal[W][i];
            // args_term->i = i;
        

            if (pthread_create(&tab_threadD[i], NULL, thread_LectureDemande, &(obj_tabTerminaux->Tab_Banque[i]) )) {
                perror("pthread_create");
                return EXIT_FAILURE;
            }       
        
    }


    
    for (i = 0 ; i<nbTerminaux ; i++){
        if (pthread_join(tab_threadD[i], NULL)) {
            perror("pthread_join");
            return EXIT_FAILURE;
        }
    }

    for (i = 0 ; i<nbTerminaux ; i++){
        if (pthread_join(tab_threadR[i], NULL)) {
            perror("pthread_join");
            return EXIT_FAILURE;
        }
    }
     
        
    printf("Après la création du thread.\n");
    return 0;
}