/** Include files **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "serveur.h"
/** Some constants **/

#define WEB_DIR Serveur/www
#define LOG_DIR Serveur/log
#define PAGE_NOTFOUND "./Serveur/www/error.html"
#define MAX_BUFFER 1024

#define CODE_OK  200
#define CODE_NOTFOUND 404

extern DATAlist data;

//Serveur web
void http(FILE *out) {
    //char buffer[MAX_BUFFER];
    char * buffer = (char *) malloc(sizeof(char)*MAX_BUFFER);
    char cmd[MAX_BUFFER];
    char page[MAX_BUFFER];
    char proto[MAX_BUFFER];
    char path[MAX_BUFFER];
    char type[MAX_BUFFER];

    if(fgets(buffer,MAX_BUFFER,out)==NULL) exit(-1);
    if(sscanf(buffer,"%s %s %s",cmd,page,proto)!=3) exit(-1);
    printf("%s\n", buffer);
    
    //if(strcmp(buffer,"GET "))
    while(fgets(buffer,MAX_BUFFER,out)!=NULL){
        if(strcmp(buffer,"\r\n")==0) break;
    }
    if(strcasecmp(cmd,"GET")==0){
        int code=CODE_OK;
        struct stat fstat;
        sprintf(path,".%s",page);
        if(stat(path,&fstat)!=0 || !S_ISREG(fstat.st_mode)){
            sprintf(path,"%s",PAGE_NOTFOUND);
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
        fprintf(out,"Content-length: %lld\r\n",fstat.st_size);
        fprintf(out,"\r\n");
        fflush(out);
        printf("new client (tcp) at %s, len %lld\n", path, fstat.st_size);
        FILE *fd=fopen(path,"r");
		if(fd!=NULL){
		    while(fgets(buffer, fstat.st_size, fd)!=NULL) fputs(buffer,out);
		    fclose(fd);
            free(buffer);
		}
    }
}

//importation de valeurs.html depuis un fichier génirique 
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

//importation de graph.html depuis un fichier génirique 
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
