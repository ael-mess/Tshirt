/** fichier serveur.h **/

/*********************************************/
/* Structures necessaires pour le serveur    */
/*********************************************/

/** Constantes **/

/** Structures **/
typedef struct
{
    int id;
    int x;
    int y;
    int z;
    int temp;
} DATA;

typedef struct
{
    DATA data[7];
    int fin;
} DATAlist;

/** Variables publiques  **/
int gestionClient(int s);
void *wrapper_gestionClient(void *arg);
char *analyseArguments(int argc, char *argv[]);
