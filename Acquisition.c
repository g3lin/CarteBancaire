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

    int fd_pipeacquisitionAutorisation[2] ;
    int fd_pipeAutorisationacquisition[2] ;
    int fd_pipeacquisitionTerminal[2] ;
    int fd_pipeTerminalacquisition[2] ;
    int p;
    char *rep ;

    // PIPES ENTRE acquisition ET AUTORISATION
    if(pipe(fd_pipeacquisitionAutorisation) != 0) 
    	fprintf(stderr,"Erreur creation fd_pipeacquisitionAutorisation\n");

    if(pipe(fd_pipeAutorisationacquisition) !=0)
    	fprintf(stderr,"Erreur creation fd_pipeAutorisationacquisition\n");

    //PIPES ENTRE acquisition ET TERMINAL

    if(pipe(fd_pipeacquisitionTerminal)!= 0) 
    	fprintf(stderr,"Erreur creation fd_pipeacquisitionTerminal\n");
    
    if(pipe(fd_pipeTerminalacquisition)!= 0) 
    	fprintf(stderr,"Erreur creation fd_pipeTerminalacquisition\n");   
   


    //CONFIG INITIALE
    
    p=fork();

    if(p == -1)
        printf("error");
        
    if(p==0){
            // PROCESSUS FILS
            // Ceci devient le serveur d'autorisation

            char arg1[255], arg2[255]; 
            sprintf(arg1,"%d", fd_pipeacquisitionAutorisation[R]);
            sprintf(arg2,"%d", fd_pipeAutorisationacquisition[W]);

            int err_exec = execl("./Autorisation","./Autorisation",arg1, arg2,  (char *)0);
            perror("acquisition - initialisation : Le serveur d'autorisation s'est mal initialisé: ");
        }


    p = fork();
    if(p == 0){
            // PROCESSUS FILS
            // Ceci devient le terminal

            char arg1[255], arg2[255]; 
            sprintf(arg1,"%d", fd_pipeTerminalacquisition[W]);
            sprintf(arg2,"%d", fd_pipeacquisitionTerminal[R]);
            
            int err_exec = execl("./Terminal","./Terminal",arg1, arg2,  (char *)0);
            perror("acquisition - initialisation : Le terminal s'est mal initialisé: ");

        
	}
    






    //----------------------------------------------------------------------  
    // PROCESSUS PERE 
    // Ceci reste le serveur d'aquistion

    // 1- On lit la demande du terminal
    
    rep = litLigne(fd_pipeTerminalacquisition[R]) ;
    if (rep == 0) {
        perror("acquisition : fd_pipeTerminalacquisition - ecritLigne");
        exit(0);
    }
     
    printf("Serveur acquisition : message recu\n");





    // 2- On transmet la demande au serveur d'autorisation

    int err = ecritLigne(fd_pipeacquisitionAutorisation[W], rep);
    if (err == 0) {
        perror("acquisition : fd_pipeacquisitionAutorisation - ecritLigne");
        exit(0);
    }
    printf("Serveur acquisition : message envoyé\n");







    // 3- On attend la reponse du serveur d'autorisation

    rep = litLigne(fd_pipeAutorisationacquisition[R]);
    if (rep == 0) {
        perror("TestLectureEcriture - litLigne");
        exit(0);
     }
    printf("Serveur acquisition : reponse recu\n");






    // 4- On renvoie la reponse au terminal 
    err = ecritLigne(fd_pipeacquisitionTerminal[W], rep);
    if (err == 0) {
        perror("acquisition : fd_pipeacquisitionTerminal - ecritLigne");
        exit(0);
    }
    printf("Serveur acquisition : reponse envoyée\n");
        




    return 0;
}