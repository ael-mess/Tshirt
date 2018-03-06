/** Fichier serveur.c **/

/***********************************************************/
/** Serveur pour le serveur                               **/
/***********************************************************/

/** Fichiers d'inclusion **/

/** Constantes **/

/** Variables publiques **/

/** Variables statiques **/

/** Procedure principale **/

int gestionClient(int s){
	/* Obtient une structure de fichier */
	FILE *dialogue=fdopen(s,"a+");
	if(dialogue==NULL){ perror("gestionClient.fdopen"); exit(EXIT_FAILURE); }

	/* Echo */
	//char ligne[6] = "couco\n";
	//while(fgets(ligne,6,dialogue)!=NULL)
    	fprintf(dialogue,"> coucou\n");

	/* Termine la connexion */
	fclose(dialogue);
	return 0;
}

int main(int argc,char *argv[])
{
	int s;
	 
	/* Lecture des arguments de la commande */
	//analyseArguments(argc,argv);
	char *service = argv[1];
    
	/* Initialisation du serveur */
	s=initialisationServeur(service);
	   
	/* Lancement de la boucle d'ecoute */
	boucleServeur(s,gestionClient);
}

