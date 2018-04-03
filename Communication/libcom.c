/** fichier libcom.c **/

/*****************************************/
/** Ce fichier contient des fonctions  **/
/**  concernant les sockets.           **/
/****************************************/

/**** Fichiers d'inclusion ****/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "libcom.h"

/**** Constantes ****/

#define MAX_TAMPON	1024

/**** Variables globales *****/
int sudp;

/**** Prototype des fonctions locales *****/

/**** Fonctions de gestion des sockets ****/

int initialisationServeur(char *service)
{
    struct addrinfo precisions,*resultat,*origine;
    int statut;
    int s;

    /* Construction de la structure adresse */
    memset(&precisions,0,sizeof(precisions));
    precisions.ai_family=AF_UNSPEC;
    precisions.ai_socktype=SOCK_STREAM;
    precisions.ai_flags=AI_PASSIVE;
    statut=getaddrinfo(NULL,service,&precisions,&origine);
    if(statut<0){ perror("initialisationServeur.getaddrinfo"); exit(EXIT_FAILURE); }
    struct addrinfo *p;
    for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
    if(p->ai_family==AF_INET6){ resultat=p; break; }

    /* Creation d'une socket */
    s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
    if(s<0){ perror("initialisationServeur.socket"); exit(EXIT_FAILURE); }

    /* Options utiles */
    int vrai=1;
    if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai))<0){
    perror("initialisationServeur.setsockopt (REUSEADDR)");
    exit(EXIT_FAILURE);
    }
    if(setsockopt(s,IPPROTO_TCP,TCP_NODELAY,&vrai,sizeof(vrai))<0){
    perror("initialisationServeur.setsockopt (NODELAY)");
    exit(EXIT_FAILURE);
    }

    /* Specification de l'adresse de la socket */
    statut=bind(s,resultat->ai_addr,resultat->ai_addrlen);
    if(statut<0) return -1;

    /* Liberation de la structure d'informations */
    freeaddrinfo(origine);

    /* Taille de la queue d'attente */
    statut=listen(s,50);
    if(statut<0) return -1;

    return s;
}

int boucleServeur(int s,void *(*traitement)(void *))
{
    int dialogue;
    while(1){

        /* Attente d'une connexion */
        if((dialogue=accept(s,NULL,NULL))<0) return -1;

        /* Passage de la socket de dialogue a la fonction de traitement */
        if(lanceThread(traitement, (void *)&dialogue,50)<0){ shutdown(s,SHUT_RDWR); return 0;}
        printf("tcp sever loop :%d\n", s);
    }
}

int initialisationSocketUDP(char *service){
    struct addrinfo precisions,*resultat,*origine;
    int statut;

    /* Construction de la structure adresse */
    memset(&precisions,0,sizeof precisions);
    precisions.ai_family=AF_UNSPEC;
    precisions.ai_socktype=SOCK_DGRAM;
    precisions.ai_flags=AI_PASSIVE;
    statut=getaddrinfo(NULL,service,&precisions,&origine);
    if(statut<0){ perror("initialisationSocketUDP.getaddrinfo"); exit(EXIT_FAILURE); }
    struct addrinfo *p;
    for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
      if(p->ai_family==AF_INET6){ resultat=p; break; }

    /* Creation d'une socket */
    sudp=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
    if(sudp<0){ perror("initialisationSocketUDP.socket"); exit(EXIT_FAILURE); }

    /* Options utiles */
    int vrai=1;
    if(setsockopt(sudp,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai))<0){
      perror("initialisationServeurUDPgenerique.setsockopt (REUSEADDR)");
      exit(-1);
      }

    /* Specification de l'adresse de la socket */
    statut=bind(sudp,resultat->ai_addr,resultat->ai_addrlen);
    if(statut<0) {perror("initialisationServeurUDP.bind"); exit(-1);}

    /* Liberation de la structure d'informations */
    freeaddrinfo(origine);

    return sudp;
}

int serveurMessages(int s,void *(*traitement)(void *))
{
    while(1){
        struct sockaddr_storage adresse;
        socklen_t taille=sizeof(adresse);
        unsigned char message[1500];
        int nboctets=recvfrom(s,message,1500,0,(struct sockaddr *) &adresse,&taille);
        if(nboctets<0) return -1;
        if(lanceThread(traitement, (void *) message,50)<0) break;
        printf("udp sever loop %d\n", s);

        char host[NI_MAXHOST], service[NI_MAXSERV];
        int ser = getnameinfo((struct sockaddr *) &adresse, taille, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);

        if (ser == 0) {
            printf("Received %ld bytes from %s:%s\n", (long) nboctets, host, service);
            char messrec[32] = "reçu frère\n";
            if(sizeof(messrec)!=sendto(s,messrec,sizeof(messrec),0,(struct sockaddr *) &adresse,taille)) perror("serveurMessages.sendto");
        }
        else perror("serveurMessages.getnameinfo");
        envoiMessage("4242", "  wesh frère\n", 54);
    }
    return 0;
}

int envoiMessage(char * service, unsigned char *message, int taille)
{
    //struct sockaddr_storage adresse;
    //socklen_t taille=sizeof(adresse);

    struct sockaddr_in adresse;
    memset(&adresse,0,sizeof(adresse));
    adresse.sin_family = AF_INET;
    adresse.sin_addr.s_addr = inet_addr("127.0.0.1");
    adresse.sin_port=htons(4242);

    /*int vrai=1;
    if(setsockopt(sudp,SOL_SOCKET,SO_BROADCAST,&vrai,sizeof(vrai))<0){
        perror("envoiMessage.setsockopt (BROADCAST)");
        exit(-1);
    }*/
    if(0>sendto(sudp,(const char *)message,strlen((const char *)message),0,(struct sockaddr *) &adresse,sizeof(adresse))) perror("envoiMessage.sendto");
    return 0;
}


void *traitement(void *message)
{
    unsigned char * msg = (unsigned char *) message;
    printf("traitement %s\n", msg);
    return 0;
}
