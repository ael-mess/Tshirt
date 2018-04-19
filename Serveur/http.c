/** Include files **/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


/** Some constants **/

#define WEB_DIR  "www"
#define PAGE_NOTFOUND "./Serveur/www/error.html"
#define MAX_BUFFER 1024

#define CODE_OK  200
#define CODE_NOTFOUND 404

void http(FILE *out) {
    char buffer[MAX_BUFFER];
    char cmd[MAX_BUFFER];
    char page[MAX_BUFFER];
    char proto[MAX_BUFFER];
    char path[MAX_BUFFER];
    char type[MAX_BUFFER];

    if(fgets(buffer,MAX_BUFFER,out)==NULL) exit(-1);
    if(sscanf(buffer,"%s %s %s",cmd,page,proto)!=3) exit(-1);
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
        printf("page %s, path %s, len %lld\n", page, path, fstat.st_size);
        FILE *fd=fopen(path,"r");
        printf("    after fopen \n");
		if(fd!=NULL){
		    while(fgets(buffer, fstat.st_size, fd)!=NULL) fputs(buffer,out);
		    fclose(fd);
		}
		printf("  after write \n");
    }
}
