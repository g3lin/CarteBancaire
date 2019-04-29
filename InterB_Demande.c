#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include "InterBancaire.h"
#include "lectureEcriture.h"
#include "message.h"
#include "annuaire.h"

//---------------------------------------------------------------------- 
void *thread_LectureDemande(void *arg){
        // printf("thread Terminal\n");
        Banque *true_args = arg;
        int fd_pipeAcquisitionIB = true_args->fd_fromAcquisition;
        int fd_toRepAcquisitionBanque =true_args->fd_toRepAcquisitionBanque;

        // int fd_fromTerminal = true_args->fd_fromTerminal;
        // int fd_toTerminal = true_args->fd_toTerminal;
        // int i = true_args->i;
    sem_post(&semaphoreCopyArgs); 

    

    char *rep;
    int err;

    char* emetteur = malloc(255 * sizeof(char));
    char* type = malloc(255 * sizeof(char));
    char* valeur = malloc(255 * sizeof(char));
    int decoupeOk;
    char *banqueClient = malloc(5*sizeof(char)) ;
    //tab_Terminaux->terminal[i].CB =malloc(sizeof(char)*255);
    printf("THREAD IB DEMANDE\n");
    //---------------------------------------------------------------------- 
    while(1){
        // 1- On lit la demande du terminal
        rep = litLigne(fd_pipeAcquisitionIB) ;
        if (rep == 0) {
            perror("InterBancaire :: fd_pipeTerminalInterBancaire :- ecritLigne");
            exit(0);
        }
        printf("Serveur InterBancaire :: message recu depuis %d \n", fd_pipeAcquisitionIB);

        printf("lecture demande: %s (depuis %d )\n",rep ,fd_pipeAcquisitionIB);

        // 2- Ecire le fd de sortie dans la memoire partagée
        decoupeOk = decoupe(rep, emetteur, type, valeur);
        if (!decoupeOk) {
            printf("Erreur de découpage!!\n");
            //printf("%s", messageAutorisation);
            exit(0);
        }





        if(strcmp(type , "Demande")){
            //LA DEMANDE VIENT D'ETRE ENVOYEE D'UN TERMINAL
            // ELLE DOIT ETRE DISPATCHÉE VERS LA BONNE BANQUE
            for(int i = 0;i < obj_tabTerminaux->nbBanque ; i++){

                
                AnnuaireClients* an = annuaire("annuaire.an");
                banqueClient =  (client(an,emetteur)->banque);

                if(strcmp(banqueClient,obj_tabTerminaux->Tab_Banque[i].codeBanque)==0){
                    // On a la bonne banque
                    sem_wait(&obj_tabTerminaux->Tab_Banque[i].semaphoreTraitement);
                    strcpy(obj_tabTerminaux->Tab_Banque[i].CBenTraitement,emetteur);
                    obj_tabTerminaux->Tab_Banque[i].fd_toRepAcquisitionEnTraitement = fd_toRepAcquisitionBanque;

                    err = ecritLigne(obj_tabTerminaux->Tab_Banque[i].fd_toAquisition, rep);
                    
                    if (err == 0) {
                        perror("InterBancaire :: fd_pipeAcquisitionAutorisation - ecritLigne");
                        exit(0);
                    }
                    printf("Serveur InterBancaire :: message envoyé au bon serveur d'acquistion\n");

                    break;
                }
            }
        }









        else if (strcmp(type , "Reponse"))
        {
            //LA DEMANDE REVIENT DU SERVEUR D'AUTORISATION 
            // ELLE DOIT REVENIR À LA BANQUE OU EST LE TERMINAL
            for (int i = 0; i < obj_tabTerminaux->nbBanque; i++){
                if(strcmp(obj_tabTerminaux->Tab_Banque[i].CBenTraitement, emetteur) == 0){

                    err = ecritLigne(obj_tabTerminaux->Tab_Banque[i].fd_toRepAcquisitionEnTraitement, rep);
                    
                    if (err == 0) {
                        perror("InterBancaire :: fd_toRepAcquisition - ecritLigne");
                        exit(0);
                    }
                    printf("Serveur InterBancaire :: reponse envoyé au bon serveur de reponse\n");

                    sem_post(&obj_tabTerminaux->Tab_Banque[i].semaphoreTraitement);

                }
            }
            





        }
        
        else{
            fprintf(stderr,"DEMANDE INVALIDE\n");
        }

       
        // sem_wait(&(semaphoreTab));
        //     sprintf(tab_Terminaux->terminal[i].CB ,"%s", emetteur);
        //     tab_Terminaux->terminal[i].FileDescriptor = fd_toTerminal;
        // sem_post(&(semaphoreTab));


    //     // 3- On transmet la demande au serveur d'autorisation
    //     err = ecritLigne(fd_toAuto, rep);
    //     if (err == 0) {
    //         perror("InterBancaire :: fd_pipeAcquisitionAutorisation - ecritLigne");
    //         exit(0);
    //     }
    //     printf("Serveur InterBancaire :: message envoyé\n");
    }
}
//---------------------------------------------------------------------- 