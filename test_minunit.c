/**
 * Jeu Othello : Les tests Cunit
 * @authors Beaugiraud Paul, Magnien geoffrey, Fourel romain
 */

/** Bibliotheques */
#include <stdio.h>
#include <stdbool.h>
#include "player.h"
#include <unistd.h>
#include "minunit.h"
#include <time.h>

//execution :
//gcc -Wall -std=c11 -c -o config.o config.c
//gcc -Wall -std=c11 -c -o player.o player.c
//gcc -Wall -std=c11 -c -o test_minunit.o test_minunit.c
//gcc -Wall -std=c11 -o test_minunit config.o player.o test_minunit.o

//#####################################################################################################################
//#
//#
//###################################### VARIABLES Importantes globales de test #######################################

bool coup_valide = true;
bool passe = false;
bool fin_de_partie = false;
bool erreur_fin_partie = false;


//#####################################################################################################################
//#
//#
//###################################### Fonctions générales de fonctionnement  #######################################


/**
 * Boucle du jeu avec le deux types de joueurs en argument.
 * 
 * @param config   configuration du jeu
 * @param playerB  type de joueur Noir
 * @param playerW  type de joueur Blanc
 * @return         etat de fin de jeu
 */

status_t othello_game_run(config_t* config,player_t playerB, player_t playerW, int robotb, int robotw) {
    fin_de_partie = false;
    status_t status = STATUS_RUNNING;
    while (status == STATUS_RUNNING) {
        config->round+=1;
        /* affiche la configuration du jeu */
        /*system("clear");*/
        /* coup du noir */
        move_t mB = move_init(MOVE_PASS_BLACK);
        
        while (1) {
            mB = play_black(config, playerB, robotb);
	    /* contrôler que le coup est correct */
            if (move_check(config, &mB) == false) {
                // Une erreur est generee par un bot
                coup_valide = false;
            }
            else {
                break;
            }
        }
        move_do(config, &mB);
        if (mB.kind==MOVE_PLAY_BLACK){config->points_black=config->points_black+1;}
        /* affiche la configuration du jeu */
        /*system("clear");*/

        /* coup du blanc */
        move_t mW = move_init(MOVE_PASS_WHITE);

        while (1) {
            mW = play_white(config, playerW, robotw);
            /* check move is correct */
            if (move_check(config, &mW) == false) {
                // Une erreur est generee par un bot
                coup_valide = false;
            }
            else {
                break;
            }
        }
        move_do(config, &mW);
        if (mW.kind==MOVE_PLAY_WHITE){config->points_white=config->points_white+1;}
        if( (move_is_pass(&mW)==false) != (move_is_pass(&mB)==false) ){
            //si l un des bots passe son tour dans le cas ou il ne peut pas jouer, mais pas l'autre.
            passe = true;
        }
        if(move_is_pass(&mW)==true && move_is_pass(&mB)==true){
            fin_de_partie = true;
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
    if(fin_de_partie == false){
        erreur_fin_partie = true;
    }
    return status;
}


int othello_ending(config_t* game, status_t status) {
    switch (status) {
        case STATUS_BLACK:
        {
            /*Gagnant : Noir!*/
            return 1;
            break;
        }
        case STATUS_WHITE:
        {
            /*Gagnant : Blanc!*/
            return 2;
            break;
        }
        case STATUS_TIE:
        {
            /*Egalite*/
            return 3;
            break;
        }
        default:
        {
            /*Fin anormale*/
            return 0;
            break;
        }
    }
}

int victory_percent(int robotType) {
    int count = 0;
    player_t playerB = PLAYER_ROBOT;
    player_t playerW = PLAYER_ROBOT;

    /* initialisation de la configuration du jeu */
    config_t game;
    status_t status;
    for (int loop = 0; loop < 1000; loop++){
        config_init(&game, 8);
        /* lancement du jeu*/
        status = othello_game_run(&game, playerB, playerW, robotType, 6);
	    if((othello_ending(&game, status)) == 1){
            count += 1;
        }
    }
    return(count/10);
}

//#####################################################################################################################
//#
//#
//###################################### Fonctions de test unitaires ##################################################

MU_TEST(fin_partie) {
	mu_check(!erreur_fin_partie);
}

MU_TEST(passe_tour) {
    player_t playerBa = PLAYER_ROBOT;
    player_t playerWa = PLAYER_ROBOT;

    /* initialisation de la configuration du jeu */
    config_t gamea;
    config_init(&gamea, 8);
    /* lancement du jeu*/
    othello_game_run(&gamea, playerBa, playerWa, 2, 1);
	mu_check(passe);
}

MU_TEST(validation_coup) {
	mu_check(coup_valide);
}

MU_TEST(VictoryRate_Opt) {
	mu_check(victory_percent(1) > 70);
}
MU_TEST(VictoryRate_Corner) {
	mu_check(victory_percent(2) > 70);
}
MU_TEST(VictoryRate_Minscore) {
	mu_check(victory_percent(3) > 90);
}
MU_TEST(VictoryRate_Surpuissant) {
	mu_check(victory_percent(4) > 90);
}
MU_TEST(VictoryRate_SurpuissantV2) {
	mu_check(victory_percent(5) > 90);
}

MU_TEST(testOptVsMin) {
    /* les 2 joueurs sont des robots*/
    player_t playerB = PLAYER_ROBOT;
    player_t playerW = PLAYER_ROBOT;

    /* initialisation de la configuration du jeu */
    config_t game;
    config_init(&game, 8);
    /* lancement du jeu*/
    status_t status = othello_game_run(&game, playerB, playerW, 1, 3);
	mu_check( (othello_ending(&game, status)) == 2 );

    config_init(&game, 8);
    /* lancement du jeu */
    status = othello_game_run(&game, playerB, playerW, 3, 1);
	mu_check( (othello_ending(&game, status)) == 1 );
}

MU_TEST(view) {
    printf("\nopt(1) : %d, %d, %d, %d, %d \n",victory_percent(1));
    printf("\ncorner(2) : %d, %d, %d, %d, %d \n",victory_percent(2));
    printf("\nMinscore(3) : %d, %d, %d, %d, %d \n",victory_percent(3));
    printf("\nSurpuissant(4) : %d, %d, %d, %d, %d \n",victory_percent(4));
    printf("\nsurpV2(5) : %d \n",victory_percent(5));
	mu_check(1==2);
}

//#####################################################################################################################
//#
//#
//###################################### La suite d'execution des tests ###############################################

MU_TEST_SUITE(test_suite) {
    /* add the tests to the suite */

    //exigence 1
    MU_RUN_TEST(fin_partie);

    //exigence 2
    MU_RUN_TEST(passe_tour);

    //exigence 3
    MU_RUN_TEST(validation_coup);

    //exigence 5
    MU_RUN_TEST(VictoryRate_Opt);
    
    //exigence 6
    MU_RUN_TEST(VictoryRate_Corner);

    //exigence 7
    MU_RUN_TEST(VictoryRate_Minscore);
    
    //exigence 8
    MU_RUN_TEST(VictoryRate_Surpuissant);
    
    //exigence 9
    MU_RUN_TEST(VictoryRate_SurpuissantV2);

    //exigence 10
    MU_RUN_TEST(testOptVsMin);


    MU_RUN_TEST(view);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
	MU_RUN_SUITE(test_suite);
	MU_REPORT();
	return MU_EXIT_CODE;
}