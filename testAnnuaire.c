//auteur : Jean Cousty et Laurent Najman 

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

  /**********************************************************/
  /*                                                        */
  /*      Etape 1 : g�n�raton d'un annuaire al�atoire       */
  /*                                                        */
  /**********************************************************/

  // Generer un annuaire aleatoire avec nbBanques et nbCartes
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
  
  // lib�rer la m�moire de l'annuaire an
  libererAnnuaire(an);

  /**********************************************************/
  /*                                                        */
  /*   Etape 2 : Charger un annuaire depuis un fichier      */
  /*                                                        */
  /**********************************************************/

  // Charger un annuaire sauvegard� dans le fichier "Annu"
  an = annuaire("annuaire.an");
  if(an == NULL){
    fprintf(stderr,"%s ne peut lire l'annuaire depuis le fichier%s\n", argv[0], "Annu");
    exit(0);
  }
  
  /**********************************************************/
  /*                                                        */
  /*           Etape 3 : Filtrer un annuaire                */
  /*                                                        */
  /**********************************************************/

  // G�n�rer un annuaire ne contenant que les cartes de la banque "0000"
  anFiltre = annuaireFiltre(an, "0000");
  if(anFiltre == NULL){
    fprintf(stderr,"%s ne peut filtrer l'annuaire demande\n",argv[0]);
    libererAnnuaire(an);
    exit(0);
  }
  // Lib�rer la m�moire de l'annuaire initial
  libererAnnuaire(an);
  
  // Afficher l'annuaire filtr�
  afficherAnnuaire(anFiltre);
  
  // Sauvegarder l'annuaire filtr�
  if(!sauvegardeAnnuaire(anFiltre, "annuaire0001.an")){
    fprintf(stderr,"Ne peut pas sauvegarder l'annuaire\n");
  }
  
  // Lib�re l'annuaire filtr�
  libererAnnuaire(anFiltre);
  
  /**********************************************************/
  /*                                                        */
  /*  Etape 4 : Rechercher les informations d'une carte     */
  /*                                                        */
  /**********************************************************/

  // Charger l'annuaire stock� dans le fichier annuaireExemple
  an = annuaire("annuaireExemple.an");
  if(an == NULL){
    fprintf(stderr,"%s:Ne peut charg� annuaireExemple.an\n",argv[0]);
    exit(0);
  }

  // Rechercher le client dont la carte est 0004564593939009
  cl = client(an,"0004564593939009");
  
  // si ce client est dans l'annuaire
  if (cl != NULL){
    // On affiche ses informations
    printf("Le numero de carte du client recherche est %s\n", cl->CB);
    printf("Sa banque est %s\n", cl->banque);
    printf("Le solde de son compte est %d\n", cl->solde);
  }else {
    printf("Le client 0004564593939009 n'est pas r�pertorie dans l'annuaire\n");
  }
  
  // Lib�re l'annuaire d'exemple
  libererAnnuaire(an);

  return 0;
}
