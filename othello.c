/**
 * Jeu Othello : gestion de l'interface
 * @authors Beaugiraud Paul, Magnien geoffrey, Fourel romain
 */

/** Bibliotheques */
#include <stdio.h>
#include <stdbool.h>
#include "othello.h"
#include <unistd.h>


/** Fonctions */

/**
 * Affiche comment appeller le jeu
 */
void othello_usage() {
    printf("Usage: othello size [playerBlack [playerWhite]]\n");
    printf("avec\n");
    printf("playerBlack ou playerWhite soit [H/h]umain (defaut) soit [R/r]obot\n");
}

/**
 * Affiche un message donne en paramètre
 */
void othello_message(char* msg) {
    printf(">OTHELLO: %s\n", msg);
}

/**
 * Boucle du jeu avec le deux types de joueurs en argument.
 * 
 * @param config   configuration du jeu
 * @param playerB  type de joueur Noir
 * @param playerW  type de joueur Blanc
 * @return         etat de fin de jeu
 */
status_t othello_game_run(config_t* config,player_t playerB, player_t playerW, int robotb, int robotw) {
    status_t status = STATUS_RUNNING;
    while (status == STATUS_RUNNING) {
        config->round+=1;
        /* affiche la configuration du jeu */
        /*system("clear");*/
        config_print(config);
        /* coup du noir */
        move_t mB = move_init(MOVE_PASS_BLACK);
        
        while (1) {
            mB = play_black(config, playerB, robotb);
	    /* contrôler que le coup est correct */
            if (move_check(config, &mB) == false) {
                othello_message("Coup incorrect ! Rejouez.\n");
            } else {
                break;
            }
        }
        move_do(config, &mB);
        if (mB.kind==MOVE_PLAY_BLACK){config->points_black=config->points_black+1;}
        /* affiche la configuration du jeu */
        /*system("clear");*/
        config_print(config);

        /* coup du blanc */
        move_t mW = move_init(MOVE_PASS_WHITE);

        while (1) {
            mW = play_white(config, playerW, robotw);
            /* check move is correct */
            if (move_check(config, &mW) == false) {
                othello_message("Coup incorrect ! Rejouez.\n");
            } else {
                break;
            }
        }
        move_do(config, &mW);
        if (mW.kind==MOVE_PLAY_WHITE){config->points_white=config->points_white+1;}
        if(move_is_pass(&mW)==true && move_is_pass(&mB)==true){
            if (config->points_black>config->points_white){status=STATUS_BLACK;}
            else if(config->points_black<config->points_white){status=STATUS_WHITE;}
            else {status=STATUS_TIE;}
         }
        /* fin du jeu */
	/* si les deux coups sont PASS alors
	 *    le jeu termine après avoir 
	 *    afficher qui gagne (ou egalite) en fonction du score
         * sinon, continue
	 */
    }
    return status;
}

/**
 * Affiche information de fin de jeu
 */
void othello_end(config_t* game, status_t status) {
    othello_message("Le jeu est fini !");
    othello_message("Resultat: ");
    switch (status) {
        case STATUS_BLACK:
        {
            othello_message("Gagnant : Noir!");
            break;
        }
        case STATUS_WHITE:
        {
            othello_message("Gagnant: Blanc!");
            break;
        }
        case STATUS_TIE:
        {
            othello_message("Egalite!");
            break;
        }
        default:
        {
            othello_message("Fin anormale.");

            break;
        }
    }
}

/** Programme principal */
int main(int argc, char* argv[]) {
    size_t size=4;
    if (argc == 1) {
        othello_usage();
        othello_message("Taille 4 (defaut), deux joueurs humains");
    } 
    else if (atoi(argv[1])==4){
        othello_usage();
        othello_message("Taille 4, deux joueurs humains");
        size=4;
    }
    else if (atoi(argv[1])==8){
        othello_usage();
        othello_message("Taille 8, deux joueurs humains");
        size=8;
    }
    else if (atoi(argv[1])==16){
        othello_usage();
        othello_message("Taille 16, deux joueurs humains");
        size=16;
    }
    /* premier argument est la taille du jeu parmi 4, 8, 16 */
    /* taille jeu par defaut */
    /* lire la taille dans l'argument 1 */
    /* la taille doit être un entier parmi 4, 8 ou 16 */

    /* le reste des arguments est optionnel, pour choix des joueurs */
    /* initialisation des joueurs en fonctions de la ligne de commande */
    player_t playerB = PLAYER_HUMAN;
    player_t playerW = PLAYER_HUMAN;
    if (argc >= 3) {
        if (argv[2][0] == 'R' || argv[2][0] == 'r') {
            playerB = PLAYER_ROBOT;
            othello_message("Player Black is robot");
        } else {
            othello_message("Player Black is human");
        }
    }
    if (argc >= 4) {
        if (argv[3][0] == 'R' || argv[3][0] == 'r') {
            playerW = PLAYER_ROBOT;
            othello_message("Player White is robot");
        } else {
            othello_message("Player White is human");
        }
    }

    /* initialisation de la configuration du jeu */
    config_t game;
    config_init(&game, size);
    /* lancement du jeu */
    status_t status = othello_game_run(&game, playerB, playerW, 3, 5);

    /* affichage fin */
    othello_end(&game, status);

    /* fin */
    return 0;

}

/*
1-3 +
1-4 +
1-5 +

2-3 +
2-4 +
2-5 +

3 vs 4 = 3

5 vs 3 = 3
3 vs 5 = egalite

4 vs 5 = 5
5 vs 4 = 4
*/
