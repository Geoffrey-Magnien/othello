/**
 * Jeu Othello : joueurs du jeu
 * @authors Beaugiraud Paul, Magnien geoffrey, Fourel romain
 */

/** Bibliotheques */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "player.h"

/* Fonctions coups */

/**
 * Renvoie une valeur de type kind
 * @param kind   type de coup
 * @return       le coup de type avec lin et col a 0
 */
move_t move_init(move_kind_t kind) {
    move_t r = {.kind = kind, .lin = 0, .col = 0};
    return r;
}

move_t move_init2(move_kind_t kind) {
    move_t r = {.kind = kind, .lin = -1, .col = -1};    
    return r;
}

/**
 * Test si un coup est attente
 * @param m coup
 * @return  true si MOVE_PASS
 */
bool move_is_pass(move_t* m) {
    if (m->kind==MOVE_PASS_BLACK){return true;}
    else if (m->kind==MOVE_PASS_WHITE){return true;}
    /* m doit etre soit de type MOVE_PASS_BLACK soit MOVE_PASS_WHITE */
    return false;
}

/**
 * Controle que le coup PLAY (MOVE_PLAY_BLACK ou MOVE_PLAY_WHITE) est correct
 * 
 * @param config  configuration actuelle du jeu
 * @param lin     ligne du coup
 * @param col     colonne du coup
 * @param c       jeton joue
 * @param dLin    direction sur ligne
 * @param dCol    direction sur colonne
 * @return        true si correct, false sinon
 */

/*fonction renvoyant l'opposé d'une case noire ou blanche*/
cell_t oppose(cell_t jeton) {
    if (jeton==CELL_WHITE) {
        return CELL_BLACK;
    }
    if (jeton==CELL_BLACK) {
        return CELL_WHITE;
    }
    return CELL_OTHER;
}

bool move_check_play(config_t* config,size_t lin, size_t col, cell_t c, int dLin, int dCol) {
    board_t plateau = config->board;
    size_t taille = config->board_size;  
    cell_t jeton_voisin = board_get(plateau,taille,lin+dLin,col+dCol);

    if (board_get(plateau,taille,lin,col)!= CELL_BLACK && board_get(plateau,taille,lin,col)!=CELL_WHITE) {

        if (jeton_voisin==c) {
            return false;
        }
        if (jeton_voisin==oppose(c)) {
            for (int i=1; jeton_voisin==oppose(c); i++) {
                jeton_voisin = board_get(plateau,taille,lin+i*dLin,col+i*dCol);
            }
            if (jeton_voisin==c) {
                return true;
            }
        }

        if (jeton_voisin==CELL_FREE) {
            return false;
        }

    }       
    /* si le jeton voisin dans la direction (dLin, dCol) est de type
     *    oppose à c jusqu'a un jeton de type c,
     * alors true
     * sinon (debordement, pas de voisin de type oppose a c, etc)
     *       false
     */
    return false;
}

/**
 * Controle un coup PASS (MOVE_PASS_BLACK soit MOVE_PASS_WHITE) est correct
 * 
 * @param config  configuration actuelle du jeu
 * @param m       coup joue
 * @return        true si correct, false sinon
 */
bool move_check_pass(config_t* config, move_kind_t kind) {
    size_t size=config->board_size;
    cell_t c =(kind == MOVE_PASS_BLACK) ? CELL_BLACK : CELL_WHITE;
    for(size_t i=0;i<size;i++){
        for(size_t j=0;j<size;j++){
            if (config->board[i*config->board_size+j]==CELL_FREE){
                for (int dL = -1; dL <= 1; dL++){
                    for (int dC = -1; dC <= 1; dC++){
                        if (!(dL == 0 && dC == 0)) {
                            if(move_check_play(config,i,j,c,dC,dL)==true){ return false;}
                        }
                    }
                }
            }
        }
    }
    /* parcours de TOUTES les cases du plateau 
     *   si une case libre alors
     *      si le joueur donné par kind peut la jouer alors
     *        renvoie false
     * renvoie true
     */
    return true;
}


/**
 * Controle que le coup est correct
 * 
 * @param config  configuration actuelle du jeu
 * @param m       coup joue
 * @return        true si correct, false sinon
 */
bool move_check(config_t* config, move_t* m) {
    if (config == NULL || m == NULL)
        return false;
    if (m->kind == MOVE_PASS_BLACK || m->kind == MOVE_PASS_WHITE) {
        return move_check_pass(config, m->kind);
    }
    cell_t cell_player = (m->kind == MOVE_PLAY_BLACK) ? CELL_BLACK : CELL_WHITE;
    /* pour un coup play, balayer toutes les directions */
    if (config->board[m->lin*config->board_size+m->col]!=CELL_FREE){return false;}
    for (int dL = -1; dL <= 1; dL++)
        for (int dC = -1; dC <= 1; dC++)
            if (!(dL == 0 && dC == 0)) {
                if (move_check_play(config, m->lin, m->col, cell_player,dL, dC) == true) {
                    return true;
                }
            }
    return false;
}

/**
 * Effectue un coup PLAY
 *  
 * @param config  configuration actuelle du jeu, changee par le coup
 * @param lin     ligne du coup
 * @param col     colonne du coup
 * @param c       jeton joue
 * @param dLin    direction sur ligne
 * @param dCol    direction sur colonne
 */
void move_do_play(config_t* config,size_t lin, size_t col, cell_t c, int dLin, int dCol) {
    board_t plateau = config->board;
    size_t taille = config->board_size;  
    cell_t jeton_adverse = oppose(c);
    cell_t jeton_voisin = board_get(plateau,taille,lin+dLin,col+dCol);

    board_set(plateau,taille,lin,col,c);
    
    for (int i=1; jeton_voisin==jeton_adverse; i++) {     
        jeton_voisin = board_get(plateau,taille,lin+(i+1)*dLin,col+(i+1)*dCol);
        if(c==CELL_BLACK){config->points_black=config->points_black+1; config->points_white=config->points_white-1;}
        else if(c==CELL_WHITE){config->points_white=config->points_white+1; config->points_black=config->points_black-1;}
        board_set(plateau,taille,lin+i*dLin,col+i*dCol,c);
    }
    /* calculer le jeton de l'adversaire à partir de c */
    /* pour tous les voisins dans la direction (dLin, dCol),
     *      tant qu'ils sont des jetons de l'adversaire
     * faire
     *    les retourner, changer le score 
     * fin pour
     */
    return;
}

/**
 * Effectue un coup correct sur la configuration
 * 
 * @param config  configuration actuelle du jeu
 * @param m      coup a appliquer
 */
void move_do(config_t* config, move_t* m) {
    if (config == NULL || m == NULL || m->kind == MOVE_PASS_BLACK|| m->kind == MOVE_PASS_WHITE) {return;}
    /* le changement est fait sur une copie du plateau */
    config_t* copy = config_copy(config);
    cell_t cell_player =(m->kind == MOVE_PLAY_BLACK) ? CELL_BLACK : CELL_WHITE;
    /* balayer toutes les directions et effectue les changements sur copy */
    for (int dL = -1; dL <= 1; dL++){
        for (int dC = -1; dC <= 1; dC++){
            if (!(dL == 0 && dC == 0)) {
                if (move_check_play(config, m->lin, m->col, cell_player,dL, dC) == true){ 
                    move_do_play(copy, m->lin, m->col, cell_player, dL, dC); 
                    copy->board[(m->lin)*config->board_size+(m->col)]=cell_player;
                }
            }
        }
    }
    /* copier le plateau calcule dans la copie */
    board_copy(config->board, copy->board, config->board_size);
    /* et le score */
    config->points_black=copy->points_black;
    config->points_white=copy->points_white;
    /*mise à jour du score pour config en utilisant copy */
    /* liberer la copie */
    config_free(copy);
    return;
}

/**
 * Afficher le coup
 * @param m  pointeur vers le coup
 */
void move_print(move_t* m) {
    if (m == NULL) {
        printf("coup NULL");
    }
    switch (m->kind) {
        case MOVE_PASS_BLACK:
        {
            printf("PASS Noir\n");
            break;
        }
        case MOVE_PLAY_BLACK:
        {
            printf("PLAY Noir (lin=%c, col=%c)\n",
                    (char) ('1' + m->lin),
                    (char) ('A' + m->col));
            break;
        }
        case MOVE_PASS_WHITE:
        {
            printf("PASS Blanc\n");
            break;
        }
        case MOVE_PLAY_WHITE:
        {
            printf("PLAYBlanc (lin=%c, col=%c)\n",
                    (char) ('1' + m->lin),
                    (char) ('A' + m->col));
            break;
        }
    }
}

/**
 * Suggestion des coups possibles
 * @param game     situation du jeu
 * @param m        qui doit jouer
 */

void play_suggest(config_t* game, move_kind_t m){
    cell_t cell_player =(m == MOVE_PLAY_BLACK) ? CELL_BLACK : CELL_WHITE;
    config_t* copy=config_copy(game);
    for (int i=0;i<game->board_size;i++){
        for(int j=0;j<game->board_size;j++){
            if (game->board[i*game->board_size+j]==CELL_FREE){
                for (int dL = -1; dL <= 1; dL++){
                    for (int dC = -1; dC <= 1; dC++){
                        if (!(dL == 0 && dC == 0)) {
                            if (move_check_play(copy, i, j, cell_player,dL, dC) == true){  
                                copy->board[(i)*copy->board_size+(j)]=CELL_SUG;
                            }
                        }
                    }
                }
            }
        }
    }
    config_print(copy);
    config_free(copy);
}

/* met les suggestions sur un board*/
void mettre_les_suggestions(config_t* game, move_kind_t m){
    cell_t c=(m == MOVE_PLAY_BLACK) ? CELL_BLACK : CELL_WHITE;
    for (int i=0;i<game->board_size;i++){
        for(int j=0;j<game->board_size;j++){
            if (game->board[i*game->board_size+j]==CELL_FREE){
                for (int dL = -1; dL <= 1; dL++){
                    for (int dC = -1; dC <= 1; dC++){
                        if (!(dL == 0 && dC == 0)) {
                            if (move_check_play(game, i, j, c,dL, dC) == true){  
                                game->board[(i)*game->board_size+(j)]=CELL_SUG;
                            }
                        }
                    }
                }
            }
        }
    }
    return;
}

/**
 * Interface joueur humain
 * @param config   situation du jeu
 * @param m        qui doit jouer
 * @return         le coup joue
 */
move_t play_human(config_t* game, move_kind_t m) {
    move_t result = move_init(m);
    move_kind_t pass = (m == MOVE_PLAY_BLACK) ? MOVE_PASS_BLACK : MOVE_PASS_WHITE;
    /* lit le coup de l'utilisateur */
    printf("Coup %s (= pour pass, ? pour suggestion, ou case) :\n",(m == MOVE_PLAY_BLACK) ? "Noir" : "Blanc");
    char reponse[4];
    scanf("%s", reponse);
    if (reponse[0] == '=') {
        result.kind = pass;
        return result;
    }
    if (reponse[0] >= 'a' && reponse[0] <= ('a' + game->board_size)) {
        result.col = reponse[0] - 'a';
    }else if (reponse[0] >= 'A' && reponse[0] <= ('A' + game->board_size)) {
        result.col = reponse[0] - 'A';
    }else if(reponse[0] == '?') {
        play_suggest(game,m);
        return move_init(pass);
    }
    if(game->board_size==16){
        if(reponse[1]=='0'){
            if (reponse[2] >= '1' && reponse[2] < ('1' + game->board_size)) {
                result.lin = reponse[2] - '1';}
        }
        else if(reponse[1]=='1'){
            if (reponse[2] >= '0' && reponse[2] < ('1' + 6)) {
            result.lin = reponse[2]-'0'+9;}
        }else {
            printf("Coup incorrect ! (Entrez = ou ? ou lettre colonne suivie de nombre ligne.)\n");
            return move_init(pass);
        }
    }
    if (game->board_size==4 || game->board_size== 8){ 
        if (reponse[1] >= '1' && reponse[1] < ('1' + game->board_size)) {
                result.lin = reponse[1] - '1';}
    }
    return result;
}

/*int points_definitifs(config_t* config, move_t m) {
    size_t size = config->board_size;
    int definitifs_avant = 0;

    for (int i=0; i<size;i++) {
        for (int j=0; j<size; j++) {
            if (Definitif(config,i,j,m.kind)) {
                definitifs_avant +=1;
            }
        }
    }
        
    config_t* copy = config_copy(config);
    move_do(copy,&m);

    int definitifs_apres = 0;

    for (int i=0; i<size;i++) {
        for (int j=0; j<size; j++) {
            if (Definitif(copy,i,j,m.kind)) {
                definitifs_apres +=1;
            }
        }
    }

    config_free(copy);

    return definitifs_apres - definitifs_avant;

}*/

/*compte le nombre de point d un coup dans une direction */
int point_par_coup(config_t* config,size_t lin, size_t col, cell_t c, int dLin, int dCol) {
    board_t plateau = config->board;
    size_t taille = config->board_size;  
    cell_t jeton_voisin = board_get(plateau,taille,lin+dLin,col+dCol);
    points_t points = 0;
    if (board_get(plateau,taille,lin,col)!= CELL_BLACK && board_get(plateau,taille,lin,col)!=CELL_WHITE) {

        if (jeton_voisin==c) {
            return 0;
        }
        if (jeton_voisin==oppose(c)) {
            for (int i=1; jeton_voisin==oppose(c); i++) {
                jeton_voisin = board_get(plateau,taille,lin+i*dLin,col+i*dCol);
                points+=1;
            }
            if (jeton_voisin==c) {
                return points;
            }
        }

        if (jeton_voisin==CELL_FREE) {
            return 0;
        }

    }
    return 0;

}

/**
 * Interface joueur robot
 * tactique: tente de prendre le plus de jetons à chaque coup
 * @param config   situation du jeu
 * @param m        qui doit jouer
 * @return         le coup joue
 * ROBOT Numero 1
 */

move_t play_robot_opt(config_t* game, move_kind_t m) {
    move_t result = move_init(m); 
    int t=0;
    cell_t c=(result.kind == MOVE_PLAY_BLACK) ? CELL_BLACK : CELL_WHITE;
    move_kind_t pass = (result.kind == MOVE_PLAY_BLACK) ? MOVE_PASS_BLACK : MOVE_PASS_WHITE;
    for(int i=0;i<game->board_size;i++){
        for(int j=0;j<game->board_size;j++){
            int v=0;
            for (int dL = -1; dL <= 1; dL++){
                for (int dC = -1; dC <= 1; dC++){
                    if (!(dL == 0 && dC == 0)) {
                        if (move_check_play(game, i, j, c,dL, dC) == true && game->board[i*game->board_size+j] == CELL_FREE){  
                            for (int h=1;h<game->board_size;h++){
                                if (game->board[(i+dL*h)*game->board_size+(j+dC*h)]!=c){v+=1;}
                                else{break;}
                            }
                        }
                    }
                }
            if (v>t){t=v; result.lin=i; result.col=j;}
            }
        }
    }
    if (t==0){ result.kind = pass;}
    return result;
}

/**
 *Renvoie si la position i,j est un coté sur le plateau
 * @param i   ligne
 * @param j   colonne
 * b[i*size + j]
 */
bool testCorner(int i, int j, size_t size) {
    if ( (i==0 && j==0)
    || (i==size-1 && j==0)
    || (i==0 && j==size-1)
    || (i==size-1 && j==size-1) ) {
        return true;
    }
    else {
        return false;
    }
}

/**
 *Renvoie la distance minimale à la bordure
 * @param i   ligne
 * @param j   colonne
 */
int distance_bordure(int i,int j,size_t size) {
    /*On calcule tout d'abord la distance à la bordure de gauche:  */
    int distance_gauche = j;
    int distance_droite = size-1-j;
    int distance_haut = i;
    int distance_bas = size-1-i;

    int lin = (distance_gauche<=distance_droite) ? distance_gauche: distance_droite;
    int col = (distance_haut<distance_bas) ? distance_haut: distance_bas;

    int distance = (lin<=col) ? lin: col; 

    return distance;
}

/**
 * Interface joueur robot
 * tactique: tente de se rapprocher le plus possible des coins
 * @param config   situation du jeu
 * @param m        qui doit jouer
 * @return         le coup joue
 * ROBOT Numero 2
 */

move_t play_robot_corner(config_t* game, move_kind_t m) {
    move_t result = move_init(m);
    move_kind_t pass = (m == MOVE_PLAY_BLACK) ? MOVE_PASS_BLACK : MOVE_PASS_WHITE;
    size_t size = game->board_size;

    config_t* copy= config_copy(game);
    mettre_les_suggestions(copy,m);

    int distance = distance_bordure(size/2,size/2,size);

    int linbest = size/2;
    int colbest = size/2;

    for (int i=0; i<size; i++ ) {
        for (int j=0; j<size; j++ ) {
            cell_t cellule = board_get(copy->board,size,i,j);
            if (cellule==CELL_SUG) {
                if (testCorner(i,j,size) ) {
                    result.lin = i;
                    result.col = j;
                    return result;
                }
                else if (distance_bordure(i,j,size)<=distance) {
                    linbest = i;
                    colbest = j;      
                }    
            }
        }
    }
    result.lin = linbest;
    result.col = colbest;
    config_free(copy);
    if (result.lin==size/2 && result.col==size/2) {
        result.kind = pass;
        //move_print(&result);    
        return move_init(pass);
    }
    //move_print(&result);
    return result;
}

/**
 * Interface joueur robot
 * tactique: Joue aléatoirement sur une case ou il est possible de jouer
 * @param config   situation du jeu
 * @param m        qui doit jouer
 * @return         le coup joue
 * ROBOT Numero 6
 */

move_t play_robot_rand(config_t* game, move_kind_t m) {
    move_t result = move_init(m);
    move_kind_t pass = (m == MOVE_PLAY_BLACK) ? MOVE_PASS_BLACK : MOVE_PASS_WHITE;
    size_t size = game->board_size;

    config_t* copy= config_copy(game);
    mettre_les_suggestions(copy,m);
    int r;
    int rmax = 0;

    for (int i=0; i<size; i++ ) {
        for (int j=0; j<size; j++ ) {
            cell_t cellule = board_get(copy->board,size,i,j);
            if (cellule==CELL_SUG) {
                r = rand() % 200 + 1; // Returns a pseudo-random integer
                if (rmax < r) {
                    rmax = r;
                    result.lin = i;
                    result.col = j;
                }
            }
        }
    }
    config_free(copy);
    if (rmax == 0) {
        result.kind = pass;
        //move_print(&result);    
        return move_init(pass);
    }
    //move_print(&result);
    return result;
}


bool case_interdite(int i,int j,size_t size) {
    if ( (i==1 && j==1)
        ||(i==size-2 && j==1)
        ||(i==size-2 && j==size-2)
        ||(i==1 && j==size-2) ) {
            return true;
    }
    else {
        return false;
    }
}


/**
 * Renvoie true si un coup coller au coin est possible
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * @param i   ligne
 * @param j   colonne
 */
bool un_coin_colle(config_t* game, int i, int j, move_kind_t m) {
    cell_t c =( m == MOVE_PLAY_BLACK) ? CELL_BLACK : CELL_WHITE;
    board_t plateau = game->board;
    size_t taille = game->board_size; 
    if (i==0){
        if(plateau[0]==c){
            for(int v=j;v==0;v--){
                if(plateau[v-1]!=c){return false;}
            }
            return true;
        }
        if(plateau[taille-1]==c){
            for(int v=j;v==taille-1;v++){
                if(plateau[v+1]!=c){return false;}
            }
            return true;
        }
    }
    if (j==0){
        if(plateau[0]==c){
            for(int v=i;v==0;v--){
                if(plateau[(v-1)*taille]!=c){return false;}
            }
            return true;
        }
        if(plateau[(taille-1)*taille]==c){
            for(int v=i;v==taille-1;v++){
                if(plateau[(v+1)*taille]!=c){return false;}
            }
            return true;
        }
    }
    if (i==taille-1){
        if(plateau[(taille-1)*taille]==c){
            for(int v=j;v==0;v--){
                if(plateau[(taille-1)*taille+v]!=c){return false;}
            }
            return true;
        }
        if(plateau[(taille-1)*taille+taille-1]==c){
            for(int v=j;v==taille-1;v++){
                if(plateau[(taille+1)*taille+v]!=c){return false;}
            }
            return true;
        }
    }
    if (j==taille-1){
        if(plateau[taille-1]==c){
            for(int v=i;v==0;v--){
                if(plateau[(v-1)*taille+taille-1]!=c){return false;}
            }
            return true;
        }
        if(plateau[(taille-1)*taille+taille-1]==c){
            for(int v=i;v==taille-1;v++){
                if(plateau[(v+1)*taille+taille-1]!=c){return false;}
            }
            return true;
        }
    }
    return false;
}

/**
 * Renvoie true si un jeton adverse est collé à un de nos groupe sur le bord 
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * @param i   ligne
 * @param j   colonne
 */
bool un_adv_colle_a_un_groupe_nous(config_t* game, int i, int j, move_kind_t m){
    cell_t c =( m == MOVE_PLAY_BLACK) ? CELL_BLACK : CELL_WHITE;
    cell_t c2 =( m == MOVE_PLAY_BLACK) ? CELL_WHITE : CELL_BLACK;
    board_t plateau = game->board;
    size_t taille = game->board_size; 
   if(i==0){
        if(plateau[j-1]==c2){
            for(int v=j-2;plateau[v+1]==c2;v++){
                if(plateau[v]==c){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[j+1]==c2){
            for(int v=j+2;plateau[v-1]==c2;v++){
                if(plateau[v]==c){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
    if(j==0){
        if(plateau[(i-1)*taille]==c2){
            for(int v=i-2;plateau[(v+1)*taille]==c2;v++){
                if(plateau[(v)*taille]==c){return true;}
                if(i<0){break;}
            }
        }
        if(plateau[(i+1)*taille]==c2){
            for(int v=i+2;plateau[(v-1)*taille]==c2;v++){
                if(plateau[(v)*taille]==c){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
    if(i==taille-1){
        if(plateau[(taille-1)*taille+j-1]==c2){
            for(int v=j-2;plateau[(taille-1)*taille+v+1]==c2;v++){
                if(plateau[(taille-1)*taille+v]==c){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[(taille-1)*taille+j+1]==c2){
            for(int v=j+2;plateau[(taille-1)*taille+v-1]==c2;v++){
                if(plateau[(taille-1)*taille+v]==c){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
     if(j==taille-1){
        if(plateau[(i-1)*taille+taille-1]==c2){
            for(int v=i-2;plateau[(v+1)*taille+taille-1]==c2;v++){
                if(plateau[(v)*taille+taille-1]==c){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[(i+1)*taille+taille-1]==c2){
            for(int v=i+2;plateau[(v-1)*taille+taille-1]==c2;v++){
                if(plateau[(v)*taille+taille-1]==c){return true;}
                if(v>taille-1){break;}
            }
        }
     }
    return false;  
}

/**
 * Renvoie true si le coté du plateu est vide 
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * @param i   ligne
 * @param j   colonne
 */
bool bord_est_vide(config_t* game, int i, int j, move_kind_t m){
    board_t plateau = game->board;
    size_t taille = game->board_size; 
    if(i==0){
        for(int v=0;v<taille;v++){
            if(plateau[v]!=CELL_FREE){return false;}
        }
    }
    if(j==0){
        for(int v=0;v<taille;v++){
            if(plateau[v*taille]!=CELL_FREE){return false;}
        }
    }
    if(i==taille-1){
        for(int v=0;v<taille;v++){
            if(plateau[(taille-1)*taille+v]!=CELL_FREE){return false;}
        }
    }
    if(j==taille-1){
        for(int v=0;v<taille;v++){
            if(plateau[v*taille+taille-1]!=CELL_FREE){return false;}
        }
    }
    return true;
}

/**
 * Renvoie true si un groupe adverse est collé au coup 
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * @param i   ligne
 * @param j   colonne
 */
bool deja_un_groupe_adv_colle(config_t* game, int i, int j, move_kind_t m){
    cell_t c2 =( m == MOVE_PLAY_BLACK) ? CELL_WHITE : CELL_BLACK;
    board_t plateau = game->board;
    size_t taille = game->board_size; 
    if(i==0){
        if(plateau[j-1]==c2){
            for(int v=j-2;plateau[v+1]==c2;v++){
                if(plateau[v]==CELL_FREE){return true;}
                if(v<0){return true;}
            }
        }
        if(plateau[j+1]==c2){
            for(int v=j+2;plateau[v-1]==c2;v++){
                if(plateau[v]==CELL_FREE){return true;}
                if(v>taille-1){return true;}
            }
        }
    return false;
    }
    if(j==0){
        if(plateau[(i-1)*taille]==c2){
            for(int v=i-2;plateau[(v+1)*taille]==c2;v++){
                if(plateau[(v)*taille]==CELL_FREE){return true;}
                if(i<0){return true;}
            }
        }
        if(plateau[(i+1)*taille]==c2){
            for(int v=i+2;plateau[(v-1)*taille]==c2;v++){
                if(plateau[(v)*taille]==CELL_FREE){return true;}
                if(v>taille-1){return true;}
            }
        }
    return false;
    }
    if(i==taille-1){
        if(plateau[(taille-1)*taille+j-1]==c2){
            for(int v=j-2;plateau[(taille-1)*taille+v+1]==c2;v++){
                if(plateau[(taille-1)*taille+v]==CELL_FREE){return true;}
                if(v<0){return true;}
            }
        }
        if(plateau[(taille-1)*taille+j+1]==c2){
            for(int v=j+2;plateau[(taille-1)*taille+v-1]==c2;v++){
                if(plateau[(taille-1)*taille+v]==CELL_FREE){return true;}
                if(v>taille-1){return true;}
            }
        }
    return false;
    }
     if(j==taille-1){
        if(plateau[(i-1)*taille+taille-1]==c2){
            for(int v=i-2;plateau[(v+1)*taille+taille-1]==c2;v++){
                if(plateau[(v)*taille+taille-1]==CELL_FREE){return true;}
                if(v<0){return true;}
            }
        }
        if(plateau[(i+1)*taille+taille-1]==c2){
            for(int v=i+2;plateau[(v-1)*taille+taille-1]==c2;v++){
                if(plateau[(v)*taille+taille-1]==CELL_FREE){return true;}
                if(v>taille-1){return true;}
            }
        }
     }
    return false;
}

/**
 * Renvoie true si un groupe adverse est déjà sur le bord au coup 
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * @param i   ligne
 * @param j   colonne
 */
bool deja_un_groupe_adv_pas_colle(config_t* game, int i, int j, move_kind_t m){
    cell_t c2 =( m == MOVE_PLAY_BLACK) ? CELL_WHITE : CELL_BLACK;
    board_t plateau = game->board;
    size_t taille = game->board_size;
    if(i==0){
        if(plateau[j-1]==CELL_FREE){
            for(int v=j-2;plateau[v+1]==CELL_FREE;v++){
                if(plateau[v]==c2){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[j+1]==CELL_FREE){
            for(int v=j+2;plateau[v-1]==CELL_FREE;v++){
                if(plateau[v]==c2){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
    if(j==0){
        if(plateau[(i-1)*taille]==CELL_FREE){
            for(int v=i-2;plateau[(v+1)*taille]==CELL_FREE;v++){
                if(plateau[(v)*taille]==c2){return true;}
                if(i<0){break;}
            }
        }
        if(plateau[(i+1)*taille]==CELL_FREE){
            for(int v=i+2;plateau[(v-1)*taille]==CELL_FREE;v++){
                if(plateau[(v)*taille]==c2){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
    if(i==taille-1){
        if(plateau[(taille-1)*taille+j-1]==CELL_FREE){
            for(int v=j-2;plateau[(taille-1)*taille+v+1]==CELL_FREE;v++){
                if(plateau[(taille-1)*taille+v]==c2){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[(taille-1)*taille+j+1]==CELL_FREE){
            for(int v=j+2;plateau[(taille-1)*taille+v-1]==CELL_FREE;v++){
                if(plateau[(taille-1)*taille+v]==c2){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
     if(j==taille-1){
        if(plateau[(i-1)*taille+taille-1]==CELL_FREE){
            for(int v=i-2;plateau[(v+1)*taille+taille-1]==CELL_FREE;v++){
                if(plateau[(v)*taille+taille-1]==c2){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[(i+1)*taille+taille-1]==CELL_FREE){
            for(int v=i+2;plateau[(v-1)*taille+taille-1]==CELL_FREE;v++){
                if(plateau[(v)*taille+taille-1]==c2){return true;}
                if(v>taille-1){break;}
            }
        }
     }
    return false;
}

/**
 * Renvoie true si deux pions adverses encadrent le coup
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * @param i   ligne
 * @param j   colonne
 */
bool encadrer_par_deux_pion_adv(config_t* game, int i, int j, move_kind_t m){
    cell_t c2 =( m == MOVE_PLAY_BLACK) ? CELL_WHITE : CELL_BLACK;
    board_t plateau = game->board;
    size_t taille = game->board_size;
    if(i==0){
        if(plateau[j-1]==c2 && plateau[j+1]==c2){
            return true;
        }
    }
    if(j==0){
        if(plateau[(i-1)*taille]==c2 && plateau[(i+1)*taille]==c2){
            return true;
        }
    }
    if(i==taille-1){
        if(plateau[(taille-1)*taille+j-1]==c2 && plateau[(taille-1)*taille+j+1]==c2){
                return true;
            }
    }
    if(j==taille-1){
        if(plateau[(i-1)*taille+taille-1]==c2 && plateau[(i+1)*taille+taille-1]==c2){
            return true;
        }
    }   
    return false;
}

/**
 * Renvoie true si un de nos groupes est collé au coup mais qu un pion adverse touche ce groupe 
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * @param i   ligne
 * @param j   colonne
 */
bool deja_un_groupe_nous_colle_danger(config_t* game, int i, int j, move_kind_t m){
    cell_t c =( m == MOVE_PLAY_BLACK) ? CELL_BLACK : CELL_WHITE;
    cell_t c2 =( m == MOVE_PLAY_BLACK) ? CELL_WHITE : CELL_BLACK;
    board_t plateau = game->board;
    size_t taille = game->board_size;
    if(i==0){
        if(plateau[j-1]==c){
            for(int v=j-2;plateau[v+1]==c;v++){
                if(plateau[v]==c2){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[j+1]==c){
            for(int v=j+2;plateau[v-1]==c;v++){
                if(plateau[v]==c2){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
    if(j==0){
        if(plateau[(i-1)*taille]==c){
            for(int v=i-2;plateau[(v+1)*taille]==c;v++){
                if(plateau[(v)*taille]==c2){return true;}
                if(i<0){break;}
            }
        }
        if(plateau[(i+1)*taille]==c){
            for(int v=i+2;plateau[(v-1)*taille]==c;v++){
                if(plateau[(v)*taille]==c2){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
    if(i==taille-1){
        if(plateau[(taille-1)*taille+j-1]==c){
            for(int v=j-2;plateau[(taille-1)*taille+v+1]==c;v++){
                if(plateau[(taille-1)*taille+v]==c2){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[(taille-1)*taille+j+1]==c){
            for(int v=j+2;plateau[(taille-1)*taille+v-1]==c;v++){
                if(plateau[(taille-1)*taille+v]==c2){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
     if(j==taille-1){
        if(plateau[(i-1)*taille+taille-1]==c){
            for(int v=i-2;plateau[(v+1)*taille+taille-1]==c;v++){
                if(plateau[(v)*taille+taille-1]==c2){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[(i+1)*taille+taille-1]==c){
            for(int v=i+2;plateau[(v-1)*taille+taille-1]==c;v++){
                if(plateau[(v)*taille+taille-1]==c2){return true;}
                if(v>taille-1){break;}
            }
        }
     }
    return false; 
}

/**
 * Renvoie true si un de nos groupes est collé au coup et que aucun pions adverses touchent ce groupe 
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * @param i   ligne
 * @param j   colonne
 */
bool deja_un_groupe_nous_colle_cool(config_t* game, int i, int j, move_kind_t m){
    cell_t c =( m == MOVE_PLAY_BLACK) ? CELL_BLACK : CELL_WHITE;
    board_t plateau = game->board;
    size_t taille = game->board_size;
    if(i==0){
        if(plateau[j-1]==c){
            for(int v=j-2;plateau[v+1]==c;v++){
                if(plateau[v]==CELL_FREE){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[j+1]==c){
            for(int v=j+2;plateau[v-1]==c;v++){
                if(plateau[v]==CELL_FREE){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
    if(j==0){
        if(plateau[(i-1)*taille]==c){
            for(int v=i-2;plateau[(v+1)*taille]==c;v++){
                if(plateau[(v)*taille]==CELL_FREE){return true;}
                if(i<0){break;}
            }
        }
        if(plateau[(i+1)*taille]==c){
            for(int v=i+2;plateau[(v-1)*taille]==c;v++){
                if(plateau[(v)*taille]==CELL_FREE){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
    if(i==taille-1){
        if(plateau[(taille-1)*taille+j-1]==c){
            for(int v=j-2;plateau[(taille-1)*taille+v+1]==c;v++){
                if(plateau[(taille-1)*taille+v]==CELL_FREE){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[(taille-1)*taille+j+1]==c){
            for(int v=j+2;plateau[(taille-1)*taille+v-1]==c;v++){
                if(plateau[(taille-1)*taille+v]==CELL_FREE){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
     if(j==taille-1){
        if(plateau[(i-1)*taille+taille-1]==c){
            for(int v=i-2;plateau[(v+1)*taille+taille-1]==c;v++){
                if(plateau[(v)*taille+taille-1]==CELL_FREE){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[(i+1)*taille+taille-1]==c){
            for(int v=i+2;plateau[(v-1)*taille+taille-1]==c;v++){
                if(plateau[(v)*taille+taille-1]==CELL_FREE){return true;}
                if(v>taille-1){break;}
            }
        }
     }
    return false;   
}

/**
 * Renvoie true si un de nos groupes est déjà sur le bord du coup 
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * @param i   ligne
 * @param j   colonne
 */
bool deja_un_groupe_nous_pas_colle(config_t* game, int i, int j, move_kind_t m){
    cell_t c =( m == MOVE_PLAY_BLACK) ? CELL_BLACK : CELL_WHITE;
    board_t plateau = game->board;
    size_t taille = game->board_size;
    if(i==0){
        if(plateau[j-1]==CELL_FREE){
            for(int v=j-2;plateau[v+1]==CELL_FREE;v++){
                if(plateau[v]==c){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[j+1]==CELL_FREE){
            for(int v=j+2;plateau[v-1]==CELL_FREE;v++){
                if(plateau[v]==c){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
    if(j==0){
        if(plateau[(i-1)*taille]==CELL_FREE){
            for(int v=i-2;plateau[(v+1)*taille]==CELL_FREE;v++){
                if(plateau[(v)*taille]==c){return true;}
                if(i<0){break;}
            }
        }
        if(plateau[(i+1)*taille]==CELL_FREE){
            for(int v=i+2;plateau[(v-1)*taille]==CELL_FREE;v++){
                if(plateau[(v)*taille]==c){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
    if(i==taille-1){
        if(plateau[(taille-1)*taille+j-1]==CELL_FREE){
            for(int v=j-2;plateau[(taille-1)*taille+v+1]==CELL_FREE;v++){
                if(plateau[(taille-1)*taille+v]==c){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[(taille-1)*taille+j+1]==CELL_FREE){
            for(int v=j+2;plateau[(taille-1)*taille+v-1]==CELL_FREE;v++){
                if(plateau[(taille-1)*taille+v]==c){return true;}
                if(v>taille-1){break;}
            }
        }
    return false;
    }
     if(j==taille-1){
        if(plateau[(i-1)*taille+taille-1]==CELL_FREE){
            for(int v=i-2;plateau[(v+1)*taille+taille-1]==CELL_FREE;v++){
                if(plateau[(v)*taille+taille-1]==c){return true;}
                if(v<0){break;}
            }
        }
        if(plateau[(i+1)*taille+taille-1]==CELL_FREE){
            for(int v=i+2;plateau[(v-1)*taille+taille-1]==CELL_FREE;v++){
                if(plateau[(v)*taille+taille-1]==c){return true;}
                if(v>taille-1){break;}
            }
        }
     }
    return false;   
}

/**
 * Renvoie true si les coordonnées rentrées en entrée correspondent à un coup situé sur les coordonnées testées
 * @param game   situation du jeu
 * @param i   ligne
 * @param j   colonne
 */
bool carre_pas_fou(config_t* game, int i, int j){
    if ( (i==0 && j==1)
    || (i==0 && j==game->board_size-2)
    || (i==1 && j==0)
    || (i==game->board_size-2 && j==0)
    || (i==game->board_size-1 && j==1)
    || (i==game->board_size-1 && j==game->board_size-2)
    || (i==1 && j==game->board_size-1)
    || (i==game->board_size-2 && j==game->board_size-1) ){
        return true;
    }
    else {
        return false;
    }
}

/**
 * Renvoie true si il ne reste que un coup possible sur un bord
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * @param i   ligne
 * @param j   colonne
 */
bool dernier_coup_de_la_ligne(config_t* game, int i, int j, move_kind_t m){
    board_t plateau = game->board;
    size_t taille = game->board_size;
    int H=0;
    int B=0;
    int G=0;
    int D=0;
    for(int x=0;x<taille;x++){
        for(int y=0;y<taille;y++){
            if(plateau[y]==CELL_FREE){B+=1;}
            if(plateau[x*taille]==CELL_FREE){G+=1;}
            if(plateau[x*taille+taille-1]==CELL_FREE){D+=1;}
            if(plateau[(taille-1)*taille+y]==CELL_FREE){H+=1;}
        }
    }
    if(i==0 && B==1){return true;}
    if(j==0 && G==1){return true;}
    if(i==taille-1 && H==1){return true;}
    if(j==taille-1 && D==1){return true;}
    return false;
}
    
/**
 * Renvoie true s'il reste deux pions sur une bordure
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * @param i   ligne
 * @param j   colonne
 */
bool plus_que_deux_cases(config_t* game, int i, int j, move_kind_t m){
    board_t plateau = game->board;
    size_t taille = game->board_size;
    int H=0;
    int B=0;
    int G=0;
    int D=0;
    for(int x=0;x<taille;x++){
        for(int y=0;y<taille;y++){
            if(plateau[y]==CELL_FREE){B+=1;}
            if(plateau[x*taille]==CELL_FREE){G+=1;}
            if(plateau[x*taille+taille-1]==CELL_FREE){D+=1;}
            if(plateau[(taille-1)*taille+y]==CELL_FREE){H+=1;}
        }
    }
    if(i==0 && B==2){return true;}
    if(j==0 && G==2){return true;}
    if(i==taille-1 && H==2){return true;}
    if(j==taille-1 && D==2){return true;}
    return false;
}

/**
 * Renvoie le score du joueur
 * @param game   situation du jeu
 * @param m   qui doit jouer
 */
points_t score(config_t* game, move_kind_t m){
    cell_t c= (m==MOVE_PLAY_BLACK) ? CELL_BLACK: CELL_WHITE;
    if (c==CELL_BLACK){return game->points_black;}
    else {return game->points_white;}
}


/**
 * renvoie le jeton a coté du jeton rentré sur le bord
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * @param i   ligne
 * @param j   colonne
 */
cell_t deplacement_bordure(config_t* game, size_t size, int lin, int col,int k) {
    
    if (col==0) {
        return board_get(game->board,size,lin+k,0);
    }
    else if (col==size-1) {
            return board_get(game->board,size,lin+k,size-1);
    }
    else if (lin==size-1) {
        return board_get(game->board,size,size-1,col+k);
    }
    else if (lin==0) {
        return board_get(game->board,size,0,col+k);
    }

    return CELL_OTHER;

}



/**
 * Renvoie true si un pion est definitif au bord
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * @param i   ligne
 * @param j   colonne
 */
bool bordure_definitive(config_t* game, int lin, int col, move_kind_t m) {
    cell_t allie = (m == MOVE_PLAY_BLACK) ? CELL_BLACK : CELL_WHITE;
    size_t size = game->board_size;
    cell_t voisin = board_get(game->board,size,lin,col);
    int k=0;

    while (!testCorner(lin+k,col+k,size)) {     
        voisin = deplacement_bordure(game,size,lin,col,k);
        if (voisin != allie) {
            break;
        }
        k++;
    }

    voisin = deplacement_bordure(game,size,lin,col,k);
    if (voisin==allie) {
        return true;
    }
    else {
        k=0;
        while (!testCorner(lin-k,col-k,size)) {
            voisin = deplacement_bordure(game,size,lin,col,-k);
            if (voisin != allie) {
                return false;
            }
            k++;
        }

        voisin = deplacement_bordure(game,size,lin,col,-k);
        if (voisin==allie) {
            return true;
        }
        else {
            return false;
        }       
    }

}

/**
 * Renvoie true si un pion est imprenable 
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * @param i   ligne
 * @param j   colonne
 */
bool Definitif (config_t* game, int lin, int col, move_kind_t m) {
    
    cell_t allie = (m == MOVE_PLAY_BLACK) ? CELL_BLACK : CELL_WHITE;    
    /* On teste la fonction dans une simulation où on a posé le pion: */
    move_t move;
    move.kind = m;
    move.lin=lin;
    move.col=col;
    config_t* copy = config_copy(game);
    move_do(copy,&move);
    
    size_t size = copy->board_size;
    int distance = distance_bordure(lin,col,size);
    int compteur=0;

    int t[8][2] = {{-1,-1},   // servira à stocker l'ensemble des coordonnés des jetons imprenables voisin au jeton testé
                   {-1,-1}, 
                   {-1,-1}, 
                   {-1,-1}, 
                   {-1,-1}, 
                   {-1,-1}, 
                   {-1,-1}, 
                   {-1,-1}};

    if (testCorner(lin,col,size)) {
        return true;
    }
    if (distance==0) {
        return bordure_definitive(copy,lin,col,m);
    }
 
    for (int dL=-1; dL<=1; dL++) {
        for (int dC=-1; dC<=1; dC++)  {
       
            cell_t jeton_voisin = board_get(copy->board,size,lin+dL,col+dC);

            
            if (!(dL==0 && dC==0) 
            && jeton_voisin==allie
            && distance_bordure(lin+dL,col+dC,size)<distance  /*on test seulement pour les pions les plus proches de la bordure*/ 
            && Definitif(copy,lin+dL,col+dC,m)) { 

                bool test = true; // true si le jeton n'est pas en face d'un jeton déjà testé, false sinon
                for (int i=0; i<6; i++) {
                    /*On regarde si le jeton n'est pas en face d'un jeton déjà testé */
                    if (lin-dL==t[i][0] && lin-dC==t[i][1]) { 
                        test = false; // le jeton est en face d'un jeton déjà testé
                    }
                }

                if (test==true && Definitif(copy,lin+dL,col+dC,m)) { //si le jeton n'est pas en face d'un jeton déjà testé:
                    //on l'ajoute aux jetons déjà testés:
                    t[compteur][0]=lin+dL; 
                    t[compteur][1]=col+dC;
                    compteur++;           
                }
            }
        }
    }
    config_free(copy); 
    if (compteur==4) {
        return true;
    }
    else {
        return false;
    }    
        
}
    
/**
 * Renvoie le meilleur score possible de l adversaire selon la techniqu de play_robot_personlisé
 * @param game   situation du jeu
 * @param m   qui doit jouer
 */
int Prevision_Score_Adverse(config_t* game, move_kind_t m) {
    cell_t type = (m==MOVE_PLAY_BLACK) ? CELL_BLACK: CELL_WHITE;
    size_t size = game->board_size;

    config_t* copy = config_copy(game);
    mettre_les_suggestions(copy,m);
    
    int meilleur_score = 0;
    if (score(game,m)>1){
        for (int i=0; i<size; i++ ) {
            for (int j=0; j<size; j++ ) {
                cell_t cellule = board_get(copy->board,size,i,j);
                int distance = distance_bordure(i,j,size);
                int score_cellule = 0;
                if (cellule==CELL_SUG) {
                    if ( testCorner(i,j,size)) {
                        score_cellule = 10000;
                    }
                    else if (distance==0) {
                        score_cellule = 5000;
                        if(un_coin_colle(game,i,j,m)== true){
                            score_cellule+=2500;
                        }
                        if(un_adv_colle_a_un_groupe_nous(game,i,j,m)==true){
                            score_cellule+=2000;
                        }
                        if(bord_est_vide(game,i,j,m)==true){
                            score_cellule+=350;
                        }
                        if(deja_un_groupe_adv_colle(game,i,j,m)==true){
                            score_cellule-=3500;
                        }
                        if(deja_un_groupe_adv_pas_colle(game,i,j,m)==true){
                            score_cellule-=950;
                        }
                        if(encadrer_par_deux_pion_adv(game,i,j,m)==true){
                            score_cellule+=200;
                        }
                        else if(deja_un_groupe_nous_colle_danger(game,i,j,m)==true){
                            score_cellule-=2010;
                        }
                        if(deja_un_groupe_nous_colle_cool(game,i,j,m)==true){
                            score_cellule+=100;
                        }
                        else if(deja_un_groupe_nous_pas_colle(game,i,j,m)==true){
                            score_cellule-=1010;
                        }
                        if(carre_pas_fou(game,i,j)==true){
                            score_cellule-=1010;
                        }
                        if(dernier_coup_de_la_ligne(game,i,j,m)==true){
                            score_cellule+=375;
                        }
                        if(plus_que_deux_cases(game,i,j,m)==true){
                            score_cellule-=2010;
                        }
                    }
                    else if (distance==2) {
                        score_cellule = 4000;
                        if (Definitif(game,i,j,m)) {
                            score_cellule+=3500 ;
                        }
                    }
                    else if (distance==1 && !(case_interdite(i,j,size))) {
                        score_cellule = 3000;
                        if (Definitif(game,i,j,m)) {
                            score_cellule+=4500;
                        }
                    }
                    else if (case_interdite(i,j,size)) {
                        score_cellule = 1000;
                        if (Definitif(game,i,j,m)) {
                            score_cellule+= 6500;
                        }
                    }
                    else { 
                        score_cellule = 2000; 
                        if (Definitif(game,i,j,m)) {
                            score_cellule+=5500;
                        }
                    }

                    int score_case = 0;
                    for (int dC=-1; dC<=1; dC++){
                        for(int dL=-1; dL<=1; dL++){
                            if (!(dL==0 && dC==0)) {
                                score_case += point_par_coup(game,i,j,type,dL,dC);
                            }
                        }
                    }
                    score_cellule += score_case;

                }
                if (score_cellule>meilleur_score) {
                    meilleur_score = score_cellule;
                }

            }
        }
    }
    else {
        for (int i=0; i<size; i++ ) {
            for (int j=0; j<size; j++ ) {
                cell_t cellule = board_get(copy->board,size,i,j);
                int score_cellule = 0;
                if (cellule==CELL_SUG) {
                    for (int dC=-1; dC<=1; dC++){
                        for(int dL=-1; dL<=1; dL++){
                            if (!(dL==0 && dC==0)) {
                                score_cellule += point_par_coup(game,i,j,type,dL,dC);
                            }
                        }
                    }
                }
                if (score_cellule>meilleur_score) {
                    meilleur_score = score_cellule;
                }
            }
        }
    }
    
    config_free(copy);
    return meilleur_score;
}

/**
 * IA qui joue le coup qui minimise score du prochain score adverse 
 * @param game   situation du jeu
 * @param m   qui doit jouer
 * ROBOT Numero 3
 */
move_t play_robot_minScoreAdverse(config_t* game, move_kind_t m) {
    move_t result = move_init(m);
    move_kind_t pass = (m == MOVE_PLAY_BLACK) ? MOVE_PASS_BLACK : MOVE_PASS_WHITE;
    size_t size = game->board_size;
    cell_t type = (m==MOVE_PLAY_BLACK) ? CELL_WHITE : CELL_BLACK;
    config_t* copy1 = config_copy(game);
    mettre_les_suggestions(copy1,m);
    int score_adverse = 0;
    int minScore = 100000;

    int linbest = size/2;
    int colbest = size/2;   

    for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            cell_t jeton = board_get(copy1->board,size,i,j);
            if (jeton==CELL_SUG) {

                config_t* copy2 = config_copy(game);
                move_t move;
                move.kind=m;
                move.lin=i;
                move.col=j;
                move_do(copy2,&move);
                score_adverse = Prevision_Score_Adverse(copy2,type);
                if (score_adverse<minScore) {
                    minScore = score_adverse;
                    linbest=i;
                    colbest=j;
                }
                config_free(copy2);

            }
        }    
    }
    result.lin = linbest;
    result.col = colbest;
    config_free(copy1);
    if (minScore==100000) {
        result.kind = pass;
        //move_print(&result);    
        return move_init(pass);
    }
    //move_print(&result);
    return result;  

   
}



/** IA super puissante, fonctionne en donnant des 
 * priorités aux coups, et en optimisant le score
 * @param game  la config du jeu
 * @param m     le type du mouvement
 * ROBOT Numero 4
 */
move_t play_robot_surpuissant(config_t* game, move_kind_t m) {
    move_t result = move_init(m);
    cell_t type = (m==MOVE_PLAY_BLACK) ? CELL_BLACK: CELL_WHITE;
    move_kind_t pass = (m == MOVE_PLAY_BLACK) ? MOVE_PASS_BLACK : MOVE_PASS_WHITE;
    size_t size = game->board_size;

    config_t* copy= config_copy(game);
    mettre_les_suggestions(copy,m);
    
    int meilleur_score = 0;
    
    int linbest = size/2;
    int colbest = size/2;
    if (score(game,m)>1){
        for (int i=0; i<size; i++ ) {
            for (int j=0; j<size; j++ ) {
                cell_t cellule = board_get(copy->board,size,i,j);
                int distance = distance_bordure(i,j,size);
                int score_cellule = 0;
                if (cellule==CELL_SUG) {
                    if ( testCorner(i,j,size)) {
                        score_cellule = 10000;
                    }
                    else if (distance==0) {
                        score_cellule = 5000;
                        if(un_coin_colle(game,i,j,m)== true){
                            score_cellule+=2500;
                        }
                        else if(un_adv_colle_a_un_groupe_nous(game,i,j,m)==true){
                            score_cellule+=2000;
                        }
                        else if(bord_est_vide(game,i,j,m)==true){
                            score_cellule+=350;
                        }
                        else if(deja_un_groupe_adv_colle(game,i,j,m)==true){
                            score_cellule-=3500;
                        }
                        else if(deja_un_groupe_adv_pas_colle(game,i,j,m)==true){
                            score_cellule-=950;
                        }
                       else  if(encadrer_par_deux_pion_adv(game,i,j,m)==true){
                            score_cellule+=200;
                        }
                        else if(deja_un_groupe_nous_colle_danger(game,i,j,m)==true){
                            score_cellule-=2010;
                        }
                        else if(deja_un_groupe_nous_colle_cool(game,i,j,m)==true){
                            score_cellule+=100;
                        }
                        else if(deja_un_groupe_nous_pas_colle(game,i,j,m)==true){
                            score_cellule-=1010;
                        }
                        else if(carre_pas_fou(game,i,j)==true){
                            score_cellule-=1010;
                        }
                        else if(dernier_coup_de_la_ligne(game,i,j,m)==true){
                            score_cellule+=375;
                        }
                        else if(plus_que_deux_cases(game,i,j,m)==true){
                            score_cellule-=2010;
                        }
                    }
                    else if (distance==2) {
                        score_cellule = 4000;
                        if (Definitif(game,i,j,m)) {
                            score_cellule+=3500 ;
                        }
                    }
                    else if (distance==1 && !(case_interdite(i,j,size))) {
                        score_cellule = 3000;
                        if (Definitif(game,i,j,m)) {
                            score_cellule+=4500;
                        }
                    }
                    else if (case_interdite(i,j,size)) {
                        score_cellule = 1000;
                        if (Definitif(game,i,j,m)) {
                            score_cellule+= 6500;
                        }
                    }
                    else { 
                        score_cellule = 2000; 
                        if (Definitif(game,i,j,m)) {
                            score_cellule+=5500;
                        }
                    }

                    int score_case = 0;
                    for (int dC=-1; dC<=1; dC++){
                        for(int dL=-1; dL<=1; dL++){
                            if (!(dL==0 && dC==0)) {
                                score_case += point_par_coup(game,i,j,type,dL,dC);
                            }
                        }
                    }
                    score_cellule += score_case;

                }
                if (score_cellule>meilleur_score) {
                    meilleur_score = score_cellule;
                    linbest = i;
                    colbest = j;
                }

            }
        }
    }
    else {
        for (int i=0; i<size; i++ ) {
            for (int j=0; j<size; j++ ) {
                cell_t cellule = board_get(copy->board,size,i,j);
                int score_cellule = 0;
                if (cellule==CELL_SUG) {
                    for (int dC=-1; dC<=1; dC++){
                        for(int dL=-1; dL<=1; dL++){
                            if (!(dL==0 && dC==0)) {
                                score_cellule += point_par_coup(game,i,j,type,dL,dC);
                            }
                        }
                    }
                }
                if (score_cellule>meilleur_score) {
                    meilleur_score = score_cellule;
                    linbest = i;
                    colbest = j;
                }
            }
        }
    }
    result.lin = linbest;
    result.col = colbest;
    config_free(copy);
    if (meilleur_score==0) {
        result.kind = pass;
        //move_print(&result);    
        return move_init(pass);
    }
    //move_print(&result);
    return result;    
}



/** IA super puissante_V2, fonctionne en donnant des 
 * priorités aux coups, et en optimisant le score, et en 
 * essayant de minimiser le gain adverse si ce gain est supérieur au propre gain du coup joué
 * @param game  la config du jeu
 * @param m     le type du mouvement
 * ROBOT Numero 5
 */
move_t play_robot_surpuissant_V2(config_t* game, move_kind_t m) {
    move_t result = move_init(m);
    cell_t type = (m==MOVE_PLAY_BLACK) ? CELL_BLACK: CELL_WHITE;
    cell_t type2 = (m==MOVE_PLAY_BLACK) ? CELL_WHITE: CELL_BLACK;
    move_kind_t pass = (m == MOVE_PLAY_BLACK) ? MOVE_PASS_BLACK : MOVE_PASS_WHITE;
    size_t size = game->board_size;

    config_t* copy= config_copy(game);
    mettre_les_suggestions(copy,m);
    
    int meilleur_score = 0;
    
    int linbest = size/2;
    int colbest = size/2;
    if (score(game,m)>1){
        for (int i=0; i<size; i++ ) {
            for (int j=0; j<size; j++ ) {
                cell_t cellule = board_get(copy->board,size,i,j);
                int distance = distance_bordure(i,j,size);
                int score_cellule = 0;
                if (cellule==CELL_SUG) {
                    if ( testCorner(i,j,size)) {
                        score_cellule = 10000;
                    }
                    else if (distance==0) {
                        score_cellule = 5000;
                        if(un_coin_colle(game,i,j,m)== true){
                            score_cellule+=2500;
                        }
                        else if(un_adv_colle_a_un_groupe_nous(game,i,j,m)==true){
                            score_cellule+=2000;
                        }
                        else if(bord_est_vide(game,i,j,m)==true){
                            score_cellule+=350;
                        }
                        else if(deja_un_groupe_adv_colle(game,i,j,m)==true){
                            score_cellule-=3500;
                        }
                        else if(deja_un_groupe_adv_pas_colle(game,i,j,m)==true){
                            score_cellule-=950;
                        }
                       else  if(encadrer_par_deux_pion_adv(game,i,j,m)==true){
                            score_cellule+=200;
                        }
                        else if(deja_un_groupe_nous_colle_danger(game,i,j,m)==true){
                            score_cellule-=2010;
                        }
                        else if(deja_un_groupe_nous_colle_cool(game,i,j,m)==true){
                            score_cellule+=100;
                        }
                        else if(deja_un_groupe_nous_pas_colle(game,i,j,m)==true){
                            score_cellule-=1010;
                        }
                        else if(carre_pas_fou(game,i,j)==true){
                            score_cellule-=1010;
                        }
                        else if(dernier_coup_de_la_ligne(game,i,j,m)==true){
                            score_cellule+=375;
                        }
                        else if(plus_que_deux_cases(game,i,j,m)==true){
                            score_cellule-=2010;
                        }
                    }
                    else if (distance==2) {
                        score_cellule = 4000;
                        if (Definitif(game,i,j,m)) {
                            score_cellule+=3500 ;
                        }
                    }
                    else if (distance==1 && !(case_interdite(i,j,size))) {
                        score_cellule = 3000;
                        if (Definitif(game,i,j,m)) {
                            score_cellule+=4500;
                        }
                    }
                    else if (case_interdite(i,j,size)) {
                        score_cellule = 1000;
                        if (Definitif(game,i,j,m)) {
                            score_cellule+= 6500;
                        }
                    }
                    else { 
                        score_cellule = 2000; 
                        if (Definitif(game,i,j,m)) {
                            score_cellule+=5500;
                        }
                    }

                    int score_case = 0;
                    for (int dC=-1; dC<=1; dC++){
                        for(int dL=-1; dL<=1; dL++){
                            if (!(dL==0 && dC==0)) {
                                score_case += point_par_coup(game,i,j,type,dL,dC);
                            }
                        }
                    score_cellule += score_case;
                    int score_adverse=0;
                    config_t* copy2 = config_copy(game);
                    move_t move;
                    move.kind=m;
                    move.lin=i;
                    move.col=j;
                    move_do(copy2,&move);
                    score_adverse = Prevision_Score_Adverse(copy2,type2);
                    if (score_adverse<score_cellule) {score_cellule+=20000 ;}
                    config_free(copy2);
                    }
                }
                if (score_cellule>meilleur_score) {
                    meilleur_score = score_cellule;
                    linbest = i;
                    colbest = j;
                }

            }
        }
    }
    else {
        for (int i=0; i<size; i++ ) {
            for (int j=0; j<size; j++ ) {
                cell_t cellule = board_get(copy->board,size,i,j);
                int score_cellule = 0;
                if (cellule==CELL_SUG) {
                    for (int dC=-1; dC<=1; dC++){
                        for(int dL=-1; dL<=1; dL++){
                            if (!(dL==0 && dC==0)) {
                                score_cellule += point_par_coup(game,i,j,type,dL,dC);
                            }
                        }
                    }
                }
                if (score_cellule>meilleur_score) {
                    meilleur_score = score_cellule;
                    linbest = i;
                    colbest = j;
                }
            }
        }
    }
    result.lin = linbest;
    result.col = colbest;
    config_free(copy);
    if (meilleur_score==0) {
        result.kind = pass;
        //move_print(&result);    
        return move_init(pass);
    }
    //move_print(&result);
    return result;    
}


typedef struct node
{   
    int ligne;
    int colonne;
    unsigned int key;
    struct node *un;
    struct node *deux;
    struct node *trois;
    struct node *quatre;
    struct node *cinq;
    struct node *six;
    struct node *sept;
    struct node *huit;
    struct node *neuf;
    struct node *dix;
    struct node *onze;
    struct node *douze;
    struct node *treize;
} node ;

node *Arbre = NULL;
//node *valeur = NULL;

//print un arbre
void printTree(node *tree)
{
    if(!tree) return;

    if(tree->un)  printTree(tree->un);

    printf("Cle = %d\n", tree->key);

    if(tree->deux)  printTree(tree->deux);

    if(tree->trois)  printTree(tree->trois);

    if(tree->quatre)  printTree(tree->quatre);

    if(tree->cinq)  printTree(tree->cinq);

    if(tree->six)  printTree(tree->six);

    if(tree->sept)  printTree(tree->sept);

    if(tree->huit)  printTree(tree->huit);

    if(tree->neuf)  printTree(tree->neuf);

    if(tree->dix)  printTree(tree->dix);

    if(tree->onze)  printTree(tree->onze);

    if(tree->douze)  printTree(tree->douze);

    if(tree->treize)  printTree(tree->treize);
}

//add un noeud à un arbre
int addNode(node **tree, unsigned int key, int ligne, int colonne){
    node *tmpTree = *tree;

    node *elem = malloc(sizeof(node));
    elem->ligne = ligne;
    elem->colonne = colonne;
    elem->key = key;
    elem->un = NULL;
    elem->deux = NULL;
    elem->trois = NULL;
    elem->quatre = NULL;
    elem->cinq = NULL;
    elem->six = NULL;
    elem->sept = NULL;
    elem->huit = NULL;
    elem->neuf = NULL;
    elem->dix = NULL;
    elem->onze = NULL;
    elem->douze = NULL;
    elem->treize = NULL;

    if(tmpTree){
        if(tmpTree->treize){
            return 100;
        }

        else if(tmpTree->douze){
            tmpTree->treize = elem;
            return 13;
        }

        else if(tmpTree->onze){
            tmpTree->douze = elem;
            return 12;
        }

        else if(tmpTree->dix){
            tmpTree->onze = elem;
            return 11;
        }

        else if(tmpTree->neuf){
            tmpTree->dix = elem;
            return 10;
        }

        else if(tmpTree->huit){
            tmpTree->neuf = elem;
            return 9;
        }

        else if(tmpTree->sept){
            tmpTree->huit = elem;
            return 8;
        }

        else if(tmpTree->six){
            tmpTree->sept = elem;
            return 7;
        }

        else if(tmpTree->cinq){
            tmpTree->six = elem;
            return 6;
        }

        else if(tmpTree->quatre){
            tmpTree->cinq = elem;
            return 5;
        }

        else if(tmpTree->trois){
            tmpTree->quatre = elem;
            return 4;
        }

        else if(tmpTree->deux){
            tmpTree->trois = elem;
            return 3;
        }

        else if(tmpTree->un){
            tmpTree->deux = elem;
            return 2;
        }

        else if(tmpTree){
            tmpTree->un = elem;
            return 1;
        }
    }
    else { 
        *tree = elem;
        return 0;
    } 
    return 0;
}

//max entre deux noeud
node* maximum(node* a, node *b){
    int v = b->key;
    int u = a->key;

    if (b){
        //printf("\nmaximum :\n");
        //printf("%d\n",b->key);
        //printf("%d\n",a->key);
        //bool p = ((v) >= (u));
        //printf("%d\n",p);

        if(v >= u){
            return b;
        }
        else{
            return a;
        }
        
    }
    else
    {
        return a;
    }
    
    
}

//min entre deux noeud
node* minimum(node* a, node *b){
    //printf("\nminimum :\n");
    //printf("%d\n",b->key);
    //printf("%d\n",a->key);
    int v = b->key;
    int u = a->key;

    if (b){
        if(v <= u){
            return b;
        }
        else{
            return a;
        }
    }
    else
    {
        return a;
    }
}

//algorithme minmax
node* minmax(int depth,bool maximizingPlayer, node *noued){
    
    //printf("1\n");
    if(depth == 0){
        //printf("plllllssss\n");
        return noued;
    }

    node* value = NULL;
    

    if(maximizingPlayer){
        addNode(&value,-1000,0,0);
        for (int i = 1; i <= 13; i++){
            if(i == 1 && noued->un){
                value = maximum(value,minmax(depth-1,false, noued->un));
                //printf("2\n");
            }
            if(i == 2 && noued->deux){
                value = maximum(value,minmax(depth-1,false, noued->deux));
                //printf("2\n");
            }
            if(i == 3 && noued->trois){
                value = maximum(value,minmax(depth-1,false, noued->trois));
                //printf("2\n");
            }
            if(i == 4 && noued->quatre){
                value = maximum(value,minmax(depth-1,false, noued->quatre));
                //printf("2\n");
            }
            if(i == 5 && noued->cinq){
                value = maximum(value,minmax(depth-1,false, noued->cinq));
                //printf("2\n");
            }
            if(i == 6 && noued->six){
                value = maximum(value,minmax(depth-1,false, noued->six));
                //printf("2\n");
            }
            if(i == 7 && noued->sept){
                value = maximum(value,minmax(depth-1,false, noued->sept));
                //printf("2\n");
            }
            if(i == 8 && noued->huit){
                value = maximum(value,minmax(depth-1,false, noued->huit));
                //printf("2\n");
            }
            if(i == 9 && noued->neuf){
                value = maximum(value,minmax(depth-1,false, noued->neuf));
                //printf("2\n");
            }
            if(i == 10 && noued->dix){
                value = maximum(value,minmax(depth-1,false, noued->dix));
                //printf("2\n");
            }
            if(i == 11 && noued->onze){
                value = maximum(value,minmax(depth-1,false, noued->onze));
                //printf("2\n");
            }
            if(i == 12 && noued->douze){
                value = maximum(value,minmax(depth-1,false, noued->douze));
                //printf("2\n");
            }
            if(i == 13 && noued->treize){
                value = maximum(value,minmax(depth-1,false, noued->treize));
                //printf("2\n");
            }
        }
    }
    else{
        addNode(&value,1000,0,0);
        for (int i = 1; i <= 13; i++){
            if(i == 1 && noued->un){
                value = minimum(value,minmax(depth-1,true, noued->un));
                //printf("3\n");
            }
            if(i == 2 && noued->deux){
                value = minimum(value,minmax(depth-1,true, noued->deux));
                //printf("3\n");
            }
            if(i == 3 && noued->trois){
                value = minimum(value,minmax(depth-1,true, noued->trois));
                //printf("3\n");
            }
            if(i == 4 && noued->quatre){
                value = minimum(value,minmax(depth-1,true, noued->quatre));
                //printf("3\n");
            }
            if(i == 5 && noued->cinq){
                value = minimum(value,minmax(depth-1,true, noued->cinq));
                //printf("3\n");
            }
            if(i == 6 && noued->six){
                value = minimum(value,minmax(depth-1,true, noued->six));
                //printf("3\n");
            }
            if(i == 7 && noued->sept){
                value = minimum(value,minmax(depth-1,true, noued->sept));
                //printf("3\n");
            }
            if(i == 8 && noued->huit){
                value = minimum(value,minmax(depth-1,true, noued->huit));
                //printf("3\n");
            }
            if(i == 9 && noued->neuf){
                value = minimum(value,minmax(depth-1,true, noued->neuf));
                //printf("3\n");
            }
            if(i == 10 && noued->dix){
                value = minimum(value,minmax(depth-1,true, noued->dix));
                //printf("3\n");
            }
            if(i == 11 && noued->onze){
                value = minimum(value,minmax(depth-1,true, noued->onze));
                //printf("3\n");
            }
            if(i == 12 && noued->douze){
                value = minimum(value,minmax(depth-1,true, noued->douze));
                //printf("3\n");
            }
            if(i == 13 && noued->treize){
                value = minimum(value,minmax(depth-1,true, noued->treize));
                //printf("3\n");
            }
        }
    }
    //printf("%d\n",value->key);
    //printf("%d\n",value->ligne);
    //printf("%d\n",value->colonne);
    //valeur = value;
    return value;
}

//evalue la valeur d un coup en fonction de nombreux paramètres
unsigned int eval_score(config_t* game, move_kind_t m, int i, int j){
    cell_t type = (m==MOVE_PLAY_BLACK) ? CELL_BLACK: CELL_WHITE;
    size_t size = game->board_size;

    
    int score_cellule = 0;

    //score fait par le coup
    int score_case = 0;
    for (int dC=-1; dC<=1; dC++){
        for(int dL=-1; dL<=1; dL++){
            if (!(dL==0 && dC==0)) {
                score_case += point_par_coup(game,i,j,type,dL,dC);
            }
        }
    }
    score_cellule += score_case;
    
    if ( testCorner(i,j,size)) {
        score_cellule +=500;
    }

    if(carre_pas_fou(game,i,j)){
        score_cellule-=1000;
    }

    if(dernier_coup_de_la_ligne(game,i,j,m)){
        score_cellule+=1000;
    }

    if(un_coin_colle(game,i,j,m)){
        score_cellule+=500;
    }
    
    if(un_adv_colle_a_un_groupe_nous(game,i,j,m)){
        score_cellule+=200;
    }

    if(bord_est_vide(game,i,j,m)){
        score_cellule+=300;
    }

    if(deja_un_groupe_adv_colle(game,i,j,m)){
        score_cellule-=300;
    }
    
    if(deja_un_groupe_adv_pas_colle(game,i,j,m)){
        score_cellule-=300;
    }
    
    if(encadrer_par_deux_pion_adv(game,i,j,m)){
        score_cellule+=500;
    }
    
    if(deja_un_groupe_nous_colle_danger(game,i,j,m)){
        score_cellule-=300;
    }

    if(deja_un_groupe_nous_colle_cool(game,i,j,m)){
        score_cellule+=300;
    }
    
    if(deja_un_groupe_nous_pas_colle(game,i,j,m)){
        score_cellule-=200;
    }

    if(plus_que_deux_cases(game,i,j,m)){
        score_cellule-=100;
    }
    
    if(Definitif(game,i,j,m)) {
        score_cellule+=500;
    }

    if (case_interdite(i,j,size)){
        score_cellule-=1000;
    }

    //calcul des libertés du coup
    int score_liberte = 0;
    for (int dC=-1; dC<=1; dC++){
        for(int dL=-1; dL<=1; dL++){
            if (!(dL==0 && dC==0)) {
                cell_t cellule = board_get(game->board,size,i + dL,j + dC);
                if ( cellule == CELL_FREE)
                {
                    score_liberte += 1;
                }
            }
        }
    }

    score_cellule -= score_liberte;

    return score_cellule;

}

//delete un arbre
void clearTree(node **tree)
{
    node *tmpTree = *tree;

    if(!tree) return;

    if(tmpTree->un)  clearTree(&tmpTree->un);

    if(tmpTree->deux) clearTree(&tmpTree->deux);

    if(tmpTree->trois) clearTree(&tmpTree->trois);

    if(tmpTree->quatre) clearTree(&tmpTree->quatre);

    if(tmpTree->cinq) clearTree(&tmpTree->cinq);

    if(tmpTree->six) clearTree(&tmpTree->six);

    if(tmpTree->sept) clearTree(&tmpTree->sept);

    if(tmpTree->huit) clearTree(&tmpTree->huit);

    if(tmpTree->neuf) clearTree(&tmpTree->neuf);

    if(tmpTree->dix) clearTree(&tmpTree->dix);

    if(tmpTree->onze) clearTree(&tmpTree->onze);

    if(tmpTree->douze) clearTree(&tmpTree->douze);

    if(tmpTree->treize) clearTree(&tmpTree->treize);

    free(tmpTree);      

    *tree = NULL;
}

//fonction maximum classique
int maxi(int a, int b){
    if (a > b)
    {   
        return a;
    }
    else
    {
        return b;
    }
}

//trouver un noeud dans l'arbre
int searchNode(node *tree, int key, int colonne, int ligne)
{
    int value = 0;
    if (tree){
        int k = tree->key;
        int l = tree->ligne;
        int c = tree->colonne;

        //printf("\narbre :\n");
        //printf("%d\n",key);
        //printf("%d\n",colonne);
        //printf("%d\n",ligne);

        //printf("\ntree :\n");
        //printf("%d\n",k);
        //printf("%d\n",c);
        //printf("%d\n",l);

        if(key == k && colonne == c && ligne == l) return 1;

        if(tree->un) value = maxi(value, searchNode(tree->un,key,colonne,ligne));
        if(tree->deux) value = maxi(value, searchNode(tree->deux,key,colonne,ligne));
        if(tree->trois) value = maxi(value,searchNode(tree->trois,key,colonne,ligne));
        if(tree->quatre) value = maxi(value,searchNode(tree->quatre,key,colonne,ligne));
        if(tree->cinq) value = maxi(value,searchNode(tree->cinq,key,colonne,ligne));
        if(tree->six) value = maxi(value,searchNode(tree->six,key,colonne,ligne));
        if(tree->sept) value = maxi(value,searchNode(tree->sept,key,colonne,ligne));
        if(tree->huit) value = maxi(value,searchNode(tree->huit,key,colonne,ligne));
        if(tree->neuf) value = maxi(value,searchNode(tree->neuf,key,colonne,ligne));
        if(tree->dix) value = maxi(value,searchNode(tree->dix,key,colonne,ligne));
        if(tree->onze) value = maxi(value,searchNode(tree->onze,key,colonne,ligne));
        if(tree->douze) value = maxi(value,searchNode(tree->douze,key,colonne,ligne));
        if(tree->treize) value = maxi(value,searchNode(tree->treize,key,colonne,ligne));
    }

    return value;
}

//compte le nomre de cell_free
int compte_cell_free(config_t* game,move_kind_t m){
    int compte = 0;
    size_t size = game->board_size;
    for (int i = 0; i < size; i++){
        for (int j=0; j<size; j++) {
            cell_t jeton = board_get(game->board,size,i,j);
            if (jeton == CELL_FREE)
            {
                compte+=1;
            }
            
        }
    }
    return compte;
    
}

//robot trop nul 
//Robot 7
move_t play_robot_minmax(config_t* game, move_kind_t m){    
    //printf("1\n");
    move_t result = move_init(m);
    move_kind_t pass = (m == MOVE_PLAY_BLACK) ? MOVE_PASS_BLACK : MOVE_PASS_WHITE;
    size_t size = game->board_size;
    if (compte_cell_free(game,m) > 2){
    
        config_t* copy1 = config_copy(game);
        mettre_les_suggestions(copy1,m);

        //printf("2a\n");
        addNode(&Arbre, 0, 0,0);
        //printf("3a\n");
        //ajout des coups possiles à l'arbre sur 3 de pronfondeurs
        for (int i=0; i<size; i++) {
        for (int j=0; j<size; j++) {
            //printf("4\n");
            cell_t jeton = board_get(copy1->board,size,i,j);
            //printf("5\n");
            if (jeton==CELL_SUG) {
                //printf("6\n");
                int num = addNode(&Arbre, 0, i,j);
                //printf("7\n");
                config_t* copy2 = config_copy(game);
                move_t move;
                move.kind=m;
                move.lin=i;
                move.col=j;
                move_do(copy2,&move);

                move_kind_t m2 = (m == MOVE_PLAY_BLACK) ? MOVE_PLAY_WHITE : MOVE_PLAY_BLACK;

                //printf("8\n");

                config_t* copy3 = config_copy(copy2);
                mettre_les_suggestions(copy3,m2);
                //board_print(copy3->board,size);
                int num2;

                for (int k=0; k<size; k++) {
                    for (int l=0; l<size; l++) {
                        cell_t jeton2 = board_get(copy3->board,size,k,l);
                        

                        if (jeton2 == CELL_SUG) {
                            //printf("9\n");
                            if (num == 1){
                                num2 = addNode(&Arbre->un, 0, k,l);
                                //printf("10\n");
                            }
                            if (num == 2){
                                num2 = addNode(&Arbre->deux, 0, k,l);
                                //printf("10\n");
                            }
                            if (num == 3){
                                num2 = addNode(&Arbre->trois, 0, k,l);
                                //printf("10\n");
                            }
                            if (num == 4){
                                num2 = addNode(&Arbre->quatre, 0, k,l);
                                //printf("10\n");
                            }
                            if (num == 5){
                                num2 = addNode(&Arbre->cinq, 0, k,l);
                                //printf("10\n");
                            }
                            if (num == 6){
                                num2 = addNode(&Arbre->six, 0, k,l);
                                //printf("10\n");
                            }
                            if (num == 7){
                                num2 = addNode(&Arbre->sept, 0, k,l);
                                //printf("10\n");
                            }
                            if (num == 8){
                                num2 = addNode(&Arbre->huit, 0, k,l);
                                //printf("10\n");
                            }
                            if (num == 9){
                                num2 = addNode(&Arbre->neuf, 0, k,l);
                                //printf("10\n");
                            }
                            if (num == 10){
                                num2 = addNode(&Arbre->dix, 0, k,l);
                                //printf("10\n");
                            }
                            if (num == 11){
                                num2 = addNode(&Arbre->onze, 0, k,l);
                                //printf("10\n");
                            }
                            if (num == 12){
                                num2 = addNode(&Arbre->douze, 0, k,l);
                                //printf("10\n");
                            }
                             if (num == 13){
                                num2 = addNode(&Arbre->treize, 0, k,l);
                                //printf("10\n");
                            }
                        

                            config_t* copy4 = config_copy(copy2);
                            move_t move2;
                            move2.kind=m2;
                            move2.lin=k;
                            move2.col=l;
                            move_do(copy4,&move2);

                            config_t* copy5 = config_copy(copy4);
                            mettre_les_suggestions(copy5,m);
                            //board_print(copy5->board,size);
                            for (int v=0; v<size; v++) {
                                for (int u=0; u<size; u++) {
                                    
                                    cell_t jeton3 =board_get(copy5->board,size,v,u);

                                    if (jeton3==CELL_SUG) {
                                        //arbre->un
                                        if (num == 1 && num2 == 1){
                                            addNode(&Arbre->un->un, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 1 && num2 == 2){
                                            addNode(&Arbre->un->deux, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 1 && num2 == 3){
                                            addNode(&Arbre->un->trois, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 1 && num2 == 4){
                                            addNode(&Arbre->un->quatre, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 1 && num2 == 5){
                                            addNode(&Arbre->un->cinq, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 1 && num2 == 6){
                                            addNode(&Arbre->un->six, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 1 && num2 == 7){
                                            addNode(&Arbre->un->sept, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 1 && num2 == 8){
                                            addNode(&Arbre->un->huit, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 1 && num2 == 9){
                                            addNode(&Arbre->un->neuf, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 1 && num2 == 10){
                                            addNode(&Arbre->un->dix, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 1 && num2 == 11){
                                            addNode(&Arbre->un->onze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 1 && num2 == 12){
                                            addNode(&Arbre->un->douze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 1 && num2 == 13){
                                            addNode(&Arbre->un->treize, eval_score(copy4, m, v,u), v,u);
                                        }

                                        //arbre->deux
                                        if (num == 2 && num2 == 1){
                                            addNode(&Arbre->deux->un, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 2 && num2 == 2){
                                            addNode(&Arbre->deux->deux, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 2 && num2 == 3){
                                            addNode(&Arbre->deux->trois, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 2 && num2 == 4){
                                            addNode(&Arbre->deux->quatre, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 2 && num2 == 5){
                                            addNode(&Arbre->deux->cinq, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 2 && num2 == 6){
                                            addNode(&Arbre->deux->six, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 2 && num2 == 7){
                                            addNode(&Arbre->deux->sept, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 2 && num2 == 8){
                                            addNode(&Arbre->deux->huit, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 2 && num2 == 9){
                                            addNode(&Arbre->deux->neuf, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 2 && num2 == 10){
                                            addNode(&Arbre->deux->dix, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 2 && num2 == 11){
                                            addNode(&Arbre->deux->onze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 2 && num2 == 12){
                                            addNode(&Arbre->deux->douze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 2 && num2 == 13){
                                            addNode(&Arbre->deux->treize, eval_score(copy4, m, v,u), v,u);
                                        }

                                        //arbre->trois
                                        if (num == 3 && num2 == 1){
                                            addNode(&Arbre->trois->un, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 3 && num2 == 2){
                                            addNode(&Arbre->trois->deux, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 3 && num2 == 3){
                                            addNode(&Arbre->trois->trois, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 3 && num2 == 4){
                                            addNode(&Arbre->trois->quatre, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 3 && num2 == 5){
                                            addNode(&Arbre->trois->cinq, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 3 && num2 == 6){
                                            addNode(&Arbre->trois->six, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 3 && num2 == 7){
                                            addNode(&Arbre->trois->sept, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 3 && num2 == 8){
                                            addNode(&Arbre->trois->huit, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 3 && num2 == 9){
                                            addNode(&Arbre->trois->neuf, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 3 && num2 == 10){
                                            addNode(&Arbre->trois->dix, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 3 && num2 == 11){
                                            addNode(&Arbre->trois->onze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 3 && num2 == 12){
                                            addNode(&Arbre->trois->douze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 3 && num2 == 13){
                                            addNode(&Arbre->trois->treize, eval_score(copy4, m, v,u), v,u);
                                        }

                                        //arbre->quatre
                                        if (num == 4 && num2 == 1){
                                            addNode(&Arbre->quatre->un, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 4 && num2 == 2){
                                            addNode(&Arbre->quatre->deux, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 4 && num2 == 3){
                                            addNode(&Arbre->quatre->trois, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 4 && num2 == 4){
                                            addNode(&Arbre->quatre->quatre, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 4 && num2 == 5){
                                            addNode(&Arbre->quatre->cinq, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 4 && num2 == 6){
                                            addNode(&Arbre->quatre->six, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 4 && num2 == 7){
                                            addNode(&Arbre->quatre->sept, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 4 && num2 == 8){
                                            addNode(&Arbre->quatre->huit, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 4 && num2 == 9){
                                            addNode(&Arbre->quatre->neuf, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 4 && num2 == 10){
                                            addNode(&Arbre->quatre->dix, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 4 && num2 == 11){
                                            addNode(&Arbre->quatre->onze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 4 && num2 == 12){
                                            addNode(&Arbre->quatre->douze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 4 && num2 == 13){
                                            addNode(&Arbre->quatre->treize, eval_score(copy4, m, v,u), v,u);
                                        }

                                        //arbre->cinq
                                        if (num == 5 && num2 == 1){
                                            addNode(&Arbre->cinq->un, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 5 && num2 == 2){
                                            addNode(&Arbre->cinq->deux, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 5 && num2 == 3){
                                            addNode(&Arbre->cinq->trois, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 5 && num2 == 4){
                                            addNode(&Arbre->cinq->quatre, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 5 && num2 == 5){
                                            addNode(&Arbre->cinq->cinq, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 5 && num2 == 6){
                                            addNode(&Arbre->cinq->six, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 5 && num2 == 7){
                                            addNode(&Arbre->cinq->sept, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 5 && num2 == 8){
                                            addNode(&Arbre->cinq->huit, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 5 && num2 == 9){
                                            addNode(&Arbre->cinq->neuf, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 5 && num2 == 10){
                                            addNode(&Arbre->cinq->dix, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 5 && num2 == 11){
                                            addNode(&Arbre->cinq->onze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 5 && num2 == 12){
                                            addNode(&Arbre->cinq->douze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 5 && num2 == 13){
                                            addNode(&Arbre->cinq->treize, eval_score(copy4, m, v,u), v,u);
                                        }

                                        //arbre->six
                                        if (num == 6 && num2 == 1){
                                            addNode(&Arbre->six->un, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 6 && num2 == 2){
                                            addNode(&Arbre->six->deux, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 6 && num2 == 3){
                                            addNode(&Arbre->six->trois, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 6 && num2 == 4){
                                            addNode(&Arbre->six->quatre, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 6 && num2 == 5){
                                            addNode(&Arbre->six->cinq, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 6 && num2 == 6){
                                            addNode(&Arbre->six->six, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 6 && num2 == 7){
                                            addNode(&Arbre->six->sept, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 6 && num2 == 8){
                                            addNode(&Arbre->six->huit, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 6 && num2 == 9){
                                            addNode(&Arbre->six->neuf, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 6 && num2 == 10){
                                            addNode(&Arbre->six->dix, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 6 && num2 == 11){
                                            addNode(&Arbre->six->onze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 6 && num2 == 12){
                                            addNode(&Arbre->six->douze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 6 && num2 == 13){
                                            addNode(&Arbre->six->treize, eval_score(copy4, m, v,u), v,u);
                                        }

                                        //arbre->sept
                                        if (num == 7 && num2 == 1){
                                            addNode(&Arbre->sept->un, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 7 && num2 == 2){
                                            addNode(&Arbre->sept->deux, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 7 && num2 == 3){
                                            addNode(&Arbre->sept->trois, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 7 && num2 == 4){
                                            addNode(&Arbre->sept->quatre, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 7 && num2 == 5){
                                            addNode(&Arbre->sept->cinq, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 7 && num2 == 6){
                                            addNode(&Arbre->sept->six, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 7 && num2 == 7){
                                            addNode(&Arbre->sept->sept, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 7 && num2 == 8){
                                            addNode(&Arbre->sept->huit, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 7 && num2 == 9){
                                            addNode(&Arbre->sept->neuf, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 7 && num2 == 10){
                                            addNode(&Arbre->sept->dix, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 7 && num2 == 11){
                                            addNode(&Arbre->sept->onze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 7 && num2 == 12){
                                            addNode(&Arbre->sept->douze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 7 && num2 == 13){
                                            addNode(&Arbre->sept->treize, eval_score(copy4, m, v,u), v,u);
                                        }
                                        
                                        //arbre->huit
                                        if (num == 8 && num2 == 1){
                                            addNode(&Arbre->huit->un, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 8 && num2 == 2){
                                            addNode(&Arbre->huit->deux, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 8 && num2 == 3){
                                            addNode(&Arbre->huit->trois, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 8 && num2 == 4){
                                            addNode(&Arbre->huit->quatre, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 8 && num2 == 5){
                                            addNode(&Arbre->huit->cinq, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 8 && num2 == 6){
                                            addNode(&Arbre->huit->six, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 8 && num2 == 7){
                                            addNode(&Arbre->huit->sept, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 8 && num2 == 8){
                                            addNode(&Arbre->huit->huit, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 8 && num2 == 9){
                                            addNode(&Arbre->huit->neuf, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 8 && num2 == 10){
                                            addNode(&Arbre->huit->dix, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 8 && num2 == 11){
                                            addNode(&Arbre->huit->onze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 8 && num2 == 12){
                                            addNode(&Arbre->huit->douze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 8 && num2 == 13){
                                            addNode(&Arbre->huit->treize, eval_score(copy4, m, v,u), v,u);
                                        }

                                        //arbre->neuf
                                        if (num == 9 && num2 == 1){
                                            addNode(&Arbre->neuf->un, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 9 && num2 == 2){
                                            addNode(&Arbre->neuf->deux, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 9 && num2 == 3){
                                            addNode(&Arbre->neuf->trois, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 9 && num2 == 4){
                                            addNode(&Arbre->neuf->quatre, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 9 && num2 == 5){
                                            addNode(&Arbre->neuf->cinq, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 9 && num2 == 6){
                                            addNode(&Arbre->neuf->six, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 9 && num2 == 7){
                                            addNode(&Arbre->neuf->sept, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 9 && num2 == 8){
                                            addNode(&Arbre->neuf->huit, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 9 && num2 == 9){
                                            addNode(&Arbre->neuf->neuf, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 9 && num2 == 10){
                                            addNode(&Arbre->neuf->dix, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 9 && num2 == 11){
                                            addNode(&Arbre->neuf->onze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 9 && num2 == 12){
                                            addNode(&Arbre->neuf->douze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 9 && num2 == 13){
                                            addNode(&Arbre->neuf->treize, eval_score(copy4, m, v,u), v,u);
                                        }

                                        //arbre->dix
                                        if (num == 10 && num2 == 1){
                                            addNode(&Arbre->dix->un, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 10 && num2 == 2){
                                            addNode(&Arbre->dix->deux, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 10 && num2 == 3){
                                            addNode(&Arbre->dix->trois, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 10 && num2 == 4){
                                            addNode(&Arbre->dix->quatre, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 10 && num2 == 5){
                                            addNode(&Arbre->dix->cinq, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 10 && num2 == 6){
                                            addNode(&Arbre->dix->six, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 10 && num2 == 7){
                                            addNode(&Arbre->dix->sept, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 10 && num2 == 8){
                                            addNode(&Arbre->dix->huit, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 10 && num2 == 9){
                                            addNode(&Arbre->dix->neuf, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 10 && num2 == 10){
                                            addNode(&Arbre->dix->dix, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 10 && num2 == 11){
                                            addNode(&Arbre->dix->onze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 10 && num2 == 12){
                                            addNode(&Arbre->dix->douze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 10 && num2 == 13){
                                            addNode(&Arbre->dix->treize, eval_score(copy4, m, v,u), v,u);
                                        }

                                        //arbre->onze
                                        if (num == 11 && num2 == 1){
                                            addNode(&Arbre->onze->un, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 11 && num2 == 2){
                                            addNode(&Arbre->onze->deux, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 11 && num2 == 3){
                                            addNode(&Arbre->onze->trois, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 11 && num2 == 4){
                                            addNode(&Arbre->onze->quatre, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 11 && num2 == 5){
                                            addNode(&Arbre->onze->cinq, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 11 && num2 == 6){
                                            addNode(&Arbre->onze->six, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 11 && num2 == 7){
                                            addNode(&Arbre->onze->sept, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 11 && num2 == 8){
                                            addNode(&Arbre->onze->huit, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 11 && num2 == 9){
                                            addNode(&Arbre->onze->neuf, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 11 && num2 == 10){
                                            addNode(&Arbre->onze->dix, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 11 && num2 == 11){
                                            addNode(&Arbre->onze->onze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 11 && num2 == 12){
                                            addNode(&Arbre->onze->douze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 11 && num2 == 13){
                                            addNode(&Arbre->onze->treize, eval_score(copy4, m, v,u), v,u);
                                        }

                                        //arbre->douze
                                        if (num == 12 && num2 == 1){
                                            addNode(&Arbre->douze->un, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 12 && num2 == 2){
                                            addNode(&Arbre->douze->deux, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 12 && num2 == 3){
                                            addNode(&Arbre->douze->trois, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 12 && num2 == 4){
                                            addNode(&Arbre->douze->quatre, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 12 && num2 == 5){
                                            addNode(&Arbre->douze->cinq, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 12 && num2 == 6){
                                            addNode(&Arbre->douze->six, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 12 && num2 == 7){
                                            addNode(&Arbre->douze->sept, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 12 && num2 == 8){
                                            addNode(&Arbre->douze->huit, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 12 && num2 == 9){
                                            addNode(&Arbre->douze->neuf, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 12 && num2 == 10){
                                            addNode(&Arbre->douze->dix, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 12 && num2 == 11){
                                            addNode(&Arbre->douze->onze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 12 && num2 == 12){
                                            addNode(&Arbre->douze->douze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 12 && num2 == 13){
                                            addNode(&Arbre->douze->treize, eval_score(copy4, m, v,u), v,u);
                                        }

                                        //arbre->treize
                                        if (num == 13 && num2 == 1){
                                            addNode(&Arbre->treize->un, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 13 && num2 == 2){
                                            addNode(&Arbre->treize->deux, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 13 && num2 == 3){
                                            addNode(&Arbre->treize->trois, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 13 && num2 == 4){
                                            addNode(&Arbre->treize->quatre, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 13 && num2 == 5){
                                            addNode(&Arbre->treize->cinq, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 13 && num2 == 6){
                                            addNode(&Arbre->treize->six, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 13 && num2 == 7){
                                            addNode(&Arbre->treize->sept, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 13 && num2 == 8){
                                            addNode(&Arbre->treize->huit, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 13 && num2 == 9){
                                            addNode(&Arbre->treize->neuf, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 13 && num2 == 10){
                                            addNode(&Arbre->treize->dix, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 13 && num2 == 11){
                                            addNode(&Arbre->treize->onze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 13 && num2 == 12){
                                            addNode(&Arbre->treize->douze, eval_score(copy4, m, v,u), v,u);
                                        }
                                        if (num == 13 && num2 == 13){
                                            addNode(&Arbre->treize->treize, eval_score(copy4, m, v,u), v,u);
                                        }
                                    }
                                }
                            }
                            config_free(copy5); 
                            config_free(copy4);          
                        }
                    }
                }
                config_free(copy2);
                config_free(copy3);
            }
        }
    }
        config_free(copy1);
        //printf("sortie1\n");
    
    
        //printTree(Arbre);

        //recherche du meilleur coup dans l'arbre
        //algorithme minmax

        node* Bestnoeud = malloc(sizeof(node));

        Bestnoeud = minmax(3,true,Arbre);
        //printf("\nBestnoeud : \n");
        printf("%d\n",Bestnoeud->ligne);
        printf("%d\n",Bestnoeud->colonne);
        printf("%d\n",Bestnoeud->key);
        int k = Bestnoeud->key;
        int c = Bestnoeud->colonne;
        int l = Bestnoeud->ligne;

        if (k == 1000 || k == -1000){
            return play_robot_surpuissant_V2(game,m);
        }
        
        //printf("sortie2\n");

        if (Bestnoeud){
        if (k == 0 && l == 0 && c == 0){
            result.kind = pass;
            move_print(&result); 
            //clearTree(&value);
            //printf("sortie3\n");
            clearTree(&Arbre);
            return move_init(pass);
        }
        else
        {   
            if (searchNode(Arbre->un,k,c,l) == 1){
                result.lin = Arbre->un->ligne;
                result.col= Arbre->un->colonne;
                move_print(&result);
                clearTree(&Arbre);
                return result; 
            }

            if (searchNode(Arbre->deux,k,c,l) == 1){
                result.lin = Arbre->deux->ligne;
                result.col= Arbre->deux->colonne;
                move_print(&result);
                clearTree(&Arbre);
                return result; 
            }
            
            if (searchNode(Arbre->trois,k,c,l) == 1){
                result.lin = Arbre->trois->ligne;
                result.col= Arbre->trois->colonne;
                move_print(&result);
                clearTree(&Arbre);
                return result; 
            }

            if (searchNode(Arbre->quatre,k,c,l) == 1){
                result.lin = Arbre->quatre->ligne;
                result.col= Arbre->quatre->colonne;
                move_print(&result);
                clearTree(&Arbre);
                return result; 
            }

            if (searchNode(Arbre->cinq,k,c,l) == 1){
                result.lin = Arbre->cinq->ligne;
                result.col= Arbre->cinq->colonne;
                move_print(&result);
                clearTree(&Arbre);
                return result; 
            }

            if (searchNode(Arbre->six,k,c,l) == 1){
                result.lin = Arbre->six->ligne;
                result.col= Arbre->six->colonne;
                move_print(&result);
                clearTree(&Arbre);
                return result; 
            }

            if (searchNode(Arbre->sept,k,c,l) == 1){
                result.lin = Arbre->sept->ligne;
                result.col= Arbre->sept->colonne;
                move_print(&result);
                clearTree(&Arbre);
                return result; 
            }

            if (searchNode(Arbre->huit,k,c,l) == 1){
                result.lin = Arbre->huit->ligne;
                result.col= Arbre->huit->colonne;
                move_print(&result);
                clearTree(&Arbre);
                return result; 
            }

            if (searchNode(Arbre->neuf,k,c,l) == 1){
                result.lin = Arbre->neuf->ligne;
                result.col= Arbre->neuf->colonne;
                move_print(&result);
                clearTree(&Arbre);
                return result; 
            }

            if (searchNode(Arbre->dix,k,c,l) == 1){
                result.lin = Arbre->dix->ligne;
                result.col= Arbre->dix->colonne;
                move_print(&result);
                clearTree(&Arbre);
                return result; 
            }

            if (searchNode(Arbre->onze,k,c,l) == 1){
                result.lin = Arbre->onze->ligne;
                result.col= Arbre->onze->colonne;
                move_print(&result);
                clearTree(&Arbre);
                return result; 
            }

            if (searchNode(Arbre->douze,k,c,l) == 1){
                result.lin = Arbre->douze->ligne;
                result.col= Arbre->douze->colonne;
                move_print(&result);
                clearTree(&Arbre);
                return result; 
            }

            if (searchNode(Arbre->treize,k,c,l) == 1){
                result.lin = Arbre->treize->ligne;
                result.col= Arbre->treize->colonne;
                move_print(&result);
                clearTree(&Arbre);
                return result; 
            }

            result.kind = pass;
            move_print(&result);
            clearTree(&Arbre);
            return move_init(pass);
        } 
        } else {
            result.kind = pass;
            move_print(&result);
            //clearTree(&value);
            //printf("sortie3\n"); 
            clearTree(&Arbre);
            return move_init(pass);
        }
    }
    else
    {
        return play_robot_minScoreAdverse(game,m);
    }
    
} 

/*
Faire les coups possibles et les evaluer
Ensuite créer un autre plateau et faire un coup puis calculer les coups suivant et les mettres dasn l'arbre
Ensuite recommencer une autre fois

Une fois l'arbre de coup possible obtenu, faire un algorithme minmax pour calculer le meilleur coup -> 
    le joueur 1 choisi la haute evaluation
    le joueur 2 choisi la plus petite
    le joueur 2 choisi la plus haute


ensuite rechercher dans l arbre le coup otenu pour recup le coup à jouer

L'évaluation : - le nombre de pion 
               - ( le nombre de liberté si j ai la fois)
               - sa place sur le plateau ( coin / bord / carré de neuf / reste)

delete l'arbre à la fin
*/





/**
 * Interface pour le joueur Noir
 * @param config   situation du jeu
 * @param pB       type de joueur pour Noir
 * @return         le coup joue
 */
move_t play_black(config_t* game, player_t pB, int numero) {
    if (pB == PLAYER_HUMAN){
        return play_human(game, MOVE_PLAY_BLACK);
    }
    else{
        switch (numero) {
        case 1:
        {
            return play_robot_opt(game, MOVE_PLAY_BLACK);
        }
        case 2:
        {
            return play_robot_corner(game, MOVE_PLAY_BLACK);
        }
        case 3:
        {
            return play_robot_minScoreAdverse(game, MOVE_PLAY_BLACK);
        }
        case 4:
        {
            return play_robot_surpuissant(game, MOVE_PLAY_BLACK);
        }
        case 5:
        {
            return play_robot_surpuissant_V2(game, MOVE_PLAY_BLACK);
        }
        case 6:
        {
            return play_robot_rand(game, MOVE_PLAY_BLACK);
        }
        case 7:
        {
            return play_robot_minmax(game, MOVE_PLAY_BLACK);
        }
        default:
            return play_robot_opt(game, MOVE_PLAY_BLACK);
        }
    }
}

/**
 * Interface pour le joueur Blanc
 * @param config   situation du jeu
 * @param pW       type de joueur pour Blanc
 * @return         le coup joue
 */
move_t play_white(config_t* game, player_t pW, int numero) {
    if (pW == PLAYER_HUMAN){
        return play_human(game, MOVE_PLAY_WHITE);
    }
    else {
        switch (numero) {
        case 1:
        {
            return play_robot_opt(game, MOVE_PLAY_WHITE);
        }
        case 2:
        {
            return play_robot_corner(game, MOVE_PLAY_WHITE);
        }
        case 3:
        {
            return play_robot_minScoreAdverse(game, MOVE_PLAY_WHITE);
        }
        case 4:
        {
            return play_robot_surpuissant(game, MOVE_PLAY_WHITE);
        }
        case 5:
        {
            return play_robot_surpuissant_V2(game, MOVE_PLAY_WHITE);
        }
        case 6:
        {
            return play_robot_rand(game, MOVE_PLAY_WHITE);
        }
        case 7:
        {
            return play_robot_minmax(game, MOVE_PLAY_WHITE);
        }
        default:
            return play_robot_opt(game, MOVE_PLAY_WHITE);
        }
    }
}





