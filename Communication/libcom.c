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
int s_serveur_udp;


DATA data;


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
    s_serveur_udp=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
    if(s_serveur_udp<0){ perror("initialisationSocketUDP.socket"); exit(EXIT_FAILURE); }

    /* Options utiles */
    int vrai=1;
    if(setsockopt(s_serveur_udp,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai))<0){
      perror("initialisationServeurUDPgenerique.setsockopt (REUSEADDR)");
      exit(-1);
      }

    /* Specification de l'adresse de la socket */
    statut=bind(s_serveur_udp,resultat->ai_addr,resultat->ai_addrlen);
    if(statut<0) {perror("initialisationServeurUDP.bind"); exit(-1);}

    /* Liberation de la structure d'informations */
    freeaddrinfo(origine);

    return s_serveur_udp;
}

int serveurMessages(int s, void *(*traitement)(void *))
{
	int i=0;
    while(i<5){
        struct sockaddr_storage adresse;
        socklen_t taille=sizeof(adresse);
        unsigned char message[1500];
        unsigned char strH[11];
        int nboctets=recvfrom(s,message,1500,0,(struct sockaddr *) &adresse,&taille);
        if(nboctets<0) return -1;
    	memset(strH,0,sizeof(strH));
    	int i,j;
        for(i=0,j=0;i<5;i++,j+=2)
		{
		    sprintf((char*)strH+j,"%02X",message[i]); //Conversion de la data en hexa
		}
		strH[j]='\0'; /*adding NULL in the end*/
        
        if(lanceThread(traitement, (void *) strH,50)<0) break;
        printf("udp sever loop %d\n", s);

        char host[NI_MAXHOST], service[NI_MAXSERV];
        int ser = getnameinfo((struct sockaddr *) &adresse, taille, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);

        if (ser == 0) {
            printf("Received %ld bytes from %s:%s\n", (long) nboctets, host, service);
            char messrec[32] = "reçu gy-h_i frère\n";
            if(sizeof(messrec)!=sendto(s,messrec,sizeof(messrec),0,(struct sockaddr *) &adresse,taille)) perror("serveurMessages.sendto");
        }
        else perror("serveurMessages.getnameinfo");
	
	i++;
	}
	envoiMessage("4241", "	broadcasrt frere ", 5465);
	envoiMessageUnicast("4241", "127.0.0.1", " 	unicast sdfgser", 54);
    return 0;
}

int envoiMessage(char * service, char *message, int taille)
{
	printf("après boucle\n");
    struct sockaddr_in adresse;
    memset(&adresse,0,sizeof(adresse));
    adresse.sin_family = AF_INET;
    adresse.sin_addr.s_addr = inet_addr("INADDR_BROADCAST");
    adresse.sin_port=htons(atoi(service));

	printf("sfd : %d\n", s_serveur_udp);

    int vrai=1;
    if(setsockopt(s_serveur_udp,SOL_SOCKET,SO_BROADCAST,&vrai,sizeof(vrai))<0){
        perror("envoiMessage.setsockopt (BROADCAST)");
        exit(-1);
    }

    if(sendto(s_serveur_udp,(const char *)message,strlen((const char *)message),0,(struct sockaddr *)&adresse,sizeof(adresse)) <0)
		perror("envoiMessage.sendto");
    return 0;
}

int envoiMessageUnicast(char * service, char * machine, char *message, int taille)
{
	struct addrinfo precisions,*resultat,*origine;

    memset(&precisions,0,sizeof(precisions));
    precisions.ai_family=AF_UNSPEC;
    precisions.ai_socktype=SOCK_DGRAM;
    precisions.ai_flags=AI_PASSIVE;
    int statut=getaddrinfo(machine,service,&precisions,&origine);
    if(statut<0){ perror("envoiMessageUnicast.getaddrinfo"); exit(EXIT_FAILURE); }
    struct addrinfo *p;
    for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
    if(p->ai_family==AF_INET6){ resultat=p; break; }

    if(sendto(s_serveur_udp,(const char *)message,strlen((const char *)message),0,(struct sockaddr *)resultat->ai_addr,resultat->ai_addrlen) <0)
		perror("envoiMessage.sendto");
    return 0;
}


void *traitement(void *message)
{
    unsigned char * msg = (unsigned char *) message;
    //unsigned char strH[200];
    int i,j;
    //memset(strH,0,sizeof(strH));
    printf("transmitted message is : %s\n",msg);
    data.id=((msg[0]-48)*10+msg[1]-48);
    data.x=((msg[2]-48)*10+msg[3]-48);
    data.y=((msg[4]-48)*10+msg[5]-48);
    data.z=((msg[6]-48)*10+msg[7]-48);
    data.temp=((msg[8]-48)*10+msg[9]-48);
    return 0;
}
