/**
 * Jeu Othello : configuration du jeu
 * @authors Beaugiraud Paul, Magnien geoffrey, Fourel romain
 */

/** Bibliotheques */
#include <stdlib.h>
#include <stdbool.h>

/** Types */

/**
 * Etat du jeu : en cours, gagnant noir ou blanc, égalité
 */
enum status_e {
    STATUS_RUNNING, STATUS_BLACK, STATUS_WHITE, STATUS_TIE
};

typedef enum status_e status_t;

/**
 * Type d'une case du plateau
 */
enum cell_e {
    CELL_FREE, CELL_BLACK, CELL_WHITE, CELL_SUG,CELL_OTHER};

typedef enum cell_e cell_t;

/**
 * Type de l'othellier
 * est une matrice des cellules
 */
typedef cell_t* board_t;

/**
 * Type nombre de points
 */
typedef unsigned int points_t;

/**
 * Configuration du jeu
 */
struct config_s {
    size_t board_size;     /* taille d'un cote de l'othellier */
    board_t board;         /* l'othellier */
    unsigned int round;    /* nombre de coups joues */
    points_t points_black; /* cases noires */
    points_t points_white; /* cases blanches */
};

typedef struct config_s config_t;

/** Fonctions plateau */
cell_t board_get(board_t b, size_t size, size_t i, size_t j);
void board_set(board_t b, size_t size, size_t i, size_t j, cell_t v);
void board_copy(board_t dst, board_t src, size_t size);
void board_print(board_t b, size_t size);

/** Fonctions configuration */
void config_init(config_t* config, size_t sz);
config_t* config_copy(config_t* config);
void config_free(config_t* config);
void config_print(config_t* config);
points_t config_get_points_black(config_t* config);
points_t config_get_points_white(config_t* config);
