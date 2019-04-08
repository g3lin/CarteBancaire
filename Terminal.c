#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "lectureEcriture.h"
#include "message.h"
#include "alea.h"

/**
 * Programme de gestion du terminal
 */
int main(int argc, char **argv)
{
    
    int fd_DemandeOrdre = atoi(argv[1]);
    if (fd_DemandeOrdre < 0) {
        printf("%i", fd_DemandeOrdre);
        perror("Terminal.c - FD demande ordre invalide");
        exit(0);
    }

    int fd_DemandeServeur = atoi(argv[2]);
    if (fd_DemandeServeur < 0) {
        perror("Terminal.c - FD Demande serveur invalide");
        exit(0);
    }

    int fd_ReponseServeur = atoi(argv[3]);
    if (fd_ReponseServeur < 0) {
        perror("Terminal.c - FD Reponse serveur invalide");
        exit(0);
    }

    int fd_ReponseOrdre = atoi(argv[4]);
    if (fd_ReponseOrdre < 0) {
        perror("Terminal.c - FD reponse ordre invalide");
        exit(0);
    }

    int decoupeOk;
    char emetteur[255], type[255], valeur[255];
    char *pch;
    char *mess[2];
    
    // ON LIT L'ENTREE (au clavier pour l'instant)

    char *msg = litLigne(fd_DemandeOrdre);
    if (msg == NULL) {
        perror("Terminal.c - Ordre de paiement invalide");
        exit(0);
    }  

    // ON TRANSMET AU SERVEUR
    pch = strtok(msg," ,.-");
    int i = 0;
    while (pch != NULL){
        mess[i] = pch;
        i++;
        pch = strtok(NULL, " ,.-");
    }


    char* msg_traite = message(mess[0], "Demande", mess[1]);
    sprintf(msg_traite,"%s%s",msg_traite,"\n");

    int err = ecritLigne(fd_DemandeServeur, msg_traite);
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
    decoupeOk = decoupe(rep, emetteur, type, valeur);
    if (!decoupeOk) {
        printf("Erreur de découpage!!\n");
        perror("TestMessage (decoupe)");
        exit(0);
    }
    sprintf(valeur,"%s%s",valeur,"\n");
    err = ecritLigne(fd_ReponseOrdre, valeur);
    if (err == 0) {
        perror("fd_ReponseOrdre - ecritLigne");
        exit(0);
    }

    






}