/** Fichier serveur.c **/
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "libcom.h"
#include "libthrd.h"
#include "serveur.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


/** Some constants **/

#define WEB_DIR  "./www"
#define PAGE_NOTFOUND "error.html"
#define PAGE1 "page1.html"
#define MAX_BUFFER 1024

#define CODE_OK  200
#define CODE_NOTFOUND 404

/***********************************************************/
/** Serveur pour le serveur                               **/
/***********************************************************/

/** Fichiers d'inclusion **/

/** Constantes **/

/** Variables publiques **/
extern char *optarg;
extern int optind, opterr, optopt;

/** Variables statiques **/

/** Procedure principale **/

int gestionClient(int s)
{
	extern DATA data;
    printf("function %d\n", s);
	/* Obtient une structure de fichier */
	FILE *dialogue=fdopen(s,"a+");
	if(dialogue==NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }

	fprintf(dialogue, "HTTP/1.1 200 OK\n");
	//fprintf(dialogue, "Content-length: 146\n");
	fprintf(dialogue, "Content-Type: text/html\n\n");
	fprintf(dialogue, "<html>\n\n<body>\n\n");
	fprintf(dialogue, "<h1>Tshirt values :</h1>\n\n");
	fprintf(dialogue, "<p>id : %d</p>\n\n",data.id);
	fprintf(dialogue, "<p>x : %d</p>\n\n",data.x);
	fprintf(dialogue, "<p>y : %d</p>\n\n",data.y);
	fprintf(dialogue, "<p>z : %d</p>\n\n",data.z);
	fprintf(dialogue, "<p>temp : %d</p>\n\n",data.temp);
	fprintf(dialogue, "</body>\n\n</html>\n\n");
  		
  	
	
	/* Termine la connexion */
	fclose(dialogue);

    return 0;
}

void *wrapper_gestionClient(void *arg)
{
    int s = *((int *) (arg));

    int statut=gestionClient(s);
    if(statut!=0) { perror("gesionClient.wrapper"); exit(EXIT_FAILURE); }
    return NULL;
}

void *wrapper_serveurMessages(void *arg)
{
	int s = *((int *) (arg));

	printf("coucou\n");
	int statut = serveurMessages(s, traitement);
	if(statut!=0) { perror("serveurMessages.wrapper"); exit(EXIT_FAILURE); }
	return NULL;
}

char *analyseArguments(int argc, char *argv[])
{
    static struct option opt = {"port", 1, 0, 'p'};
    if(getopt_long(argc, argv, "p:", &opt, NULL) == 'p') return optarg; //aqpaq
    else return "80";
}

int main(int argc,char *argv[])
{
	//Lecture des arguments de la commande 
	char *service = analyseArguments(argc,argv);
    printf("service tcp :%s\n", service);
    printf("service udp :1024\n");

	//Initialisation du serveur 
	int s_serveur=initialisationServeur(service);
    int s_serveur_udp=initialisationSocketUDP("1024");
	
    //Lancement de la boucle d'ecoute
	lanceThread(wrapper_serveurMessages, (void *)&s_serveur_udp, 555555);
	boucleServeur(s_serveur, wrapper_gestionClient);
    close(s_serveur_udp);
    return 0;
}
