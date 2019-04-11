#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "lectureEcriture.h"
#include "message.h"

#define R 0
#define W 1

/**
 * Programme d'acquisition des transactions
 * Traite les requetes des terminaux qui lui sont configuré
 * Les retransmet au serveur d'acquisition si il s'agit de la meme banque
 * Les transmet au serveur interbancaire si c'est une autre banque.
 *
 * Centre de la banque, crée au démarrage le serveur d'auto et les terminaux propres
 */
int main(int argc, char **argv) { 

    if(argc < 2){
        fprintf(stderr, "Erreur veuillez préciser le nombre de termminaux.\nUsage: ./Acquistion 3\n");
        exit(0);
    }

    int nbTerminal;
    nbTerminal = atoi(argv[1]);
    if (nbTerminal < 1){
        fprintf(stderr,"Erreur pas de terminaux\n");
        exit(0);
    }
    //---------------------------------------------------------------------- 

    int fd_pipeAcquisitionAutorisation[2] ;
    int fd_pipeAutorisationAcquisition[2] ;
    int fd_pipeAcquisitionTerminal[2][nbTerminal];
    int fd_pipeTerminalAcquisition[2][nbTerminal];
    int fd_pipe_AT[2];
    int fd_pipe_TA[2];
    int i;

    int p;
    char *rep ;
    char arg1[255], arg2[255]; 

    int err;
    int err_exec;

    int nbStop = 0;
    //---------------------------------------------------------------------- 

    // PIPES ENTRE ACQUISITION ET AUTORISATION
    if(pipe(fd_pipeAcquisitionAutorisation) != 0) 
    	fprintf(stderr,"Erreur creation fd_pipeAcquisitionAutorisation\n");

    if(pipe(fd_pipeAutorisationAcquisition) != 0)
    	fprintf(stderr,"Erreur creation fd_pipeAutorisationAcquisition\n");

    //PIPES ENTRE ACQUISITION ET TERMINAUX
    for(i = 0 ; i<nbTerminal ; i++){
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

    
    
    for(i = 0 ; i<nbTerminal ; i++){
        p = fork();
        if(p == -1)
            printf("error");

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
    while(1){
        for(i = 0 ; i<nbTerminal ; i++){
            // 1- On lit la demande du terminal
            rep = litLigne(fd_pipeTerminalAcquisition[R][i]) ;
            if (rep == 0) {
                perror("Acquisition : fd_pipeTerminalAcquisition - ecritLigne");
                exit(0);
            }
            if (strcmp(rep, "STOP\n") == 0)
                nbStop ++;
            if (nbStop == nbTerminal){
                printf("ARRET DU SERVEUR D'ACQUISITION\n");
                err = ecritLigne(fd_pipeAcquisitionAutorisation[W], rep);
                printf("tututut\n");
                if (err == 0) {
                    perror("Acquisition : fd_pipeAcquisitionAutorisation - ecritLigne");
                    exit(0);
                }
                printf("tutut\n");
                exit(0);
            }
            printf("Serveur Acquisition : message recu\n");


            // 2- On transmet la demande au serveur d'autorisation
            err = ecritLigne(fd_pipeAcquisitionAutorisation[W], rep);
            if (err == 0) {
                perror("Acquisition : fd_pipeAcquisitionAutorisation - ecritLigne");
                exit(0);
            }
            printf("Serveur Acquisition : message envoyé\n");


            // 3- On attend la reponse du serveur d'autorisation
            rep = litLigne(fd_pipeAutorisationAcquisition[R]);
            if (rep == 0) {
                perror("TestLectureEcriture - litLigne");
                exit(0);
             }
            printf("Serveur Acquisition : reponse recu\n");


            // 4- On renvoie la reponse au terminal 
            err = ecritLigne(fd_pipeAcquisitionTerminal[W][i], rep);
            if (err == 0) {
                perror("Acquisition : fd_pipeAcquisitionTerminal - ecritLigne");
                exit(0);
            }
            printf("Serveur Acquisition : reponse envoyée\n");
        }
    }

        

    return 0;
}