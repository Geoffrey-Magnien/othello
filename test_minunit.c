/**
 * Jeu Othello : Les tests Cunit
 * @authors Beaugiraud Paul, Magnien geoffrey, Fourel romain
 */

/** Bibliotheques */
#include <stdio.h>
#include <stdbool.h>
#include "player.h"
#include <unistd.h>
#include <time.h>
#include "minunit.h"

//execution :
//gcc -Wall -std=c11 -c -o config.o config.c
//gcc -Wall -std=c11 -c -o player.o player.c
//gcc -Wall -std=c11 -c -o test_minunit.o test_minunit.c
//gcc -Wall -std=c11 -o test_minunit config.o player.o test_minunit.o
//.\test_minunit

//#####################################################################################################################
//#
//#
//###################################### VARIABLES Importantes globales de test #######################################

bool coup_valide = true;
bool passe = false;
bool fin_de_partie = false;
bool erreur_fin_partie = false;
size_t nombre_de_point_noir = 0;

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
            nombre_de_point_noir = config->points_black;
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

size_t victory_percent(int robotType, size_t maxloop) {
    size_t count = 0;
    player_t playerB = PLAYER_ROBOT;
    player_t playerW = PLAYER_ROBOT;

    /* initialisation de la configuration du jeu */
    config_t game;
    status_t status;
    for (size_t loop = 0; loop < maxloop; loop++){
        config_init(&game, 8);
        status = othello_game_run(&game, playerB, playerW, robotType, 6);
	    if((othello_ending(&game, status)) == 1){
            count += 1;
        }
    }
    return(100*count/maxloop);
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

MU_TEST(bot_rand_test) {
    player_t playerB = PLAYER_ROBOT;
    player_t playerW = PLAYER_ROBOT;
    config_t game;
    size_t old_nombre_pts = 0;
    mu_check(nombre_de_point_noir>0);
    while (true){
        config_init(&game, 8);
        othello_game_run(&game, playerB, playerW, 3, 6);
        old_nombre_pts = nombre_de_point_noir;
        config_init(&game, 8);
        othello_game_run(&game, playerB, playerW, 3, 6);
        if (old_nombre_pts != nombre_de_point_noir){
            break;
        }
    }
}

/*
MU_TEST(bot_rand_test) {
    player_t playerB = PLAYER_ROBOT;
    player_t playerW = PLAYER_ROBOT;
    config_t game;
    int old_nombre_pts;
    //while (true){
        othello_game_run(&game, playerB, playerW, 6, 6);
        old_nombre_pts = nombre_de_point_noir;
        othello_game_run(&game, playerB, playerW, 6, 6);
        printf("\ngame old:%d, now :%d \n",old_nombre_pts,nombre_de_point_noir);
        if (old_nombre_pts != nombre_de_point_noir){
            printf("hihi");
            //break;
        }
    //}
	mu_check(old_nombre_pts != nombre_de_point_noir);
}
*/
MU_TEST(VictoryRate_Opt) {
    size_t rate = victory_percent(1,200);
    printf("\n  %d/100 victory; needed:55\n",rate);
	mu_check(rate >= 55);
}

MU_TEST(VictoryRate_Corner) {
    size_t rate = victory_percent(2,200);
    printf("\n  %d/100 victory; needed:45\n",rate);
	mu_check(rate >= 45);
}

MU_TEST(VictoryRate_Minscore) {
    size_t rate = victory_percent(3,200);
    printf("\n  %d/100 victory; needed:85\n",rate);
	mu_check(rate >= 85);
}

MU_TEST(VictoryRate_Surpuissant) {
    size_t rate = victory_percent(4,200);
    printf("\n  %d/100 victory; needed:75\n",rate);
	mu_check(rate >= 75);
}

MU_TEST(VictoryRate_Minmax) {
    size_t rate = victory_percent(7,200);
    printf("\n  %d/100 victory; needed:80\n",rate);
	mu_check(rate >= 80);
}
/*
MU_TEST(VictoryRate_Surpuissantbis) {
    int rate = victory_percent(4);
    printf("\nSurpuissantbis(4) : %d/100 victory\n",rate);
	mu_check(rate > 75);
}
*/
/*
MU_TEST(VictoryRate_SurpuissanV2) {
    int rate = victory_percent(5,20);
    printf("\nSurpuissantV2(5) : %d/100 victory\n",rate);
	mu_check(rate> 70);
}
*/

MU_TEST(VictoryRate_SurpuissanV2) {
    player_t playerB = PLAYER_ROBOT;
    player_t playerW = PLAYER_ROBOT;

    config_t game;
    status_t status;
    size_t nb = 0;
    for (size_t i = 0; i < 20; i++){
        config_init(&game, 8);
        status = othello_game_run(&game, playerB, playerW, 5, 6);
	    if( (othello_ending(&game, status)) == 1 ){
            nb = nb + 1;
        }
    }
    printf("\n  %d/100 victory; needed:80\n",nb*5);
    mu_check(nb >= 16);
}


MU_TEST(testOptVsMinscore) {
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

/*
MU_TEST(testMinscoreVsSurpuissant) {
    player_t playerB = PLAYER_ROBOT;
    player_t playerW = PLAYER_ROBOT;

    config_t game;
    config_init(&game, 8);
    status_t status;
    int victory_min = 0;
    for (int i = 0; i < 3; i++){
        status = othello_game_run(&game, playerB, playerW, 4, 3);
	    if( (othello_ending(&game, status)) == 2 ){
            victory_min += 1;
        }
        status = othello_game_run(&game, playerB, playerW, 3, 4);
	    if( (othello_ending(&game, status)) == 1 ){
            victory_min += 1;
        }
    }
    printf("\nVS %d\n",victory_min);
	mu_check( victory_min > 3 );
}
*/

//#####################################################################################################################
//#
//#
//###################################### La suite d'execution des tests ###############################################

MU_TEST_SUITE(test_suite) {
    //srand(time(NULL));
    /* add the tests to the suite */

    //exigence 1
    printf("\nExigence 1 : Fin partie");
    MU_RUN_TEST(fin_partie);

    //exigence 2
    printf("\nExigence 2 : passe_tour");
    MU_RUN_TEST(passe_tour);

    //exigence 3
    printf("\nExigence 3 : validation_coup");
    MU_RUN_TEST(validation_coup);

    //exigence 4
    printf("\nExigence 4 : bot_rand_test.\n");
    MU_RUN_TEST(bot_rand_test);

    //exigence 5
    printf("\nExigence 5 : VictoryRate_Opt:");
    MU_RUN_TEST(VictoryRate_Opt);

    //exigence 6
    printf("\nExigence 6 : VictoryRate_Corner:");
    MU_RUN_TEST(VictoryRate_Corner);
    
    //exigence 7
    printf("\nExigence 7 : VictoryRate_Minscore:");
    MU_RUN_TEST(VictoryRate_Minscore);

    //exigence 8
    printf("\nExigence 8 : VictoryRate_Surpuissant:");
    MU_RUN_TEST(VictoryRate_Surpuissant);

    //exigence 9
    printf("\nExigence 9 : VictoryRate_SurpuissanV2:");
    MU_RUN_TEST(VictoryRate_SurpuissanV2);

    //exigence 10
    printf("\nExigence 10 : VictoryRate_Minmax");
    MU_RUN_TEST(VictoryRate_Minmax);

    //exigence 11
    printf("\nExigence 11 : testOptVsMinscore\n");
    MU_RUN_TEST(testOptVsMinscore);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
	MU_RUN_SUITE(test_suite);
    MU_REPORT();
	return MU_EXIT_CODE;
}