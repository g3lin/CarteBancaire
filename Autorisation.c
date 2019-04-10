#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "lectureEcriture.h"
#include "message.h"

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

    int soldeSurCompte = 999999;

    int continu = 1;
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

    while(continu){

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
            exit(0);
        }


        // LECTURE DE LA BASE DE CARTES

        // int fd_ListeCartes = open("./ListeCartes.txt", O_RDONLY);
        // if (fd < 0) {
        //     perror("Autorisation - ./ListeCartes.txt invalide");
        //     exit(0);
        // }

        // char *c = LitLigne(fd_ListeCartes);
        // if (c == NULL) {
        //     perror("Autorisation - ./ListeCartes.txt LitLigne non valide");
        //     exit(0);
        // }


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
            printf("Serveur autorisation : reponse envoyée");   
        }
    }
    return 0;
}