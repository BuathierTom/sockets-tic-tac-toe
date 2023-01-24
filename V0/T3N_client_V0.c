#include <stdio.h>
#include <stdlib.h> /* pour exit */
#include <unistd.h> /* pour read, write, close, sleep */
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h> /* pour memset */
#include <netinet/in.h> /* pour struct sockaddr_in */
#include <arpa/inet.h> /* pour htons et inet_aton */

#define LG_MESSAGE 256 // Longueur du message reçu maximal

// Creation des variables pour le morpion
char grille[3][3] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
// variable pour stocker le choix et le numéro de joueur
int choix, joueur;

// déclaration des fonctions créés
void dessineGrille();
void marque(int, int, char);

int main(int argc, char *argv[]){
	/* Definition des variables de morpion */

	// déclaration du char winner que l'on initialise en char vide
    char winner = ' ';
    // déclaration d'entiers pour lignes et colonnes 
    int lig, col;

	int position, positionIA;
	char res;
    // joueur par défaut
    joueur = 1;
	
	char choixMarque;

    // Creation des variables pour les sockets
    int descripteurSocket;
	struct sockaddr_in sockaddrDistant;
	socklen_t longueurAdresse;

	char message[100]; // message stockant le message Start
	char posTab[100]; // message stockant le message pour la position du joueur
	char posTabIA[100]; // message stockant le message pour la position de l'IA
	int start, poster, reponseServ; // stockage des messages et de réponses

	int nb, lus; /* Etapes faites par le serveur lors de la lecture et l'ouverture*/

	char ip_dest[16]; // Ip donnée lors de l'appel du programme (ex: 127.0.0.1)
	int  port_dest; // Port donné lors de l'appel du programme (ex: 5000)

	if (argc>1) { // si il y a au moins 2 arguments passés en ligne de commande, récupération ip et port
		strncpy(ip_dest,argv[1],16);
		sscanf(argv[2],"%d",&port_dest);
	}
	else{
		printf("USAGE : %s ip port\n",argv[0]);
		exit(-1);
	}

	// Créer un socket de communication
	descripteurSocket = socket(AF_INET, SOCK_STREAM, 0);
	// Teste la valeur renvoyée par l’appel système socket()
	if(descripteurSocket < 0){
		perror("Erreur en création de la socket..."); // Affiche le message d’erreur
		exit(-1); // On sort en indiquant un code erreur
	}
	printf("Socket créée! (%d)\n", descripteurSocket);

	// Remplissage de sockaddrDistant (structure sockaddr_in identifiant la machine distante)
	// Obtient la longueur en octets de la structure sockaddr_in
	longueurAdresse = sizeof(sockaddrDistant);
	// Initialise à 0 la structure sockaddr_in
	memset(&sockaddrDistant, 0x00, longueurAdresse);
	// Renseigne la structure sockaddr_in avec les informations du serveur distant
	sockaddrDistant.sin_family = AF_INET;
	// On choisit le numéro de port d’écoute du serveur
	sockaddrDistant.sin_port = htons(port_dest);
	// On choisit l’adresse IPv4 du serveur
	inet_aton(ip_dest, &sockaddrDistant.sin_addr);

	// Débute la connexion vers le processus serveur distant
	if((connect(descripteurSocket, (struct sockaddr *)&sockaddrDistant,longueurAdresse)) == -1){
		perror("Erreur de connection avec le serveur distant...");
		close(descripteurSocket);
		exit(-2); // On sort en indiquant un code erreur
	}
	printf("Connexion au serveur %s:%d réussie!\n",ip_dest,port_dest);

 	// Envoi du message

        memset(message, 0x00, LG_MESSAGE*sizeof(char));

		// Lors du lancement de la session, le serveur envoie au client "Start", ce qui permet au client de savoir quand commencer.
		// Le read permet ici de recevoir le start du serveur et ensuite de commencer le jeu 
        start = read(descripteurSocket, message, LG_MESSAGE*sizeof(char));
		switch(start) {
            /* une erreur ! */ 
			case -1 : 
				  perror("read_start"); 
				  close(descripteurSocket); 
				  exit(-5);
            /* la socket est fermée */      
			case 0  : 
				  fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
   				  close(descripteurSocket);
   				  return 0;
            /* réception de n octets */
			default:  
				// message de depart
				printf("%s, le jeu peut commencer !\n\n", message);	
				while(1)
    			{
					memset(message, 0x00, LG_MESSAGE*sizeof(char));
					
					// affiche la grille
					dessineGrille();

					// récupération du choix de jeu
					printf("\n -> ");
					scanf("%d", &position);
					printf("\n\n");

					sprintf(posTab, "%d", position);
					// sscanf() pour le decodage
					
					// placement du coup joué 
					lig = (position - 1) / 3;
					col = (position - 1) % 3;

					// appel de la fonction qui place la marque
					marque(lig, col, 'X');
					// dessiner la grille 
					dessineGrille();
					switch(poster = write(descripteurSocket, posTab, strlen(posTab))){
						/* une erreur ! */
						case -1 : 
								perror("Erreur en écriture...");
								close(descripteurSocket);
								exit(-3);
						/* la socket est fermée */
						case 0 : 
							fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
							return 0;
						/* envoi de n octets */
						default: 
							printf("Message %s envoyé!\n\n", posTab);
						}
						
						switch(reponseServ = read(descripteurSocket, posTabIA, LG_MESSAGE*sizeof(char))) {
							/* une erreur ! */ 
							case -1 : 
								perror("read"); 
								close(descripteurSocket); 
								exit(-5);
							/* la socket est fermée */      
							case 0  : 
								fprintf(stderr, "La socket a été fermée par le serveur !\n\n");
								close(descripteurSocket);
								return 0;
							/* réception de n octets */
							default:  
								printf("Message reçu : %s (%d octets)\n\n", posTabIA, reponseServ);
							}

						// récupération de la chaine de char envoyé par le serveur
						sscanf(posTabIA, "%d", &positionIA);
						lig = (positionIA - 1) / 3;
						col = (positionIA - 1) % 3;

						// appel de la fonction pour placer la marque
						marque(lig, col, 'O');
				}
		    }
			// On ferme la ressource avant de quitter
			close(descripteurSocket);
			return 0;
    }
	

// Creation des fonctions pour les marques dans la grille et aussi l'affichage de la grille

// Fonction qui va permettre de remplacer la case choisi par la marque du joueur
void marque(int lig, int col, char marque)
{   
    // Prend la ligne et la colonne de la grille et remplace par la marque
    grille[lig][col] = marque;
}

// fonction d'affichage de la carte
void dessineGrille()
{
    // init de i et j qui vont nous servir pour les lignes et les colonnes
    int i, j;
    // Parcours la largeur 
    for (i = 0; i < 3; i++)
    {
        // Parcours la hauteur
        for (j = 0; j < 3; j++)
        {
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