#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "lectureEcriture.h"
#include "message.h"
#include "annuaire.h"

/**
 * Programme d'autorisation des transactions
 */
int main(int argc, char **argv){
    int fd_DemandeAuto;
    int fd_ReponseAuto;

    char* messageAutorisation;
    char* messageReponseAutorisation;
    char emetteur[255], type[255], valeur[255];
    int decoupeOk;
    int err;

    AnnuaireClients *an;
    Client *clientCB;

    int soldeSurCompte;

    //----------------------------------------------------------------------   
    fd_DemandeAuto = atoi(argv[1]);
    if (fd_DemandeAuto < 0) {
        perror("Autorisation.c - FD demande autorisation invalide");
        exit(0);
    }

    fd_ReponseAuto = atoi(argv[2]);
    if (fd_ReponseAuto < 0) {
        perror("Autorisation.c - FD reponse autorisation invalide");
        exit(0);
    }
    //----------------------------------------------------------------------  

    while(1){

        // LECTURE DU MESSAGE DE DEMANDE D'AUTORISATION
        messageAutorisation = litLigne(fd_DemandeAuto);
        if (messageAutorisation == NULL) {
            perror("Autorisation - Demande d'autorisation LitLigne non valide");
            exit(0);
        }
        if (strcmp(messageAutorisation, "STOP\n") == 0){
            printf("ARRET DU SERVEUR D'AUTORISATION\n");
            exit(0);
        }
        printf("Serveur autorisation : message recu\n");

        decoupeOk = decoupe(messageAutorisation, emetteur, type, valeur);
        if (!decoupeOk) {
            printf("Erreur de découpage!!\n");
            //printf("%s", messageAutorisation);
            exit(0);
        }


        // LECTURE DE LA BASE DE CARTES
        an = annuaire("annuaire.an");
        if (an == NULL){
            fprintf(stderr,"Annuaire non trouvé");
            exit(0);
        }

        clientCB = client(an,emetteur);
        if (clientCB == NULL){
            fprintf(stderr,"Client non trouvé");
            exit(0);
        }
        soldeSurCompte = clientCB->solde;



        // LE PAIEMENT EST AUTORISE
        if(soldeSurCompte - atoi(valeur) >= 0 ){
            
            messageReponseAutorisation = message(emetteur,"Reponse","1");

            err = ecritLigne(fd_ReponseAuto, messageReponseAutorisation);
            if (err == 0) {
                perror("Autorisation- ecritLigne Réponse Autorisation (valide)");
                exit(0);
            }
            printf("Serveur autorisation : reponse envoyée\n");
            
        }

        // PAIEMENT REFUSE CAR PAS LE SOLDE REQUIS
        else{
            messageReponseAutorisation = message(emetteur,"Reponse","0");

            err = ecritLigne(fd_ReponseAuto, messageReponseAutorisation);
            if (err == 0) {
                perror("Autorisation- ecritLigne Réponse Autorisation (invalide)");
                exit(0);
            }
            printf("Serveur autorisation : reponse envoyée\n");   
        }
    }
    return 0;
}