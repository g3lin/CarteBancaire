
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include "generationAnnuaire.h"

// Codes des couleurs empruntés à la question suivante:
// https://stackoverflow.com/questions/3219393/stdlib-and-colored-output-in-c
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"



/* Pour tester les annuaires */
int main(int argc, char **argv){
    int p, err_exec;
    int nbBanques,nbTerminaux, nbCartes ;
    char nbBanquesS[10],nbTerminauxS[10];
    printf(RED "Combien de banques voulez vous ?\n" );
    printf("(Pour le moment, mettre plus d'une banque ici fait bugger le programme)\n" RESET);
    scanf("%d",&nbBanques);
    printf(GREEN "Combien de terminaux par banques voulez vous ?\n" RESET);
    scanf("%d",&nbTerminaux);
    printf(BLUE "Combien de cartes par banque en moyenne voulez vous ?\n" RESET);
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