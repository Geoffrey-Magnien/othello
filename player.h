/**
 * Jeu Othello : joueurs du jeu
 * @authors Beaugiraud Paul, Magnien geoffrey, Fourel romain
 */

/** Bibliotheques */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "config.h"

/** Types: A LIRE Semaine 1 */

/**
 * Type de joueur : humain ou robot
 */
enum player_e {
    PLAYER_HUMAN, PLAYER_ROBOT
};

typedef enum player_e player_t;

/**
 * Coup de jeu
 */
enum move_kind_e {
    MOVE_PASS_BLACK, MOVE_PLAY_BLACK, MOVE_PASS_WHITE, MOVE_PLAY_WHITE
};

typedef enum move_kind_e move_kind_t;

struct move_s {
    move_kind_t kind;
    size_t lin;
    size_t col;
};

typedef struct move_s move_t;

/** Fonctions coups */
move_t move_init(move_kind_t kind);
bool   move_is_pass(move_t* m);
bool   move_check(config_t* config, move_t* m);
void   move_do(config_t* config, move_t* m);
void   move_print(move_t* m);

/** Fonctions joueurs */
move_t play_black(config_t* game, player_t pB, int numero);
move_t play_white(config_t* game, player_t pB, int numero);
