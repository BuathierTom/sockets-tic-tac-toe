#include <stdio.h> //import de base 
#include <stdlib.h> //import de base 
#include <string.h> // import pour la manipulation de chaîne de charactère
#include <unistd.h> // import de base 

// déclaration de la grille 3x3 avec les numéros de chaques cases 
char grille[3][3] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
// variables
int choix, joueur;

// les fonctions en dessous du main
void dessineGrille();
void marque(int, int, char);

// debut du main
int main()
{
    // déclaration du char winner que l'on unitialise en char vide
    char winner = ' ';
    // déclaration d'entier pour lignes et colonnes 
    int lig, col;
    // joueur par défaut
    joueur = 1;

    do{   
        // appel de la fonction dessine grille pour voir l'avancement de la partie au fils du temps
        dessineGrille();

        // if qui permet de savoir à qui est le tour de jouer
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

        // joueur suivant 
        joueur++;
    
    }

    // temps qu'il n'y a pas de gagnant la partie continue 
    while (winner == ' ');

    // pas le choix tu connais mais j'avais envie de tout mettre des commentaires partout :eyes: 
    return 0;
}

// fonction qui va permettre de remplacer la case choisi par la marque du joueur
void marque(int lig, int col, char marque)
{   
    // prend la ligne et la colonne de la grille et remplace par la marque
    grille[lig][col] = marque;
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
