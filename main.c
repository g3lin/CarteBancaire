
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include "generationAnnuaire.h"



/* Pour tester les annuaires */
int main(int argc, char **argv){
    int p, err_exec;
    int nbBanques,nbTerminaux, nbCartes ;
    char nbBanquesS[10],nbTerminauxS[10];
    printf("Combien de banques voulez vous ?\n");
    printf("(Pour le moment, mettre plus d'une banque ici fait bugger le programme)\n");
    scanf("%d",&nbBanques);
    printf("Combien de terminaux par banques voulez vous ?\n");
    scanf("%d",&nbTerminaux);
    printf("Combien de cartes par banque en moyenne voulez vous ?\n");
    scanf("%d",&nbCartes);

    printf("\n\nGENERATION DES CLIENTS DES BANQUES\n\n");
    generate_ann(nbBanques,nbCartes);
    printf("\n\n");

    sprintf(nbBanquesS,"%d",nbBanques);
    sprintf(nbTerminauxS,"%d",nbTerminaux);

    p = fork();
    if(p == -1){
        printf("error");
        exit(0);
    }

    if(p == 0){
        // PROCESSUS FILS
        err_exec = execl( "/usr/bin/xterm", "/usr/bin/xterm",  "-e", "./InterBancaire",nbBanquesS, nbTerminauxS,  (char *)NULL );
        perror("main - initialisation : L'exec s'est mal initialisé: ");    
    }


    
}