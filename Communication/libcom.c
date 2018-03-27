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

/**** Prototype des fonctions locales *****/

/**** Fonctions de gestion des sockets ****/

int initialisationServeur(char *service)
{
    struct addrinfo precisions,*resultat,*origine;
    int statut;
    int s;

    /* Construction de la structure adresse */
    memset(&precisions,0,sizeof precisions);
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
    printf("while loop serv sfd :%d\n", s);
    int dialogue;
    while(1){

        /* Attente d'une connexion */
        if((dialogue=accept(s,NULL,NULL))<0) return -1;

        /* Passage de la socket de dialogue a la fonction de traitement */
        if(lanceThread(traitement, (void *)&dialogue,50)<0){ shutdown(s,SHUT_RDWR); return 0;}
    }
}

int initialisationSocketUDP(char *service){
    struct addrinfo precisions,*resultat,*origine;
    int statut;
    int s;

    /* Construction de la structure adresse */
    memset(&precisions,0,sizeof precisions);
    precisions.ai_family=AF_UNSPEC;
    precisions.ai_socktype=SOCK_DGRAM;
    precisions.ai_flags=AI_PASSIVE;
    statut=getaddrinfo("127.0.0.1",service,&precisions,&origine);
    if(statut<0){ perror("initialisationSocketUDP.getaddrinfo"); exit(EXIT_FAILURE); }
    struct addrinfo *p;
    for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
      if(p->ai_family==AF_INET6){ resultat=p; break; }

    /* Creation d'une socket */
    s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
    if(s<0){ perror("initialisationSocketUDP.socket"); exit(EXIT_FAILURE); }

    /* Options utiles */
    int vrai=1;
    if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai))<0){
      perror("initialisationServeurUDPgenerique.setsockopt (REUSEADDR)");
      exit(-1);
      }

    /* Specification de l'adresse de la socket */
    statut=bind(s,resultat->ai_addr,resultat->ai_addrlen);
    if(statut<0) {perror("initialisationServeurUDP.bind"); exit(-1);}

    /* Liberation de la structure d'informations */
    freeaddrinfo(origine);

    return s;
}

int serveurMessages(int s,void *(*traitement)(void *))
{
    while(1){
        printf("inside while\n");
        struct sockaddr_storage adresse;
	struct addrinfo precisions,*resultat,*origine;
        socklen_t taille=sizeof(adresse);
        unsigned char message[1500];
        printf("before int\n");
        int nboctets=sendto(s,message,1500,0,(struct sockaddr *)&adresse,taille);
        printf("ndoctets = %d\n",nboctets);
        printf("before if\n");
        if(nboctets<0) return -1;
        printf("after if\n");
        if(lanceThread(traitement, (void *) message,50)<0) break;
    }
    return 0;
}

int envoiMessage(char * s, unsigned char * sz, int d)
{
	return 0;
}


void *traitement(void *message)
{
    unsigned char * msg = (unsigned char *) message;
    printf("salut %s\n", msg);
    return 0;
}

void messageUDP(char *hote,char *service,unsigned char *message,int taille){
	struct addrinfo precisions,*resultat,*origine;
	int statut;
	int s;

	/* Creation de l'adresse de socket */
	memset(&precisions,0,sizeof precisions);
	precisions.ai_family=AF_UNSPEC;
	precisions.ai_socktype=SOCK_DGRAM;
	statut=getaddrinfo(hote,service,&precisions,&origine);
	if(statut<0){ perror("messageUDPgenerique.getaddrinfo"); exit(EXIT_FAILURE); }
	struct addrinfo *p;
	for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
	  if(p->ai_family==AF_INET6){ resultat=p; break; }

	/* Creation d'une socket */
	s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
	if(s<0){ perror("messageUDPgenerique.socket"); exit(EXIT_FAILURE); }

	/* Option sur la socket */
	int vrai=1;
	if(setsockopt(s,SOL_SOCKET,SO_BROADCAST,&vrai,sizeof(vrai))<0){
	  perror("initialisationServeurUDPgenerique.setsockopt (BROADCAST)");
	  exit(-1);
	  }

	/* Envoi du message */
	int nboctets=recvfrom(s,message,taille,0,resultat->ai_addr,&resultat->ai_addrlen);
	if(nboctets<0){ perror("messageUDPgenerique.sento"); exit(EXIT_FAILURE); }

	printf("Message : %s \n", message);

	/* Liberation de la structure d'informations */
	freeaddrinfo(origine);

	/* Fermeture de la socket d'envoi */
	close(s);
}
