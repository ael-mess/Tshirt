/** Fichier serveur.c **/
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
	//Lecture des arguments de la commande 
	char *service = analyseArguments(argc,argv);
    printf("service tcp :%s\n", service);
    printf("service udp :4242\n");

	//Initialisation du serveur 
	int s_serveur=initialisationServeur(service);
    int s_client_udp=initialisationSocketUDP("4242");
	
    //Lancement de la boucle d'ecoute 
	//boucleServeur(s_serveur, wrapper_gestionClient);
	serveurMessages(s_client_udp, traitement);
    close(s_client_udp);
    return 0;
}


/*int main(int argc,char *argv[]){
	if(argc!=3){
	  fprintf(stderr,"Syntaxe : %s <serveur> <message>\n",argv[0]);
	  exit(EXIT_FAILURE);
	  }
	char *hote=argv[1];
	char *message=argv[2];
	char *service="4000";
	messageUDP(hote,service,(unsigned char *)message,strlen(message));
	return 0;
}*/
