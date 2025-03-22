#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include "mybt.h"

// 1 si le premier coup de chaque joueur doit être aléatoire, 0 sinon.
#define FIRST_MOVE_RANDOM 1
#define PLAYER_NAME "mcts"

bt_t B;
int board_width = 0;
int board_height = 0;
bool white_turn = true;

std::unordered_map<std::string, int> hashmap;

#ifndef VERBOSE_MCTS_PLAYER
#define VERBOSE_MCTS_PLAYER
bool verbose = false;
bool debug   = false;
bool showboard_at_each_move = false;
#endif

void displayHelp() {
  fprintf(stderr, "  quit\n");
  fprintf(stderr, "  help\n");
  fprintf(stderr, "  debug ON | OFF\n");
  fprintf(stderr, "  verbose ON | OFF\n");
  fprintf(stderr, "  name\n");
  fprintf(stderr, "  newgame <NBCOL> <NBLINE>\n");
  fprintf(stderr, "  genmove\n");
  fprintf(stderr, "  play <L0C0L1C1>\n");
  fprintf(stderr, "  showboard\n");
}

void displayName() {
  printf("= %s\n\n", PLAYER_NAME);
}

void displayBoard() {
  B.print_board(stderr);
  printf("= \n\n");
}

void startNewGame() {
  if ((board_height < 1 || board_height > 10) && (board_width < 1 || board_width > 10)) {
    fprintf(stderr, "boardsize is %d %d ???\n", board_height, board_width);
    printf("= \n\n");
    return;
  }

  B.init(board_height, board_width);
  white_turn = true;

  if (verbose) fprintf(stderr, "ready to play on %dx%d board\n", board_height, board_width);

  printf("= \n\n");
}

// Renvoie la liste des coups possibles dans la configuration de plateau donnée.
std::vector<bt_move_t> nextMoves(bt_t &state) {
  state.update_moves();
  std::vector<bt_move_t> moves(state.moves, state.moves + state.nb_moves);
  return moves;
}

// Applique un coup donné et retourne le nouvel état du plateau.
bt_t applyMove(const bt_t &state, const bt_move_t &move) {
  bt_t new_state = state;
  new_state.play(move);
  return new_state;
}

void printMove(bt_move_t move, int depth) {
  char a, b, c, d;

  a = '0' + (board_height - move.line_i);
  b = 'a' + move.col_i;
  c = '0' + (board_height - move.line_f);
  d = 'a' + move.col_f;

  std::string str = std::string(1, a) +
                    std::string(1, b) +
                    std::string(1, c) +
                    std::string(1, d);

  for (int i = 0; i < depth; i++) fprintf(stderr, "\t");
  fprintf(stderr, "(%d)%s\n", depth, str.c_str());
}

void generateMove() {
  int ret = B.endgame();

  if (ret != EMPTY) {
    fprintf(stderr, "game finished\n");
    if (ret == WHITE) {
      fprintf(stderr, "white player wins\n");
    } else {
      fprintf(stderr, "black player wins\n");
    }
    printf("= \n\n");
    return;
  }

  bt_move_t move = B.get_rand_move();
  B.play(move);

  if (verbose) {
    move.print(stderr, white_turn, B.nbl);
    fprintf(stderr, "\n");
  }

  white_turn = !white_turn;
  printf("= %s\n\n", move.tostr(B.nbl).c_str());

  if (showboard_at_each_move) displayBoard();
}

// Joue un coup selon les positions de départ et d'arrivée données.
void playMove(char a, char b, char c, char d) {
  bt_move_t m;

  m.line_i = board_height-(a-'0');
  m.col_i  = b-'a';
  m.line_f = board_height-(c-'0');
  m.col_f  = d-'a';

  if (B.can_play(m)) {
    B.play(m);
    if(verbose) {
      m.print(stderr, white_turn, B.nbl);
      fprintf(stderr, "\n");
    }
    white_turn = !white_turn;
  } else {
    fprintf(stderr, "CANT play %d %d %d %d ?\n", m.line_i, m.col_i, m.line_f, m.col_f);
  }

  if (showboard_at_each_move) displayBoard();
  printf("= \n\n");
}

int main(int _ac, char** _av) {
  setbuf(stdout, 0);
  setbuf(stderr, 0);

  if (verbose) fprintf(stderr, "%s started\n", PLAYER_NAME);
  char a,b,c,d;
  
  for (std::string line; std::getline(std::cin, line);) {
    if (verbose)
      fprintf(stderr, "%s receive %s\n", PLAYER_NAME, line.c_str());

    if (line == "quit") {

      printf("= \n\n");
      break;
    } else if (line == "debug ON") {
      debug = true;
    } else if (line == "debug OFF") {
      debug = false;
    } else if (line == "verbose ON") {
      verbose = true;
    } else if (line == "verbose OFF") {
      verbose = false;
    } else if (line == "help") {
      displayHelp();
    } else if (line == "name") {
      displayName();
    } else if (sscanf(line.c_str(), "newgame %d %d", &board_height, &board_width) == 2) {
      startNewGame();
    } else if (line == "genmove") {
      generateMove();
    } else if (sscanf(line.c_str(), "play %c%c%c%c", &a, &b, &c, &d) == 4) {
      playMove(a, b, c, d);
    } else if (line == "showboard") {
      displayBoard();
    } else if (line.compare(0, 2, "//") == 0) {
      // Commentaires
    } else {
      fprintf(stderr, "Unknown command: %s\n", line.c_str());
    }
  }

  if (verbose) fprintf(stderr, "bye.\n");

  return 0;
}
