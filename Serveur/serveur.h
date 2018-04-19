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
    DATA data[1024];
    int fin;
} DATAlist;

/** Variables publiques  **/
int gestionClient(int s);
void *wrapper_gestionClient(void *arg);
char *analyseArguments(int argc, char *argv[]);
void http(FILE *out);
void values();
void graph();
void enre_valeur(DATA d);
int HexToInt(char a);
void *traitementUDP(void *message);
void detec_chute(DATA d);
