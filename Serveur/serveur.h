/** fichier serveur.h **/

/*********************************************/
/* Structures necessaires pour le serveur    */
/*********************************************/

/** Constantes **/

/** Structures **/

/** Variables publiques  **/
int gestionClient(int s);
void *wrapper_gestionClient(void *arg);
char *analyseArguments(int argc, char *argv[]);
