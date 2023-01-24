#include <stdio.h> // import de base
#include <stdlib.h> // import de base
#include <stdbool.h> // import pour la manupulation de booléen 
#include <string.h> // import pour la manipulation de chaîne de charactère
#include <unistd.h> // import de base

#define clear() printf("\033[H\033[J") //pour clear la console et faire un affichage plus propre

// déclaration de la grille 3x3 avec les numéros de chaques cases 
char grille[3][3] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
// variables
int choix, joueur;

// les fonctions en dessous du main
char checkWinner();
void dessineGrille();
void marque(int, int, char);

// debut du main
int main()
{   
    // déclaration du char winner que l'on unitialise en char vide
    char winner;
    // déclaration d'entier pour lignes et colonnes
    int lig, col;
    // joueur par défaut
    joueur = 1;

    do
    {
        clear();
        // appel de la fonction dessine grille pour voir l'avancement de la partie au fils du temps
        dessineGrille();
        
        if (joueur % 2) {
            joueur = 1;
        } else {
            joueur = 2;
        }

        // affiche un message au joueur 1 ou 2
        printf("Joueur %d, entrez votre choix : ", joueur);
        // récupération du choix
        scanf("%d", &choix);

        // pour savoir ou l'on joue 
        lig = (choix - 1) / 3;
        col = (choix - 1) % 3;

        // definir le char qui va etre placer en fonction du joueur qui vient de jouer
        char choixMarque;
        if (joueur == 1) {
            choixMarque = 'X';
        } else {
            choixMarque = 'O';
        }
        // appel la fonction marque qui va remplacer la case choisir par la marque du joueur
        marque(lig, col, choixMarque);
        
        // remplie ou non la condition de victoire
        winner = checkWinner();
        printf("%s", &winner);

        // joueur suivant
        joueur++;

    } 
    // temps qu'il n'y a pas de gagnant la partie continue
    while (winner == ' ');

    // si le joueur qui place les X a gagné
    if (winner == 'X')
        printf("Le joueur 1 a gagné\n");
    // si le joueur qui pas les O a gagné
    else if (winner == 'O')
        printf("Le joueur 2 a gagné\n");
    // sinon égalité
    else
        printf("Egalité\n");

    // pas le choix tu connais mais j'avais envie de tout mettre des commentaires partout :eyes: 
    return 0;
}

// fonction d'affichage de la carte
void dessineGrille()
{
    // init de i et j qui vont nous servir pour les lignes et les colonnes
    int i, j;

    // parcour la largeur 
    for (i = 0; i < 3; i++)
    {   
        // parcour la hauteur
        for (j = 0; j < 3; j++)
        {
            //print des ligne
            printf(" %c ", grille[i][j]);
            if (j != 2)
                //gestion des séparations en hauteur
                printf("|");
        }

        // gestion des separations en largeur
        if (i != 2)
            printf("\n-----------\n");
    }

    printf("\n");
}

// fonction qui va permettre de remplacer la case choisi par la marque du joueur
void marque(int lig, int col, char marque)
{
    if (grille[lig][col] == 'X' || grille[lig][col] == 'O')
    {   
        // si la case est deja remplie
        printf("Case déjà utilisée, veuillez réessayer\n");
        joueur--;
    }
    // sinon prend la ligne et la colonne de la grille et remplace par la marque
    else
        grille[lig][col] = marque;
}

// fonction de vérif d'une potentiel victoire
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