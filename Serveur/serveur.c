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
#include <semaphore.h>

#define WEB_DIR  Serveur/www
#define LOG_DIR  Serveur/log
/***********************************************************/
/** Serveur pour le serveur                               **/
/***********************************************************/

/** Fichiers d'inclusion **/

/** Constantes **/

/** Variables publiques **/
extern char *optarg;
extern int optind, opterr, optopt;
DATAlist data;
sem_t s_log, s_html;

/** Variables extern **/
extern void *traitementUDP(void *);

/** Procedure principale **/

//gestion thread sur socket tcp
int gestionClient(int s)
{
	/* Obtient une structure de fichier */
	FILE *dialogue=fdopen(s,"a+");
	if(dialogue==NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }

	//importation des fichier html avec sémaphore
	sem_wait(&s_html);
	values();
    sem_wait(&s_log);
    graph();
    sem_post(&s_log);
    sem_post(&s_html);

    //Lancement du serveur web
    sem_wait(&s_html);
    http(dialogue);
    sem_post(&s_html);
    /*char buffer[1024];
    while(fgets(buffer, 1024, dialogue)!=NULL) puts(buffer);*/
	
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

//wrapper serveur udp
void *wrapper_serveurMessages(void *arg)
{
	int s = *((int *) (arg));

	int statut = serveurMessages(s, traitementUDP);
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
    data.fin=-1;
	
    //Lancement de la boucle d'ecoute
    sem_init(&s_log, 0, 1);
    sem_init(&s_html, 0, 1);
	lanceThread(wrapper_serveurMessages, (void *)&s_serveur_udp);
	boucleServeur(s_serveur, wrapper_gestionClient);
    close(s_serveur_udp);
    sem_destroy(&s_log);
    sem_destroy(&s_html);
    return 0;
}
