/**
 * Jeu Othello : Les tests Cunit
 * @authors Beaugiraud Paul, Magnien geoffrey, Fourel romain
 */

/** Bibliotheques */
#include <stdio.h>
#include <stdbool.h>
#include "player.h"
#include <unistd.h>

#include "TestRun.h"
#include "CUnit.h"
#include "Basic.h"
//#include "CUnit/Automated.h"
//#include "CUnit/Console.h"

//execution :
//make
//gcc -Wall -fPIC -shared -O -g config.o player.o -o libhello.so
//gcc -Wall -I/home/secret/CUnitHome/include/CUnit test_unit.c -L/home/secret/CUnitHome/lib -lcunit ./libhello.so -o executable
//LD_LIBRARY_PATH=/home/secret/CUnitHome/lib ./executable

bool coup_valide = true;
bool passe = false;
bool fin_de_partie = false;
bool erreur_fin_partie = false;

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


/* Test Suite setup and cleanup functions: */

int init_suite(void) { return 0; }
int clean_suite(void) { return 0; }

/************* Test case functions ****************/
/*
{
  CU_ASSERT(CU_TRUE);
  CU_ASSERT_NOT_EQUAL(2, -1);

*/

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

void fin_partie(void){
    CU_ASSERT(!erreur_fin_partie);
}

void passe_tour(void){    /* les 2 joueurs sont des robots*/
    player_t playerBa = PLAYER_ROBOT;
    player_t playerWa = PLAYER_ROBOT;

    /* initialisation de la configuration du jeu */
    config_t gamea;
    config_init(&gamea, 8);
    /* lancement du jeu*/
    othello_game_run(&gamea, playerBa, playerWa, 2, 1);
    CU_ASSERT(passe);
}

void validation_coup(void){
    CU_ASSERT(coup_valide);
}

void testOptVsMin(void){
    /* les 2 joueurs sont des robots*/
    player_t playerB = PLAYER_ROBOT;
    player_t playerW = PLAYER_ROBOT;

    /* initialisation de la configuration du jeu */
    config_t game;
    config_init(&game, 8);
    /* lancement du jeu*/
    status_t status = othello_game_run(&game, playerB, playerW, 1, 3);
    CU_ASSERT( (othello_ending(&game, status)) == 2 );

    config_init(&game, 8);
    /* lancement du jeu */
    status = othello_game_run(&game, playerB, playerW, 3, 1);
    CU_ASSERT( (othello_ending(&game, status)) == 1 );
}

/************* Test Runner Code goes here **************/

int main ( void )
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if ( CUE_SUCCESS != CU_initialize_registry() )
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite( "max_test_suite", init_suite, clean_suite );
   if ( NULL == pSuite ) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ( (NULL == CU_add_test(pSuite, "Exigence 1:", fin_partie))
   || (NULL == CU_add_test(pSuite, "Exigence 2 : ", validation_coup))
   || (NULL == CU_add_test(pSuite, "Exigence 3 :", passe_tour))
   || (NULL == CU_add_test(pSuite, "Exigence ? (victoire de Min face a Opt):", testOptVsMin))
   || (NULL == CU_add_test(pSuite, "Exigence ? (victoire de Min face a Opt):", testOptVsMin))
   || (NULL == CU_add_test(pSuite, "Exigence ? (victoire de Min face a Opt):", testOptVsMin))
   )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   // Run all tests using the basic interface
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   printf("\n");
   CU_basic_show_failures(CU_get_failure_list());
   printf("\n\n");
/*
   // Run all tests using the automated interface
   CU_automated_run_tests();
   CU_list_tests_to_file();

   // Run all tests using the console interface
   CU_console_run_tests();
*/
   /* Clean up registry and return */

   CU_cleanup_registry();
   return CU_get_error();
}