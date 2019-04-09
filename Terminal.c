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
    //----------------------------------------------------------------------  
    int fd_DemandeServeur = atoi(argv[1]);
    if (fd_DemandeServeur < 0) {
        perror("Terminal.c - FD Demande serveur invalide");
        exit(0);
    }

    int fd_ReponseServeur = atoi(argv[2]);
    if (fd_ReponseServeur < 0) {
        perror("Terminal.c - FD Reponse serveur invalide");
        exit(0);
    }
    /*printf("Term : tube TerminalToServeur : %d\n", fd_DemandeServeur);
    printf("Term : tube ServeurToTerminal : %d\n", fd_ReponseServeur);*/
    //----------------------------------------------------------------------  

    int decoupeOk;
    char emetteur[255], type[255], valeur[255];
    
    // ON LIT L'ENTREE (au clavier pour l'instant)
    aleainit();
    sprintf(valeur,"%d",alea(1,50000));

    // ON TRANSMET AU SERVEUR
    char *msg = message("0001000000000000", "Demande", valeur);
   // sprintf(msg, "%s\n", msg);
    if (msg == NULL) {
        perror("Terminal.c - Ordre de paiement invalide");
        exit(0);
    }
    int err = ecritLigne(fd_DemandeServeur, msg);
    if (err == 0) {
        perror("fd_DemandeServeur - ecritLigne");
        exit(0);
    }
    printf("Terminal : message envoyé\n");

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

    if (*valeur == 1){
        // 0 = NON AUTORISEE -- 1 = AUTORISEE
         printf("%s", "AUTORISATION ACCORDEE\n");
    }
    else
        printf("%s", "AUTORISATION NON ACCORDEE\n");
   
    
    
return 0;
}