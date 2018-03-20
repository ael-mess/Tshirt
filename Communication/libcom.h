/** fichier libcom.h **/

/******************************************************************/
/** Ce fichier decrit les structures et les constantes utilisees **/
/** par les fonctions de gestion des sockets                     **/
/******************************************************************/

/**** Constantes ****/
typedef struct thread_struct
{
   void *(*thread)(void*);
   int arg;
} thread_struct;

/**** Fonctions ****/
int initialisationServeur(char *service);
int boucleServeur(thread_struct ecoute,int (*traitement)(void *(*)(void *), void *, int));
