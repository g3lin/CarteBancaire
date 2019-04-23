
#include<stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "annuaire.h"



/* Pour tester les annuaires */
int main(int argc, char **argv){
    long int nbBanques, nbCartes;
    AnnuaireClients *an;
    AnnuaireClients *anFiltre;
    Client *cl;

    if(argc != 3){
        fprintf(stderr, "usage : %s nbBanques nbCartes\n", argv[0]);
        exit(0);
    }

    nbBanques = strtol(argv[1], NULL, 10);
    if( (nbBanques == LONG_MIN) || (nbBanques == LONG_MAX) ){
        fprintf(stderr, "%s: nombre de banques invalide\n", argv[0]);
        exit(0);
    }
        
    nbCartes = atoi(argv[2]);
    if( (nbCartes == LONG_MIN) || (nbCartes == LONG_MAX) ){
        fprintf(stderr, "%s: nombre de cartes invalide\n", argv[0]);
        exit(0);
    }

    an = annuaireAleatoire(nbBanques, nbCartes);
    if(an == NULL){
        fprintf(stderr, "%s: Ne peut allouer un annuaire \n", argv[0]);
        exit(0);
    }

    // afficher l'annuaire an
    afficherAnnuaire(an);
    
    // sauvegarder l'annuaire an dans le fichier "Annu"
    if(!sauvegardeAnnuaire(an, "annuaire.an")){
        fprintf(stderr, "%s, Ne peut pas sauvegarder l'annuaire\n", argv[0]);
    }


    for (int i = 0; i<nbBanques;i++){
        char idbanque[4] ;
        sprintf(idbanque,"000%d",i);
        printf("=================================");
        printf("%s", idbanque);
        printf("================================="); 


        anFiltre = annuaireFiltre(an, idbanque);
        if(anFiltre == NULL){
            fprintf(stderr,"%s ne peut filtrer l'annuaire demande\n",argv[0]);
            libererAnnuaire(an);
            exit(0);
        }
        // Lib�rer la m�moire de l'annuaire initial
        char annuaireFile[255] ;
        sprintf(annuaireFile,"annuaire000%d.an",i);
        // Afficher l'annuaire filtr�
        afficherAnnuaire(anFiltre);
        
        // Sauvegarder l'annuaire filtr�

        if(!sauvegardeAnnuaire(anFiltre, annuaireFile)){
            fprintf(stderr,"Ne peut pas sauvegarder l'annuaire\n");
        }

    }
}