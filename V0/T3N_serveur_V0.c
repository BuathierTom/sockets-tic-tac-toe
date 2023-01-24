#include <stdio.h> 
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#define PORT IPPORT_USERRESERVED // = 5000 (ports >= 5000 réservés pour usage explicite)

#define LG_MESSAGE 256 // Longueur du message reçu maximal

int main(int argc, char *argv[]){
    // Definition des variables pour le morpion basique sans rêgles

    // Définition des variables de Sockets 
	int socketEcoute;

	int joue_choix, position; // choix de L'IA et la position de L'IA

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	int socketDialogue; // oh !
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */

	int lirePosTab, envoieNbr; // lecture de la position du tableau et envoie du nombre
	char envoiePosTab[100];

	int ecrits, lus, nb; /* nb d’octets ecrits et lus */
	int retour, start; 

	// Crée un socket de communication
	socketEcoute = socket(PF_INET, SOCK_STREAM, 0); 
	// Teste la valeur renvoyée par l’appel système socket() 
	if(socketEcoute < 0){
		perror("Socket"); // Affiche le message d’erreur 
	exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée avec succès ! (%d)\n", socketEcoute); // On prépare l’adresse d’attachement locale

	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant le point d'écoute local)
	longueurAdresse = sizeof(pointDeRencontreLocal);
	// memset sert à faire une copie d'un octet n fois à partir d'une adresse mémoire donnée
	// ici l'octet 0 est recopié longueurAdresse fois à partir de l'adresse &pointDeRencontreLocal
	memset(&pointDeRencontreLocal, 0x00, longueurAdresse); pointDeRencontreLocal.sin_family = PF_INET;
	pointDeRencontreLocal.sin_addr.s_addr = htonl(INADDR_ANY); // attaché à toutes les interfaces locales disponibles
	pointDeRencontreLocal.sin_port = htons(PORT); // = 5000 ou plus
	
	// On demande l’attachement local de la socket
	if((bind(socketEcoute, (struct sockaddr *)&pointDeRencontreLocal, longueurAdresse)) < 0) {
		perror("bind");
		exit(-2);
	}
	printf("Socket attachée avec succès !\n");

	// On fixe la taille de la file d’attente à 5 (pour les demandes de connexion non encore traitées)
	if(listen(socketEcoute, 5) < 0){
   		perror("listen");
   		exit(-3);
	}
	printf("Socket placée en écoute passive ...\n");
	
	// boucle d’attente de connexion : en théorie, un serveur attend indéfiniment ! 
	
		memset(messageRecu, 0x00, LG_MESSAGE*sizeof(char));
		printf("Attente d’une demande de connexion (quitter avec Ctrl-C)\n\n");
		
		// c’est un appel bloquant
		socketDialogue = accept(socketEcoute, (struct sockaddr *)&pointDeRencontreDistant, &longueurAdresse);
		if (socketDialogue < 0) {
   			perror("accept");
			close(socketDialogue);
   			close(socketEcoute);
   			exit(-4);
		}

        // On renvoie la grille au client
	    start = write(socketDialogue, "Start", strlen("Start"));
        switch(start) {
			case -1 : /* une erreur ! */ 
				  perror("read"); 
				  close(socketDialogue); 
				  exit(-5);
			case 0  : /* la socket est fermée */
				  fprintf(stderr, "La socket a été fermée par le client !\n\n");
   				  close(socketDialogue);
   				  return 0;
			default:  /* réception de n octets */
				  printf("%s\n\n", "Start");
				  
				while(1){

				switch(lirePosTab = read(socketDialogue, messageRecu, LG_MESSAGE*sizeof(char))) {
					case -1 : /* une erreur ! */ 
						  perror("read"); 
						  close(socketDialogue); 
						  exit(-5);
					case 0  : /* la socket est fermée */
						  fprintf(stderr, "La socket a été fermée par le client !\n\n");
						  close(socketDialogue);
						  return 0;
					default:  /* réception de n octets */
						  printf("Message reçu : %s (%d octets)\n\n", messageRecu, lirePosTab);
					
				}

				// récupération du message
				sscanf(messageRecu, "%d", &position);
				
				// generation d'un nombre aléatoire 
				joue_choix = rand() % 9 + 1;

				// envoie du message de la position choisie
				sprintf(envoiePosTab, "%d", joue_choix);

				// On renvoie la grille au client
				switch(envoieNbr = write(socketDialogue, envoiePosTab, strlen(envoiePosTab))){
					case -1 : /* une erreur ! */
								perror("Erreur en écriture...");
								close(socketDialogue);
								exit(-3);
					case 0 : /* la socket est fermée */
							fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
							return 0;
					default: /* envoi de n octets */
							printf("Je joue en %s !\n\n", envoiePosTab);
			}
        }
	}
	// On ferme la ressource avant de quitter
	close(socketDialogue);
	close(socketEcoute);
	return 0; 
}
	


