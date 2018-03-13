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

#include <getopt.h>
#include <pthread.h>

#define MAX_TAMPON	1024

/**** Variables globales *****/
extern char *optarg;
extern int optind, opterr, optopt;

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

int boucleServeur(int ecoute,int (*traitement)(int))
{
    int dialogue;

    while(1){

        /* Attente d'une connexion */
        if((dialogue=accept(ecoute,NULL,NULL))<0) return -1;

        /* Passage de la socket de dialogue a la fonction de traitement */
        if(traitement(dialogue)<0){ shutdown(ecoute,SHUT_RDWR); return 0;}
    }
}

void *gestionClient(int *s){
	/* Obtient une structure de fichier */
	FILE *dialogue=fdopen(*s,"a+");
	if(dialogue==NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }

	/* Echo */
	//char ligne[6] = "couco\n";
	//while(fgets(ligne,6,dialogue)!=NULL)
    	fprintf(dialogue,"> coucou\n");

	/* Termine la connexion */
	fclose(dialogue);
}

char *analyseArguments(int argc,char *argv[])
{
    static struct option opt = {"port", 1, 0, 'p'};
    if(getopt_long(argc, argv, "p:", &opt, NULL) == 'p') return optarg; //aqpaq
    else return "80";
}

int lanceThread(void *(*thread)(void *), void *arg, int taille)
{
    pthread_t tid;
    pthread_detach(tid); //compile with -lpthread
    pthread_create(&tid, NULL, thread, arg);
    
    //int status;
    //pthread_exit((void *)&status);
    return tid;
}

int main(int argc,char *argv[])
{
	int s;
	 
	/* Lecture des arguments de la commande */
	//analyseArguments(argc,argv);
	char *service = analyseArguments(argc,argv);
    printf("%s\n", service);
    
	/* Initialisation du serveur */
	s=initialisationServeur(service);
	   
	/* Lancement de la boucle d'ecoute */
	//boucleServeur(s,gestionClient);
	printf("%d\n", s);
	lanceThread((void *(*)(void*))gestionClient, (void *)&s, 50);
}
