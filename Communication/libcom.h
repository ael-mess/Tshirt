/** fichier libcom.h **/

/******************************************************************/
/** Ce fichier decrit les structures et les constantes utilisees **/
/** par les fonctions de gestion des sockets                     **/
/******************************************************************/

/**** Constantes ****/

/**** Fonctions ****/
int initialisationServeur(char *service);
int boucleServeur(int s,void *(*traitement)(void *));
void *traitement(void *message);
int serveurMessages(int s,void *(*traitement)(void *));
int initialisationSocketUDP(char *service);

int lanceThread(void *(*thread)(void *), void *arg, int taille); // à enlever
