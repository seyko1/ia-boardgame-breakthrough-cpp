#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <string>
#include "mybt.h"

int verbose_level = 1;

int main(int _ac, char** _av) {
  setbuf(stdout, 0);
  setbuf(stderr, 0);

  if (_ac!=4) {
    fprintf(stderr, "usage: %s BOARD PLAYER_TURN TIME\n", _av[0]);
    return 0;    
  }

  if (verbose_level>=2)
    fprintf(stderr, "received str_board %s\n", _av[1]);

  int boardwidth = 0;
  int boardheight = 0;

  if (strlen(_av[1])==18) {
    boardwidth = 3; boardheight = 6;
  }

  if (strlen(_av[1])==60) {
    boardwidth = 10; boardheight = 6;
  }

  if (boardwidth==0) {
    fprintf(stderr, "usage: %s BOARD PLAYER_TURN TIME\n", _av[0]);
    fprintf(stderr, "where BOARD is 18 or 60 char long\n");
    return 0;    
  }

  bool white_turn = true;
  
  if (_av[2][0]=='@') white_turn = false;

  int genmove_time = atoi(_av[3]);

  if (genmove_time < 0) {
    fprintf(stderr, "ERROR genmove_time < 0\n");
    return 0;    
  }

  bt_t B;
  B.clear(boardheight,boardwidth);

  for (int i = 0; i < boardheight; i++)
    for (int j = 0; j < boardwidth; j++) {
      int k = i*boardwidth+j;

      if (_av[1][k]=='o')
        B.board[i][j] = WHITE;
      else if (_av[1][k]=='@')
        B.board[i][j] = BLACK;
      else
        B.board[i][j] = EMPTY;
    }

  B.init_pieces();

  if (white_turn)
    B.update_moves(WHITE);
  else
    B.update_moves(BLACK);

  if (verbose_level>=1) {
    fprintf(stderr, "rand_player_cmd started\n");
    B.print_board();
  }

  if (verbose_level>=2) {
    B.print_turn_and_moves();
  }

  if (B.endgame()!=EMPTY) {
    fprintf(stderr, "ERROR endgame position \n");
    return 0;    
  }

  bt_move_t m;

  if (white_turn)
    m = B.get_rand_move(WHITE);
  else
    m = B.get_rand_move(BLACK);  

  if (verbose_level>=1) {
    m.print(stderr, white_turn, 6);
    fprintf(stderr, "\n");
  }

  printf("%s\n", m.tostr(6).c_str()); // imprime tjrs en n&b les décisions
  return 0;
}

// exemple de ligne de commande
// ./rand_player_cmd @@@@@@......oooooo o 2
// ./rand_player_cmd @@@@@@......oooooo @ 2
// ./rand_player_cmd @@@@@@@@@@@@@@@@@@@@....................oooooooooooooooooooo o 2
// ./rand_player_cmd @@@@@@@@@@@@@@@@@@@@....................oooooooooooooooooooo @ 2

// pour vérifier le retour sur stdout et l'absence de confusions entre message d'info/suivi/debug et la réponse du programme
// ./rand_player_cmd @@@@@@......oooooo o 2 2>/dev/null
