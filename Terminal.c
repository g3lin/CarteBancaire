#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "lectureEcriture.h"
#include "message.h"
#include "alea.h"
#include "annuaire.h"

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
    int clientRandom;

    int i,j;

    AnnuaireClients *an;
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
    printf("tube d'entree à aquisition %d\n\n",fd_DemandeServeur);
    printf("tube de sortie d'aquistion : %d\n\n",fd_ReponseServeur);

    aleainit();
    while(1){
        //printf("%d\n", i);
        // ON LIT L'ENTREE (ici, on crée une valeur aléatoire pour chaque cb)
        
        sprintf(valeur,"%d",alea(1,50000));

        an = annuaire("annuaire.an");
        if(an == NULL){
            fprintf(stderr,"%s ne peut lire l'annuaire depuis le fichier%s\n", argv[0], "Annu");
            exit(0);
        }

        
        clientRandom = alea(1,an->nbClients);
        for(j = 0; j < clientRandom ; j++){
            strcpy(emetteur , an->donnees[j].CB);
        }


        // ON TRANSMET AU SERVEUR
        msg = message(emetteur, "Demande", valeur);
        printf("%s", msg);
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
        printf("%s\n",rep);
        if (atoi(valeur) == 1){
            // 0 = NON AUTORISEE -- 1 = AUTORISEE
            printf("%s", "AUTORISATION ACCORDEE\n\n");
        }
        else
            printf("%s", "AUTORISATION NON ACCORDEE\n\n");
    }
    printf("ARRET DU TERMINAL\n");
    err = ecritLigne(fd_DemandeServeur, "STOP\n");
    if (err == 0) {
        perror("fd_DemandeServeur - ecritLigne");
        exit(0);
    }

    return 0;
}