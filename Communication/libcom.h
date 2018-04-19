/** fichier libcom.h **/

/******************************************************************/
/** Ce fichier decrit les structures et les constantes utilisees **/
/** par les fonctions de gestion des sockets                     **/
/******************************************************************/

/**** Constantes ****/

/**** Fonctions ****/
int initialisationServeur(char *service);
int boucleServeur(int s,void *(*traitement)(void *));
void *traitementUDP(void *message);
int serveurMessages(int s,void *(*traitement)(void *));
int initialisationSocketUDP(char *service);
int envoiMessage(char * service, char *message, int taille);
int envoiMessageUnicast(char * service, char * machine, char *message, int taille);

int lanceThread(void *(*thread)(void *), void *arg, int taille); // à enlever
