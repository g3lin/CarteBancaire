#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include "lectureEcriture.h"
#include "message.h"
#include "Acquisition_Demande.h"
#include "Acquisition_Reponse.h"
#include "Acquisition.h"


#define R 0
#define W 1



int main(int argc, char **argv) { 
    int nbTerminaux;

    if(argc < 2){
        fprintf(stderr, "Erreur veuillez préciser le nombre de termminaux.\nUsage: ./Acquistion 3\n");
        exit(0);
    }


    nbTerminaux = atoi(argv[1]);
    if (nbTerminaux < 1){
        fprintf(stderr,"Erreur pas de terminaux\n");
        exit(0);
    }
    //---------------------------------------------------------------------- 
    int fd_pipeAcquisitionAutorisation[2] ;
    int fd_pipeAutorisationAcquisition[2] ;
    int fd_pipeAcquisitionTerminal[2][nbTerminaux];
    int fd_pipeTerminalAcquisition[2][nbTerminaux];
    int fd_pipe_AT[2];
    int fd_pipe_TA[2];
    int i;
    int p;
    char *rep ;
    char arg1[255], arg2[255]; 

    int err;
    int err_exec;
    if (sem_init(&semaphoreCopyArgs,1,1) == -1){
        perror("Erreur d'initialisation du sémaphore CB: ");
        exit(0);
    }



    if (sem_init(&semaphoreTableauTerm,1,1) == -1){
        perror("Erreur d'initialisation du struct Terminaux: ");
        exit(0);
    }

    tab_Terminaux = malloc(sizeof(TabTerminaux));
    terminaux = malloc(nbTerminaux*sizeof(Terminal));

    tab_Terminaux->nbClients = nbTerminaux;
    tab_Terminaux->terminal = terminaux;

    pthread_t thread_auto;
    pthread_t tab_thread[nbTerminaux];

    arg_thread_A *args_auto  = malloc(sizeof(arg_thread_A));
    arg_thread_T * args_term = malloc(sizeof(arg_thread_T));
    //---------------------------------------------------------------------- 

    // PIPES ENTRE ACQUISITION ET AUTORISATION
    if(pipe(fd_pipeAcquisitionAutorisation) != 0) 
    	fprintf(stderr,"Erreur creation fd_pipeAcquisitionAutorisation\n");

    if(pipe(fd_pipeAutorisationAcquisition) != 0)
    	fprintf(stderr,"Erreur creation fd_pipeAutorisationAcquisition\n");

    //PIPES ENTRE ACQUISITION ET TERMINAUX
    for(i = 0 ; i<nbTerminaux ; i++){
        // Creation des pipes AcquisitionTerminal
        if(pipe(fd_pipe_AT)!= 0) 
            fprintf(stderr,"Erreur creation fd_pipeAcquisitionTerminal\n");
        fd_pipeAcquisitionTerminal[W][i] = fd_pipe_AT[W];
        fd_pipeAcquisitionTerminal[R][i] = fd_pipe_AT[R];

        
        // Creation des pipes TerminalAcquisition
        if(pipe(fd_pipe_TA)!= 0) 
            fprintf(stderr,"Erreur creation fd_pipeTerminalAcquisition\n");
        fd_pipeTerminalAcquisition[W][i] = fd_pipe_TA[W];
        fd_pipeTerminalAcquisition[R][i] = fd_pipe_TA[R];
    }
    //---------------------------------------------------------------------- 

    //CONFIG INITIALE
    p=fork();
    if(p == -1)
        printf("error");
        
    if(p==0){
        // PROCESSUS FILS
        // Ceci devient le serveur d'autorisation

        sprintf(arg1,"%d", fd_pipeAcquisitionAutorisation[R]);
        sprintf(arg2,"%d", fd_pipeAutorisationAcquisition[W]);

        err_exec = execl("./Autorisation","./Autorisation",arg1, arg2,  (char *)0);
        perror("Acquisition - initialisation : Le serveur d'autorisation s'est mal initialisé: ");
    }

    
    
    for(i = 0 ; i<nbTerminaux ; i++){
        p = fork();
        if(p == -1){
            printf("error");
            exit(0);
        }

        if(p == 0){
            // PROCESSUS FILS
            // Ceci devient le terminal
            sprintf(arg1,"%d", fd_pipeTerminalAcquisition[W][i]);
            sprintf(arg2,"%d", fd_pipeAcquisitionTerminal[R][i]);

            err_exec = execl( "/usr/bin/xterm", "/usr/bin/xterm", "-hold", "-e", "./Terminal", arg1, arg2, (char *)NULL );
            perror("Acquisition - initialisation : Le terminal s'est mal initialisé: ");    
        }

    }

    //----------------------------------------------------------------------  
    // PROCESSUS PERE 
    // Ceci reste le serveur d'acquisition
    printf("Avant la création des args du thread.\n");

    args_auto->fd_fromAuto = fd_pipeAutorisationAcquisition[R];


    printf("Avant la création du thread.\n");
    if (pthread_create(&thread_auto, NULL, thread_LectureReponse, args_auto)) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }
    
   
    // Creation des threads lisant les terminaux 
    args_term->fd_toAuto = fd_pipeAcquisitionAutorisation[W];

    
    

    for (i = 0 ; i<nbTerminaux ; i++){
        sem_wait(&semaphoreCopyArgs);
            args_term->fd_fromTerminal = fd_pipeTerminalAcquisition[R][i];
            args_term->fd_toTerminal = fd_pipeAcquisitionTerminal[W][i];
            args_term->i = i;
        

        if (pthread_create(&tab_thread[i], NULL, thread_LectureDemande, args_term)) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
        
    }

    
    for (i = 0 ; i<nbTerminaux ; i++){
        if (pthread_join(tab_thread[i], NULL)) {
            perror("pthread_join");
            return EXIT_FAILURE;
        }
    }
    if (pthread_join(thread_auto, NULL)) {
        perror("pthread_join");
        return EXIT_FAILURE;
    }
     
        
    printf("Après la création du thread.\n");
    free(args_auto);
    free(args_term);
    return 0;
}