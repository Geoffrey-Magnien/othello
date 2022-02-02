/**
 * Jeu Othello : configuration du jeu
 * @authors Beaugiraud Paul, Magnien geoffrey, Fourel romain
 */

/** Bibliotheques */
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Fonctions othellier */

/**
 * Affiche contenu d'une cellule
 * @param c cellule
 */
void cell_print(cell_t c) {
    if (c==CELL_FREE){printf("|   ");}
    else if (c==CELL_BLACK){printf("| # ");}
    else if (c==CELL_WHITE){printf("| O ");}
    else if (c==CELL_SUG){printf("| ? ");}
        
}

/**
 * Renvoie la cellule e la ligne i et colonne j
 * @param b  othellier
 * @param size taille de cote
 * @param i  numero ligne
 * @param j  numero colonne
 * @return   contenu othellier ligne i et colonne j
 */
cell_t board_get(board_t b, size_t size, size_t i, size_t j) {
    if (i < size && j < size) {
        return b[i * size + j];
    }
    return CELL_OTHER;
}

/**
 * Change la cellule e la ligne i et colonne j
 * @param b  othellier
 * @param size taille de cote
 * @param i  numero ligne
 * @param j  numero colonne
 * @param v  valeur a utiliser
 * @return   contenu othellier ligne i et colonne j
 */
void board_set(board_t b, size_t size, size_t i, size_t j, cell_t v) {
    if (i < size && j < size) {
        b[i * size + j] = v;
    }
}

/**
 * Copie le contenu de la source src vers la destination dst
 * @param dst  adresse destination
 * @param src  adresse source
 */
void board_copy(board_t dst, board_t src, size_t size) {
    /* copie sur size * size * sizeof(cell_t) */
    memcpy(dst, src, size * size * sizeof (cell_t));
}

/**
 * Affiche bordure haute avec les lettres des colonnes
 * Par exemple, pour size = 4
 *      A   B   C   D   
 * @param size   taille d'une cote
 */
void board_print_cols_number(size_t size) {
    if (size==4){printf("    A   B   C   D");}
    else if (size==8){printf("    A   B   C   D   E   F   G   H");}
    else if (size==16){printf("    A   B   C   D   E   F   G   H   I   J   K   L   M   N   O   P");}
}

/**
 * Affiche bordure entre lignes
 * Par exemple, pour size = 4
 *  +---+---+---+---+
 * @param size   taille d'une cote
 */
void board_print_line_border(size_t size) {
    printf("  ");
    for (int i=0;i<size;i++){
        printf("+---");
    }
    printf("+");
}

/**
 * Affiche la ligne i de l'othellier b de cote size
 * Par exemple, pour size = 4 et centre de l'othellier
 *  2|   | O | # |   |
 * @param b      la matrice
 * @param size   taille d'une cote
 * @param i      la ligne
 */
void board_print_line(board_t b, size_t size, size_t i) {
    if (i+1<10){printf("%ld ",i+1);}
    else {printf("%ld",i+1);}
    for(int j=0;j<size;j++){
        cell_print(board_get(b,size,i,j));
    }
}

/**
 * Affichage formate de l'othellier
 * @param b    la matrice
 * @param size taille d'un cote
 */
void board_print(board_t b, size_t size) {
    board_print_cols_number(size);
    printf("\n");
    board_print_line_border(size);
    printf("\n");
    for (size_t i = 0; i < size; i++) {
        board_print_line(b, size, i);
        printf("|\n");
        board_print_line_border(size);
        printf("\n");
    }
}

/** Fonctions configuration */

/**
 * Initialise l'etat du jeu
 * A LIRE Semaine 1
 * @param config  configuration a initialiser
 * @param sz      taille du plateau
 */
void config_init(config_t* config, size_t sz) {
    /* allocation du plateau */
    config->board_size = (sz == 4 || sz == 8 || sz == 16) ? sz : 4;
    config->board = (cell_t*) malloc(config->board_size *config->board_size *sizeof (cell_t));
    /* initialisation partout libre */
    for (int i=0;i<sz;i++){
        for (int j=0;j<sz;j++){
            /*plateau[i][j]*/board_set(config->board,sz,i,j,CELL_FREE);
        }
    }
    /* centre occupe */
    int centre = sz/2-1;
    board_set(config->board,sz,centre,centre,CELL_WHITE);
    board_set(config->board,sz,centre,centre+1,CELL_BLACK);
    board_set(config->board,sz,centre+1,centre,CELL_BLACK);
    board_set(config->board,sz,centre+1,centre+1,CELL_WHITE);
    /* tour de jeu et points */
    config->round = 0;
    config->points_black = 2;
    config->points_white = 2;
}

/**
 * Alloue une copie de Affiche l'etat du jeu
 * @param config  a copier
 * @return        nouvelle configuration
 */
config_t* config_copy(config_t* config) {
    if (config == NULL)
        return NULL;
    config_t* copy = (config_t*) malloc(sizeof(config_t));
    config_init(copy, config->board_size); /* alloue le board */
    /* copier le plateau */
    memcpy(copy->board, config->board, 
            config->board_size * config->board_size * sizeof (cell_t));
    /* et le score */
    copy->points_black = config->points_black;
    copy->points_white = config->points_white;
    return copy;
}

/**
 * Libere la mémoire occupee par la configuration
 * @param config a liberer
 */
void config_free(config_t* config) {
    if (config == NULL)
        return;
    /* libere le board */
    free (config->board);
    free (config);
}

/**
 * Affiche l'etat du jeu
 * A LIRE Semaine 1
 * @param config  configuration du jeu
 */
void config_print(config_t* config) {
    /* affiche le plateau du jeu */
    board_print(config->board, config->board_size);
    printf("Tour du jeu  : %d\n", config->round);
    printf("Points Noir  : %d\n", config_get_points_black(config));
    printf("Points Blanc : %d\n", config_get_points_white(config));
}

/**
 * Nombre de points du joueur Noir
 * A LIRE Semaine 1
 * @param config  configuration du jeu
 */
inline points_t config_get_points_black(config_t* config) {
    return config->points_black;
}

/**
 * Nombre de points du joueur Blanc
 * A LIRE Semaine 1
 * @param config  configuration du jeu
 */
inline points_t config_get_points_white(config_t* config) {
    return config->points_white;
}

