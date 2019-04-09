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
 * Programme d'aquisition des transactions
 * Traite les requetes des terminaux qui lui sont configuré
 * Les retransmet au serveur d'aquisition si il s'agit de la meme banque
 * Les transmet au serveur interbancaire si c'est une autre banque.
 *
 * Centre de la banque, crée au démarrage le serveur d'auto et les terminaux propres
 */
int main(int argc, char **argv) { 
    // PIPES ENTRE AQUISITION ET AUTORISATION
    int fd_pipeAquisitionAutorisation[2] ;
    int fd_pipeAutorisationAquisition[2] ;

    if(pipe(fd_pipeAquisitionAutorisation) != 0) 
    	fprintf(stderr,"Erreur creation fd_pipeAquisitionAutorisation\n");

    if(pipe(fd_pipeAutorisationAquisition) !=0)
    	fprintf(stderr,"Erreur creation fd_pipeAutorisationAquisition\n");

    //PIPES ENTRE AQUISITION ET TERMINAL
    int fd_pipeAquisitionTerminal[2] ;
    pipe(fd_pipeAquisitionTerminal);

    int fd_pipeTerminalAquisition[2] ;
    pipe(fd_pipeTerminalAquisition);     
   
    //CONFIG INITIALE
    int p;
    p=fork();

    if(p == -1)
        printf("error");
        
    if(p==0){
        // PROCESSUS FILS
        	//----------------------------------------------------------------------  
            // PROCESSUS FILS-PERE 
            // Ceci devient le serveur d'autorisation
            char arg1[255], arg2[255]; 
            sprintf(arg1,"%d", fd_pipeAquisitionAutorisation[R]);
            sprintf(arg2,"%d", fd_pipeAutorisationAquisition[W]);

            /*printf("Acqui : tube AcquiToAuto : %s\n", arg1);
            printf("Acqui : tube AutoToAcqui : %s\n", arg2);*/

            int err_exec = execl("./Autorisation","./Autorisation",arg1, arg2,  (char *)0);
            perror("Aquisition - initialisation : Le serveur d'autorisation s'est mal initialisé: ");
        }


     p = fork();
             if(p == 0){
        	// while(i=0; i < nbterminaux-1; i++) 
            // et de transformer les pere en terminaux
            char arg1[255], arg2[255]; 
            sprintf(arg1,"%d", fd_pipeTerminalAquisition[W]);
            sprintf(arg2,"%d", fd_pipeAquisitionTerminal[R]);

            /*printf("Acqui : tube TerminalToServeur : %s\n", arg1);
   			printf("Acqui : tube ServeurToTerminal : %s\n", arg2);*/
            
            int err_exec = execl("./Terminal","./Terminal",arg1, arg2,  (char *)0);
            perror("Aquisition - initialisation : Le terminal s'est mal initialisé: ");

        
	}
    

     	//----------------------------------------------------------------------  
        // PROCESSUS PERE 
        // Ceci reste le serveur d'aquistion

        // 1- On lit la demande du terminal
        int aRep = 0;
        char *rep ;

        while(aRep == 0){
            rep = litLigne(fd_pipeTerminalAquisition[R]) ;
            if (rep != NULL) {
                aRep = 1;
            }
        }
        printf("Serveur acquisition : message recu\n");

        // 2- On transmet la demande au serveur d'autorisation

        int err = ecritLigne(fd_pipeAquisitionAutorisation[W], rep);
        if (err == 0) {
            perror("Aquisition : fd_pipeAquisitionAutorisation - ecritLigne");
            exit(0);
        }
        printf("Serveur acquisition : message envoyé\n");


        // 3- On attend la reponse du serveur d'autorisation
        aRep = 0;

        rep = litLigne(fd_pipeAutorisationAquisition[R]);
        perror("TestLectureEcriture - litLigne");

        printf("Serveur acquisition : reponse recu\n");


        // 4- On renvoie la reponse au terminal 
        err = ecritLigne(fd_pipeAquisitionTerminal[W], rep);
        if (err == 0) {
            perror("Aquisition : fd_pipeAquisitionTerminal - ecritLigne");
            exit(0);
        }
        printf("Serveur acquisition : reponse envoyée\n");
            
   



    return 0;
}