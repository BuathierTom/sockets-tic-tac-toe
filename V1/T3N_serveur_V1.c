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

#define LG_MESSAGE 256 // Longueur du message reçu maximal+

char grille[3][3] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};

void marque(int, int, char);
void dessineGrille();
char checkWinner();

int main(int argc, char *argv[]){
    // Definition des variables pour le morpion basique sans rêgles

    // Définition des variables de Sockets 
	int socketEcoute;
    int lig, col; // position de la ligne et de la colone pour la grille
	
	int compteur; // compteur pour savoir quand la grille est rempli !!

	char winner; // Qui est le gagnant ?
	char envoiePosTab[100]; // message vers le client pour les position et le message
	char finPartie[100]; // message vers le client pour les cas de fin de partie

	int joue_choix, position; // choix de L'IA et la position de L'IA

	struct sockaddr_in pointDeRencontreLocal;
	socklen_t longueurAdresse;

	int socketDialogue; // oh !
	struct sockaddr_in pointDeRencontreDistant;
	char messageRecu[LG_MESSAGE]; /* le message de la couche Application ! */

	int lirePosTab, envoieNbr; // Pour lire et envoie le nombre

	int start; // Envoie le start au client

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

        // On envoie le start au client pour que le jeu commence
        switch(start = write(socketDialogue, "Start", strlen("Start"))) {
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
				  
				while(1){ // on boucle pour que le serveur prenne un nombre au hasard et l'envoie au client
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
						printf("Le joueur joue: %s\n\n", messageRecu);
						compteur += 1;
						// On regarde met a jour notre grille avec la position du coup du joueur
						sscanf(messageRecu, "%d", &position);

						lig = (position - 1) / 3;
						col = (position - 1) % 3;
						marque(lig, col, 'X');
						
						// On check si il y a un gagnant dans la grille 
						winner = checkWinner();
						// Si le winner est X alors il envoie que Xwin a gagner
						if (winner == 'X'){	
							sprintf(finPartie, "%s", "Xwin");
							switch(envoieNbr = write(socketDialogue, finPartie, strlen(finPartie))){
								case -1 : /* une erreur ! */
									perror("Erreur en écriture...");
									close(socketDialogue);
									exit(-3);
								case 0 : /* la socket est fermée */
									fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
									return 0;
								default: /* envoi de n octets */
									printf("Les %s !\n\n", finPartie);
									exit(-1);
							}
						// Si le compteur est a 9 donc la grille est rempli par X alors il envoie Xend
						}else if (compteur == 9){
							sprintf(finPartie, "%s", "Xend");
							switch(envoieNbr = write(socketDialogue, finPartie, strlen(finPartie))){
								case -1 : /* une erreur ! */
									perror("Erreur en écriture...");
									close(socketDialogue);
									exit(-3);
								case 0 : /* la socket est fermée */
									fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
									return 0;
								default: /* envoi de n octets */
									printf("Les %s !\n\n", finPartie);
									exit(-1);
							}
						}
						// Initialisation des valeurs pour les boucles
						bool confirmation = false;
						// Le serveur créé un nombre entre 1 et 9
						joue_choix = rand() % 9 + 1;
						lig = (joue_choix - 1) / 3;
						col = (joue_choix - 1) % 3;
						// J'ajoute un a compteur
						compteur += 1;
						// Verification que la case ne sois pas prise
						while (confirmation == false){
							if (grille[lig][col] == 'X' || grille[lig][col] == 'O'){
								printf("Changer de position\n");
								joue_choix = rand() % 9 + 1;
								// On recalcule la position 
								lig = (joue_choix - 1) / 3;
								col = (joue_choix - 1) % 3;

								compteur += 1;
							}else{
								// On confirme
								confirmation = true;
							}
						}
						// On marque avec les positions récuperées et on les marques dans la grille
						marque(lig, col, 'O');
						dessineGrille();
						// On recheck si il y a un winner ou pas
						winner = checkWinner();
						// Si le compteur est a 9 donc la grille est rempli par O alors il envoie Oend
						if (compteur == 9){
							sprintf(finPartie, "%s", "Oend");
							switch(envoieNbr = write(socketDialogue, finPartie, strlen(finPartie))){
								case -1 : /* une erreur ! */
									perror("Erreur en écriture...");
									close(socketDialogue);
									exit(-3);
								case 0 : /* la socket est fermée */
									fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
									return 0;
								default: /* envoi de n octets */
									printf("Les %s !\n\n", finPartie);
									exit(-1);
							}
						// Si le winner est O alors il envoie que Owin a gagner
						}else if (winner == 'O'){
							sprintf(envoiePosTab, "%s %d","Owin", joue_choix);
							switch(envoieNbr = write(socketDialogue, envoiePosTab, strlen(envoiePosTab))){
								case -1 : /* une erreur ! */
									perror("Erreur en écriture...");
									close(socketDialogue);
									exit(-3);
								case 0 : /* la socket est fermée */
									fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
									return 0;
								default: /* envoi de n octets */
									printf("Les O ont gagné en %s !\n\n", envoiePosTab);
									exit(-1);
							}
						// Sinon on continue et on renvoie la position avec le message
						}else{
							sprintf(envoiePosTab, "%s %d","continue", joue_choix);
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
        }
	}
	// On ferme la ressource avant de quitter
	close(socketDialogue);
	close(socketEcoute);
	return 0; 
}
	
// Fonction qui va permettre de remplacer la case choisi par la marque du joueur
void marque(int lig, int col, char marque){   
    // Prend la ligne et la colonne de la grille et remplace par la marque
    grille[lig][col] = marque;
}

// Fonction d'affichage de la carte
void dessineGrille(){
    // init de i et j qui vont nous servir pour les lignes et les colonnes
    int i, j;
    // Parcours la largeur 
    for (i = 0; i < 3; i++){
        // Parcours la hauteur
        for (j = 0; j < 3; j++){
            //Print des lignes
            printf(" %c ", grille[i][j]);
            if (j != 2)
                //Gestion des séparations en hauteur
                printf("|");
        }
        // Gestion des separations en largeur
        if (i != 2)
            printf("\n-----------\n");
    }
    printf("\n");
}

char checkWinner()
{
    // variables
    int i;
    // Vérifie les lignes
    for (i = 0; i < 3; i++)
        if (grille[i][0] == grille[i][1] && grille[i][0] == grille[i][2])
            return grille[i][0];
    // Vérifie les colonnes
    for (i = 0; i < 3; i++)
        if (grille[0][i] == grille[1][i] && grille[0][i] == grille[2][i])
            return grille[0][i];
    // Vérifie la diagonale de en haut à gauche à en bas à droite
    if (grille[0][0] == grille[1][1] && grille[0][0] == grille[2][2])
        return grille[0][0];
    // verification en diagonale de en haut à droite à en bas à gauche 
    if (grille[0][2] == grille[1][1] && grille[0][2] == grille[2][0])
        return grille[0][2];

    return ' ';
}