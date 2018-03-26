/** Fichier serveur.c **/
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "libcom.h"
#include "libthrd.h"
#include "serveur.h"

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
    printf("function %d\n", s);
	/* Obtient une structure de fichier */
	FILE *dialogue=fdopen(s,"a+");
	if(dialogue==NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }

	/* Echo */
	char ligne[6] = "couco\n";
	while(fgets(ligne,6,dialogue)!=NULL)
    	fprintf(dialogue,"> coucou\n");

	/* Termine la connexion */
	fclose(dialogue);

    return 0;
}

void *wrapper_gestionClient(void *arg)
{
    int s = *((int *) (arg));
    printf("while thread sfd :%d\n", s);

    int statut=gestionClient(s);
    if(statut!=0) { perror("gesionClient.wrapper"); exit(EXIT_FAILURE); }
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
	/* Lecture des arguments de la commande */
	char *service = analyseArguments(argc,argv);
    	printf("service :%s\n", service);

	/* Initialisation du serveur */
	int s=initialisationServeur(service);

	/* Lancement de la boucle d'ecoute */
	//thread_struct ths = {(void *(*)(void*))wrapper_gestionClient, s};
	//boucleServeur(ths, lanceThread);
	boucleServeurUDP(s,traitement);
    close(s);
    return 0;
}

