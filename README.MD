# SAE SOCKETS
#### Quentin Gilleron
#### Tom Buathier
#
## Organisation des fichiers :  
* README.MD
* Org.pdf
* Fichier V0  
`- T3N_client_V0.c` (client)  
`- T3N_serveur_V0.c` (serveur)
* Fichier V1  
`- T3N_client_V1.c` (client)  
`- T3N_client_V1.c` (serveur)
#
## Comment lancer le programme dans le terminal ?

(exemple pour la V0)  
__Serveur :__  
gcc T3N_serveur_V0.c -o serveur  
./serveur  
 
__Client :__  
gcc T3N_client_V0.c -o client  
./client 127.0.0.1 5000

#
## V0

### Prérequis pour cette V0 :

* L'envoie du message __Start__ pour commencer une partie
* Affichage d'une grille vide
* Demander le numéro
* Afficher une grille vide
* Demander un numéro au client
* Envoyer la grille à jour au serveur avec un X
* Le serveur envoi un nombre aléatoire
* Le client le reçoit et l'affiche avec un O
* Et le client boucle

#
## V1

### Prérequis pour cette V1 :

* Reprendre la V0
* Ajout de vérification de fin de partie
* Envoie de message du serveur vers le client en fonction de la fin
* Si message de fin, l'afficher et fermer le programme
* Si aucun message de fin alors continue et boucle 

