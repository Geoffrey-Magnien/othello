/**
 * Jeu Othello : gestion de l'interface
 * @authors Beaugiraud Paul, Magnien geoffrey, Fourel romain
 */

/** Bibliotheques */
#include "player.h" /* gestion joueurs */

/** Fonctions */
void othello_usage();
void othello_message(char* msg);
status_t othello_game_run(config_t* c, player_t pB, player_t pW, int robotb, int robotw);

