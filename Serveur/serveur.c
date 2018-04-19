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

/** Variables statiques **/

/** Procedure principale **/

void values() {
    FILE * val = fopen("Serveur/www/valeurs.html", "w");
    FILE * ge = fopen("Serveur/www/valeurs_ge.html", "r");
    if(val!=NULL || ge!=NULL) {
        int i = 0;
        char c = fgetc(ge);
        while(c!=EOF) {
            if(c=='%' && i==0) { fputc(data.data[data.fin].id +48, val); i++; }
            else if(c=='%' && i==1) { fputc(data.data[data.fin].x +48, val); i++; }
            else if(c=='%' && i==2) { fputc(data.data[data.fin].y +48, val); i++; }
            else if(c=='%' && i==3) { fputc(data.data[data.fin].z + 48, val); i++; }
            else if(c=='%' && i==4) { fputc(data.data[data.fin].temp +48, val); i=0; }
            else fputc(c,val);
            c = fgetc(ge);
        }
        fclose(ge);
        fclose(val);
    }
}

void graph() {
    FILE * val = fopen("Serveur/www/graph.html", "w");
    FILE * ge = fopen("Serveur/www/graph_ge.html", "r");

    FILE * accx = fopen("Serveur/log/accx.log", "rb");
    FILE * accy = fopen("Serveur/log/accy.log", "rb");
    FILE * accz = fopen("Serveur/log/accz.log", "rb");
    FILE * temp = fopen("Serveur/log/temp.log", "rb");

    if(val!=NULL && ge!=NULL && accx!=NULL && accy!=NULL && accz!=NULL && temp!=NULL) {
        int j=0, i=0;
        DATA d;
        char c = fgetc(ge);

        while(c!=EOF) {
            if(c=='%' && i==0) {
                j=0;
                while(fscanf(accx,"%d %d", &d.id, &d.x)!=EOF) {
                    fprintf(val,"[%d,%d],",j,d.x);
                    j++;
                }
                i++;
            }
            else if(c=='%' && i==1) {
                j=0;
                while(fscanf(accy,"%d %d", &d.id, &d.y)!=EOF) {
                    fprintf(val,"[%d,%d],",j,d.y);
                    j++;
                }
                i++;
            }
            else if(c=='%' && i==2) {
                j=0;
                while(fscanf(accz,"%d %d", &d.id, &d.z)!=EOF) {
                    fprintf(val,"[%d,%d],",j,d.z);
                    j++;
                }
                i++;
            }
            else if(c=='%' && i==3) {
                j=0;
                while(fscanf(temp,"%d %d", &d.id, &d.temp)!=EOF) {
                    fprintf(val,"[%d,%d],",j,d.temp);
                    j++;
                }
                i++;
            }
            else if(c=='%' && i==4) { fprintf(val, "%d", d.id); i=0; }
            else fputc(c,val);
            c = fgetc(ge);
        }
        fclose(ge);
        fclose(val);
        fclose(accx);
        fclose(accy);
        fclose(accz);
        fclose(temp);
    }
}

int gestionClient(int s)
{
    
    printf("function %d\n", s);
	/* Obtient une structure de fichier */
	FILE *dialogue=fdopen(s,"a+");
	if(dialogue==NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }

	sem_wait(&s_html);
	values();
    sem_wait(&s_log);
    graph();
    sem_post(&s_log);
    sem_post(&s_html);

    printf("before http \n");
    sem_wait(&s_html);
    http(dialogue);
    sem_post(&s_html);
    printf("after http \n");
    char buffer[1024];
    while(fgets(buffer, 1024, dialogue)!=NULL) puts(buffer);
	
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
	int statut = serveurMessages(s, traitementUDP);
	if(statut!=0) { perror("serveurMessages.wrapper"); exit(EXIT_FAILURE); }
	return NULL;
}

void struct_valeur(DATA d) {
    int i, bool=0;
    for(i=0; i<data.fin; i++) {
        if(data.data[i].id == d.id) {
            data.data[i].x = d.x;
            data.data[i].y = d.y;
            data.data[i].z = d.z;
            data.data[i].temp = d.temp;
            bool++;
        }
    }
    if(bool==0) {
        data.fin++;
        data.data[data.fin].x = d.x;
        data.data[data.fin].y = d.y;
        data.data[data.fin].z = d.z;
        data.data[data.fin].temp = d.temp;
    }
}

int HexToInt(char a)
{
	if(a<58) return a-48;
	else return a-55;
}

void *traitementUDP(void *message)
{
    unsigned char * msg = (unsigned char *) message;
    unsigned char strH[11];
    memset(strH,0,sizeof(strH));
    int i,j;
    for(i=0,j=0;i<5;i++,j+=2) sprintf((char*)strH+j,"%02X",msg[i]); //Conversion de la data en hexa
    strH[j]='\0'; //adding NULL in the end

    DATA d;
	d.id=HexToInt(strH[0])*16+HexToInt(strH[1]);
	d.x=HexToInt(strH[2])*16+HexToInt(strH[3]);
	d.y=HexToInt(strH[4])*16+HexToInt(strH[5]);
    d.z=HexToInt(strH[6])*16+HexToInt(strH[7]);
    d.temp=HexToInt(strH[8])*16+HexToInt(strH[9]);

    struct_valeur(d);

    sem_wait(&s_log);
    FILE * accx = fopen("Serveur/log/accx.log", "ab");
    FILE * accy = fopen("Serveur/log/accy.log", "ab");
    FILE * accz = fopen("Serveur/log/accz.log", "ab");
    FILE * temp = fopen("Serveur/log/temp.log", "ab");
    if(accx!=NULL && accy!=NULL && accz!=NULL && temp!=NULL) {
        fprintf(accx, "%d %d\n", d.id, d.x);
        fprintf(accx, "%d %d\n", d.id, d.y);
        fprintf(accx, "%d %d\n", d.id, d.z);
        fprintf(accx, "%d %d\n", d.id, d.temp);
        fclose(accx);
        fclose(accy);
        fclose(accz);
        fclose(temp);
    }
    sem_post(&s_log);
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
    sem_init(&s_log, 0, 1);
    sem_init(&s_html, 0, 1);
	lanceThread(wrapper_serveurMessages, (void *)&s_serveur_udp, 555555);
	boucleServeur(s_serveur, wrapper_gestionClient);
    close(s_serveur_udp);
    sem_destroy(&s_log);
    sem_destroy(&s_html);
    return 0;
}
