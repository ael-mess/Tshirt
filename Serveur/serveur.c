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

void http(char page[MAX_BUFFER], FILE * out) {
    char buffer[MAX_BUFFER];
    char path[MAX_BUFFER];
    char type[MAX_BUFFER];

    memset(buffer,0,MAX_BUFFER);
    int code=CODE_OK;
    struct stat fstat;
    sprintf(path,"%s",page);
    if(stat(path,&fstat)!=0 || !S_ISREG(fstat.st_mode)){
        sprintf(path,"%s/%s",WEB_DIR,PAGE_NOTFOUND);
        code=CODE_NOTFOUND;
    }
    strcpy(type,"text/html");
    fprintf(out,"HTTP/1.0 %d OK\r\n",CODE_OK);
    fprintf(out,"Server: CWeb\r\n");
    fprintf(out,"Content-type: %s\r\n",type);
    fprintf(out,"Content-length: %lld\r\n",fstat.st_size);
    fprintf(out,"\r\n");
    fflush(out);
    FILE *fd=fopen(path,"r");
    if(fd!=NULL){
        while(fgets(buffer, fstat.st_size, fd)!=NULL) fputs(buffer,out);
        fclose(fd);
    }
}

void h(FILE *out){
    char buffer[MAX_BUFFER];
    char cmd[MAX_BUFFER];
    char page[MAX_BUFFER];
    char proto[MAX_BUFFER];
    char path[MAX_BUFFER];
    char type[MAX_BUFFER];

    if(fgets(buffer,MAX_BUFFER,out)==NULL) exit(-1);
    if(sscanf(buffer,"%s %s %s",cmd,page,proto)!=3) exit(-1);
    printf("cmd = %s // page = %s // proto = %s\n",cmd,page,proto);
    while(fgets(buffer,MAX_BUFFER,out)!=NULL){
        if(strcmp(buffer,"\r\n")==0) break;
    }
    if(strcasecmp(cmd,"GET")==0){
        int code=CODE_OK;
        struct stat fstat;
        sprintf(path,".%s",page);
        system("ls");
        printf(" %d\n",stat(path,&fstat));
        if(stat(path,&fstat)!=0 || !S_ISREG(fstat.st_mode)){
            sprintf(path,"%s/%s",WEB_DIR,PAGE_NOTFOUND);
            code=CODE_NOTFOUND;
        }
        strcpy(type,"text/html");
        char *end=page+strlen(page);
        if(strcmp(end-4,".png")==0) strcpy(type,"image/png");
        if(strcmp(end-4,".jpg")==0) strcpy(type,"image/jpg");
        if(strcmp(end-4,".gif")==0) strcpy(type,"image/gif");
        fprintf(out,"HTTP/1.0 %d\r\n",code);
        fprintf(out,"Server: CWeb\r\n");
        fprintf(out,"Content-type: %s\r\n",type);
        fprintf(out,"Content-length: %d\r\n",fstat.st_size);
        fprintf(out,"\r\n");
        fflush(out);
        printf("page %s, path %s\n", page, path);
        FILE *fd=fopen(path,"r");
		if(fd!=NULL){
		    while(fgets(buffer, fstat.st_size, fd)!=NULL) fputs(buffer,out);
		    fclose(fd);
		}
    }
}

int gestionClient(int s)
{
    sem_wait(&s_fichier);
    printf("function %d\n", s);
	/* Obtient une structure de fichier */
	FILE *dialogue=fdopen(s,"a+");
	if(dialogue==NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }

	//http("Serveur/index.html", dialogue);
    h(dialogue);
    /*sleep(5);
	//http("Serveur/index.html", dialogue);*/
    char buffer[MAX_BUFFER];
    /*while(fgets(buffer, 1024, dialogue)!=NULL) {
        /*if(strcmp(buffer,"GET /Serveur/valeurs.html HTTP/1.1\r\n")==0) {
            printf("%s\n",buffer);
            http("Serveur/valeurs.html", dialogue);
            break;
        }
        puts(buffer);
    }*/
	
	/* Termine la connexion */
	fclose(dialogue);
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
