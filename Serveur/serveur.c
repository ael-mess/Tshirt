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
DATAlist data;
sem_t s_fichier;

/** Variables statiques **/

/** Procedure principale **/

int gestionClient(int s)
{
    sem_wait(&s_fichier);
	FILE* fichier = NULL;
	fichier = fopen("Serveur/index.html", "r");
	if(fichier==NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }
	int caractereActuel = 0;
	
    printf("function %d\n", s);
	/* Obtient une structure de fichier */
	FILE *dialogue=fdopen(s,"a+");
	if(dialogue==NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }

	fprintf(dialogue, "HTTP/1.1 200 OK\n");
	//fprintf(dialogue, "Content-length: 146\n");
	fprintf(dialogue, "Content-Type: text/html\n\n");
	fprintf(dialogue, "<html>\n<title>Tshirt</title>\n<body>\n");
	fprintf(dialogue, "<h1>Tshirt values :</h1>\n\n");
    fprintf(dialogue, "<p><a href=\"index.html\"><input type=\"submit\" value=\"Graph!\"></a></p>");
	fprintf(dialogue, "<p>id : %d</p>\n\n",data.data[data.fin].id);
	fprintf(dialogue, "<p>acc x : %d</p>\n\n",data.data[data.fin].x);
	fprintf(dialogue, "<p>acc y : %d</p>\n\n",data.data[data.fin].y);
	fprintf(dialogue, "<p>acc z : %d</p>\n\n",data.data[data.fin].z);
	fprintf(dialogue, "<p>temp : %d</p>\n\n",data.data[data.fin].temp);
	fprintf(dialogue, "</body>\n\n</html>\n\n");
  	/*if (fichier != NULL) {
        // Boucle de lecture des caractères un à un
		do {
			caractereActuel = fgetc(fichier); // On lit le caractère
			fprintf(dialogue, "%c", caractereActuel); // On l'affiche
		} while (caractereActuel != EOF); // On continue tant que fgetc n'a pas retourné EOF (fin de fichier)
		 
		fclose(fichier);
	}*/
  	
	
	/* Termine la connexion */
	fclose(dialogue);
    fclose(fichier);
    sem_post(&s_fichier);

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

int HexToInt(char a)
{
	//printf("%d\n",a);
	if(a<58) return a-48;
	else return a-55;
}

void *traitement(void *message)
{
    sem_wait(&s_fichier);
    unsigned char * msg = (unsigned char *) message;
    FILE * acc = fopen("Serveur/acceleration.txt", "ab");
    FILE * temp = fopen("Serveur/temperature.txt", "ab");
    FILE * cap = fopen("Serveur/capteurs.txt", "ab");
    //unsigned char strH[200];
    //memset(strH,0,sizeof(strH));
    printf("transmitted message is : %s\n",msg);

    data.fin++;
	data.data[data.fin].id=HexToInt(msg[0])*16+HexToInt(msg[1]);
    fprintf(cap, "%d (%d)acceleration température\n",data.fin, data.data[data.fin].id);
	data.data[data.fin].x=HexToInt(msg[2])*16+HexToInt(msg[3]);
    fprintf(acc, "%d ",data.data[data.fin].x);
	data.data[data.fin].y=HexToInt(msg[4])*16+HexToInt(msg[5]);
    fprintf(acc, "%d ",data.data[data.fin].y);
    data.data[data.fin].z=HexToInt(msg[6])*16+HexToInt(msg[7]);
    fprintf(acc, "%d\n",data.data[data.fin].z);
    data.data[data.fin].temp=HexToInt(msg[8])*16+HexToInt(msg[9]);
    fprintf(temp, "%d\n",data.data[data.fin].temp);

    fclose(acc);
    fclose(cap);
    fclose(temp);
    sem_post(&s_fichier);
    return 0;
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
    sem_init(&s_fichier, 0, 1);
	lanceThread(wrapper_serveurMessages, (void *)&s_serveur_udp, 555555);
	boucleServeur(s_serveur, wrapper_gestionClient);
    close(s_serveur_udp);
    sem_destroy(&s_fichier);
    return 0;
}
