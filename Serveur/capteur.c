
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <semaphore.h>

#include "serveur.h"

extern sem_t s_log, s_html;
extern DATAlist data;

//traitement message udp
void *traitementUDP(void *message)
{
    unsigned char * msg = (unsigned char *) message;
    unsigned char strH[11];
    memset(strH,0,sizeof(strH));
    int i,j;
    for(i=0,j=0;i<5;i++,j+=2) sprintf((char*)strH+j,"%02X",msg[i]); //Conversion de la data en hexa
    strH[j]='\0'; //adding NULL in the end

    //traitement
    DATA d;
	d.id=HexToInt(strH[0])*16+HexToInt(strH[1]);
	d.x=HexToInt(strH[2])*16+HexToInt(strH[3]);
	d.y=HexToInt(strH[4])*16+HexToInt(strH[5]);
    d.z=HexToInt(strH[6])*16+HexToInt(strH[7]);
    d.temp=HexToInt(strH[8])*16+HexToInt(strH[9]);

    //enregistrement dans la struct
    enre_valeur(d);

    //enregistrement dans un log avec sémaphore
    sem_wait(&s_log);
    FILE * accx = fopen("Serveur/log/accx.log", "ab");
    FILE * accy = fopen("Serveur/log/accy.log", "ab");
    FILE * accz = fopen("Serveur/log/accz.log", "ab");
    FILE * temp = fopen("Serveur/log/temp.log", "ab");
    if(accx!=NULL && accy!=NULL && accz!=NULL && temp!=NULL) {
        fprintf(accx, "%d %d\n", d.id, d.x);
        fprintf(accy, "%d %d\n", d.id, d.y);
        fprintf(accz, "%d %d\n", d.id, d.z);
        fprintf(temp, "%d %d\n", d.id, d.temp);
        fclose(accx);
        fclose(accy);
        fclose(accz);
        fclose(temp);
    }
    sem_post(&s_log);
    return 0;
}

//entegistrement d'une nouvelle data dans la liste contigue
void enre_valeur(DATA d) {
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
