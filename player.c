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
#include <time.h>

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
 * tactique: tente de se rapprocher le plus possible des coins
 * @param config   situation du jeu
 * @param m        qui doit jouer
 * @return         le coup joue
 * ROBOT Numero 2
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
                printf("r = %d",r);
                if (rmax < r) {
                    printf("   i:%d,  j:%d,  rmax:%d",i,j,rmax); 
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
        default:
            return play_robot_opt(game, MOVE_PLAY_WHITE);
        }
    }
}





