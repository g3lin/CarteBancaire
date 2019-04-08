#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "lectureEcriture.h"
#include "message.h"



/**
 * Programme de gestion du terminal
 */
int main(int argc, char **argv)
{
    
    int fd_DemandeOrdre = 1;//(int)argv[1];
    if (fd_DemandeOrdre < 0) {
        printf("%i", fd_DemandeOrdre);
        perror("Terminal.c - FD demande ordre invalide");
        exit(0);
    }

    int fd_DemandeServeur = 0;
    if (fd_DemandeServeur < 0) {
        perror("Terminal.c - FD Demande serveur invalide");
        exit(0);
    }

    int fd_ReponseServeur = 1;
    if (fd_ReponseServeur < 0) {
        perror("Terminal.c - FD Reponse serveur invalide");
        exit(0);
    }

    int fd_ReponseOrdre = 0;
    if (fd_ReponseOrdre < 0) {
        perror("Terminal.c - FD reponse ordre invalide");
        exit(0);
    }


    // ON LIT L'ENTREE (au clavier pour l'instant)

    char *msg = litLigne(fd_DemandeOrdre);
    if (msg == NULL) {
        perror("Terminal.c - Ordre de paiement invalide");
        exit(0);
    }
    

    // ON TRANSMET AU SERVEUR

    msg_traite = message(cb, "Demande", valeur)
    int err = ecritLigne(fd_DemandeServeur, msg);
    if (err == 0) {
        perror("fd_DemandeServeur - ecritLigne");
        exit(0);
    }

    // ON ATTEND LA REPONSE DU SERVEUR
    int aRep = 0;
    char *rep ;

    while(aRep == 0){
        rep = litLigne(fd_ReponseServeur) ;
        if (rep != NULL) {
            aRep = 1;
        }
    }
    
    // ON RETRANSMET LA REPONSE A L'ORDRE

    err = ecritLigne(fd_ReponseOrdre, rep);
    if (err == 0) {
        perror("fd_ReponseOrdre - ecritLigne");
        exit(0);
    }

    






}