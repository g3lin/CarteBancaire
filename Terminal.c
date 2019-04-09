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
int main(int argc, char **argv){
    int fd_DemandeServeur;
    int fd_ReponseServeur;

    int decoupeOk;
    char emetteur[255], type[255], valeur[255];

    char *msg;
    int err;

    char *rep ;
    //----------------------------------------------------------------------  
    fd_DemandeServeur = atoi(argv[1]);
    if (fd_DemandeServeur < 0) {
        perror("Terminal.c - FD Demande serveur invalide");
        exit(0);
    }

    fd_ReponseServeur = atoi(argv[2]);
    if (fd_ReponseServeur < 0) {
        perror("Terminal.c - FD Reponse serveur invalide");
        exit(0);
    }
    //----------------------------------------------------------------------  

    // ON LIT L'ENTREE
    aleainit();
    sprintf(valeur,"%d",alea(1,50000));

    // ON TRANSMET AU SERVEUR
    msg = message("0001000000000000", "Demande", valeur);
    if (msg == NULL) {
        perror("Terminal.c - Ordre de paiement invalide");
        exit(0);
    }
    err = ecritLigne(fd_DemandeServeur, msg);
    if (err == 0) {
        perror("fd_DemandeServeur - ecritLigne");
        exit(0);
    }
    printf("Terminal : message envoyé\n");

    // ON ATTEND LA REPONSE DU SERVEUR
    rep = litLigne(fd_ReponseServeur) ;
    
    // ON RETRANSMET LA REPONSE A L'ORDRE
    decoupeOk = decoupe(rep, emetteur, type, valeur);
    if (!decoupeOk) {
        printf("Erreur de découpage!!\n");
        perror("TestMessage (decoupe)");
        exit(0);
    }

    if (atoi(valeur) == 1){
        // 0 = NON AUTORISEE -- 1 = AUTORISEE
        printf("%s", "AUTORISATION ACCORDEE\n\n");
    }
    else
        printf("%s", "AUTORISATION NON ACCORDEE\n\n");
  

return 0;
}