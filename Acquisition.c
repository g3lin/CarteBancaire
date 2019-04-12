#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

#include "lectureEcriture.h"
#include "message.h"

#define R 0
#define W 1

//---------------------------------------------------------------------- 
typedef struct {
    //infos pour terminal
    int fd_fromTerminal;
    int fd_toTerminal;
    //info pour autorisation
    int fd_toAuto;
    //adresse tableau
    int* Tab_fd_Term;
    char* tab_cb;
    int i; //ligne où écrire
} arg_thread_T;

typedef struct {
    //info pour autorisation
    int fd_fromAuto;
    //adresse tableau
    int* Tab_fd_Term;
    char* tab_cb;
} arg_thread_A;
//---------------------------------------------------------------------- 

/**
 * Programme d'acquisition des transactions
 * Traite les requetes des terminaux qui lui sont configuré
 * Les retransmet au serveur d'acquisition si il s'agit de la meme banque
 * Les transmet au serveur interbancaire si c'est une autre banque.
 *
 * Centre de la banque, crée au démarrage le serveur d'auto et les terminaux propres
 */

//---------------------------------------------------------------------- 
void *thread_LectureDemande(void *arg){
    printf("thread Terminal\n");
//     arg_thread_T *true_args = arg;
//     int fd_toAuto = true_args->fd_toAuto;
//     int fd_fromTerminal = true_args->fd_fromTerminal;
//     int fd_toTerminal = true_args->fd_toTerminal;
//     int* Tab_fd_Term = true_args->Tab_fd_Term;
//     char* tab_cb = true_args->tab_cb;
//     int i = true_args->i;

//     char *rep;
//     int err;

//     char emetteur[255], type[255], valeur[255];
//     int decoupeOk;
//     //---------------------------------------------------------------------- 
//     while(1){
//         // 1- On lit la demande du terminal
//         rep = litLigne(fd_fromTerminal) ;
//         if (rep == 0) {
//             perror("Acquisition : fd_pipeTerminalAcquisition - ecritLigne");
//             exit(0);
//         }
//         printf("Serveur Acquisition : message recu\n");


//         // 2- Ecire le fd de sortie dans la memoire partagée
//         decoupeOk = decoupe(rep, emetteur, type, valeur);
//         if (!decoupeOk) {
//             printf("Erreur de découpage!!\n");
//             //printf("%s", messageAutorisation);
//             exit(0);
//         }
//         tab_cb[i] = *emetteur;
//         Tab_fd_Term[i] = fd_toTerminal;


//         // 3- On transmet la demande au serveur d'autorisation
//         err = ecritLigne(fd_toAuto, rep);
//         if (err == 0) {
//             perror("Acquisition : fd_pipeAcquisitionAutorisation - ecritLigne");
//             exit(0);
//         }
//         printf("Serveur Acquisition : message envoyé\n");
//     }
}
//---------------------------------------------------------------------- 

void *thread_LectureReponse(void *arg){
    arg_thread_A *true_arg = arg;
    int fd_fromAuto = true_arg->fd_fromAuto;
    int* Tab_fd_Term = true_arg->Tab_fd_Term;
    char* tab_cb = true_arg->tab_cb;

    int fd_toTerminal;
    char* rep;
    int err;

    char emetteur[255], type[255], valeur[255];
    int decoupeOk;

    int fd_pas_trouve = 1;
    int i = 0;
    //---------------------------------------------------------------------- 
    printf("thread autorisation\n");
    // while(1){
    //     // 1- On attend la reponse du serveur d'autorisation
    //     rep = litLigne(fd_fromAuto);
    //     if (rep == 0) {
    //         perror("TestLectureEcriture - litLigne");
    //         exit(0);
    //         }
    //     printf("Serveur Acquisition : reponse recu\n");


    //     // 2- Lire le fd pour renvoyer la reponse
    //     decoupeOk = decoupe(rep, emetteur, type, valeur);
    //     if (!decoupeOk) {
    //         printf("Erreur de découpage!!\n");
    //         //printf("%s", messageAutorisation);
    //         exit(0);
    //     }
    //     // while(fd_pas_trouve){
    //         // char cb = tab_cb[i];
    //         // printf("%d\n", cb);
    //         // if(strcmp(emetteur,cb) == 0){
    //         //     fd_pas_trouve = 0;
    //         //     break;
    //         // }  
    //         // i++;
    //     // }
    //     fd_toTerminal = Tab_fd_Term[i];


    //     // 3- On renvoie la reponse au terminal 
    //     err = ecritLigne(fd_toTerminal, rep);
    //     if (err == 0) {
    //         perror("Acquisition : fd_pipeAcquisitionTerminal - ecritLigne");
    //         exit(0);
    //     }
    //     printf("Serveur Acquisition : reponse envoyée\n");
    // }
}
//---------------------------------------------------------------------- 

int main(int argc, char **argv) { 

    if(argc < 2){
        fprintf(stderr, "Erreur veuillez préciser le nombre de termminaux.\nUsage: ./Acquistion 3\n");
        exit(0);
    }

    int nbTerminal;
    nbTerminal = atoi(argv[1]);
    if (nbTerminal < 1){
        fprintf(stderr,"Erreur pas de terminaux\n");
        exit(0);
    }
    //---------------------------------------------------------------------- 
    int fd_pipeAcquisitionAutorisation[2] ;
    int fd_pipeAutorisationAcquisition[2] ;
    int fd_pipeAcquisitionTerminal[2][nbTerminal];
    int fd_pipeTerminalAcquisition[2][nbTerminal];
    int fd_pipe_AT[2];
    int fd_pipe_TA[2];
    int i;

    char* Tab_cb = malloc(sizeof(char[255])*nbTerminal);
    int* Tab_fd_Term = malloc(sizeof(int)*nbTerminal);

    int p;
    char *rep ;
    char arg1[255], arg2[255]; 

    int err;
    int err_exec;

    pthread_t thread_auto;
    pthread_t tab_thread[nbTerminal];
    arg_thread_A *args_auto  = malloc(sizeof(arg_thread_A));
    arg_thread_T * args_term = malloc(sizeof(arg_thread_T));
    //---------------------------------------------------------------------- 

    // PIPES ENTRE ACQUISITION ET AUTORISATION
    if(pipe(fd_pipeAcquisitionAutorisation) != 0) 
    	fprintf(stderr,"Erreur creation fd_pipeAcquisitionAutorisation\n");

    if(pipe(fd_pipeAutorisationAcquisition) != 0)
    	fprintf(stderr,"Erreur creation fd_pipeAutorisationAcquisition\n");

    //PIPES ENTRE ACQUISITION ET TERMINAUX
    for(i = 0 ; i<nbTerminal ; i++){
        // Creation des pipes AcquisitionTerminal
        if(pipe(fd_pipe_AT)!= 0) 
            fprintf(stderr,"Erreur creation fd_pipeAcquisitionTerminal\n");
        fd_pipeAcquisitionTerminal[W][i] = fd_pipe_AT[W];
        fd_pipeAcquisitionTerminal[R][i] = fd_pipe_AT[R];
        
        // Creation des pipes TerminalAcquisition
        if(pipe(fd_pipe_TA)!= 0) 
            fprintf(stderr,"Erreur creation fd_pipeTerminalAcquisition\n");
        fd_pipeTerminalAcquisition[W][i] = fd_pipe_TA[W];
        fd_pipeTerminalAcquisition[R][i] = fd_pipe_TA[R];
    }
    //---------------------------------------------------------------------- 

    //CONFIG INITIALE
    p=fork();
    if(p == -1)
        printf("error");
        
    if(p==0){
        // PROCESSUS FILS
        // Ceci devient le serveur d'autorisation

        sprintf(arg1,"%d", fd_pipeAcquisitionAutorisation[R]);
        sprintf(arg2,"%d", fd_pipeAutorisationAcquisition[W]);

        err_exec = execl("./Autorisation","./Autorisation",arg1, arg2,  (char *)0);
        perror("Acquisition - initialisation : Le serveur d'autorisation s'est mal initialisé: ");
    }

    
    
    for(i = 0 ; i<nbTerminal ; i++){
        p = fork();
        if(p == -1){
            printf("error");
            exit(0);
        }

        if(p == 0){
            // PROCESSUS FILS
            // Ceci devient le terminal
            sprintf(arg1,"%d", fd_pipeTerminalAcquisition[W][i]);
            sprintf(arg2,"%d", fd_pipeAcquisitionTerminal[R][i]);

            err_exec = execl( "/usr/bin/xterm", "/usr/bin/xterm", "-hold", "-e", "./Terminal", arg1, arg2, (char *)NULL );
            perror("Acquisition - initialisation : Le terminal s'est mal initialisé: ");    
        }

    }

    //----------------------------------------------------------------------  
    // PROCESSUS PERE 
    // Ceci reste le serveur d'acquisition
    printf("Avant la création des args du thread.\n");

    args_auto->fd_fromAuto = fd_pipeAutorisationAcquisition[R];
    args_auto->Tab_fd_Term = Tab_fd_Term;
    args_auto->tab_cb = Tab_cb;

    printf("Avant la création du thread.\n");
    if (pthread_create(&thread_auto, NULL, thread_LectureReponse, args_auto)) {
        perror("pthread_create");
        return EXIT_FAILURE;
    }
    
   
    // Creation des threads lisant les terminaux
    args_term->fd_toAuto = fd_pipeAcquisitionAutorisation[W];
    args_term->Tab_fd_Term = Tab_fd_Term;
    args_term->tab_cb = Tab_cb;

    for (i = 0 ; i<nbTerminal ; i++){
        args_term->fd_fromTerminal = fd_pipeTerminalAcquisition[R][i];
        args_term->fd_toTerminal = fd_pipeAcquisitionTerminal[W][i];
        args_term->i = i;
        
        if (pthread_create(&tab_thread[i], NULL, thread_LectureDemande, args_term)) {
            perror("pthread_create");
            return EXIT_FAILURE;
        }
    }

    
    for (i = 0 ; i<nbTerminal ; i++){
        if (pthread_join(tab_thread[i], NULL)) {
            perror("pthread_join");
            return EXIT_FAILURE;
        }
    }
    if (pthread_join(thread_auto, NULL)) {
        perror("pthread_join");
        return EXIT_FAILURE;
    }
     
        
    printf("Après la création du thread.\n");
    free(args_auto);
    free(args_term);
    return 0;
}