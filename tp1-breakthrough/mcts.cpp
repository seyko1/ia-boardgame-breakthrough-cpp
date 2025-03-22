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
#define NB_ITER 1000

bt_t B;
int board_width = 0;
int board_height = 0;
bool white_turn = true;

std::unordered_map<std::string, std::pair<int, int>> hashmap;

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

double uct(
  const std::string &state_hash,
  const std::string &new_state_hash
) {
  int wins              = hashmap[new_state_hash].first;
  int nb_playouts       = hashmap[new_state_hash].second;
  int nb_parent_layouts = hashmap[state_hash].second;

  return ((double)wins / nb_playouts) + 0.4 * sqrt(log(nb_parent_layouts) / nb_playouts);
}

bt_t selection(
  bt_t &state,
  const std::string &state_hash,
  bool is_white
) {
  if (state.endgame()) return state;

  std::vector<bt_move_t> moves = nextMoves(state);

  double max_uct = -1;
  bt_t best;

  std::string new_state_hash;
  for (bt_move_t move : moves) {
    bt_t new_state = applyMove(state, move);

    new_state_hash = new_state.board_to_string(is_white);

    if (hashmap.find(new_state_hash) == hashmap.end()) {
      hashmap[new_state_hash] = {0, 0}; // initialiser new_state dans la hashmap ?
      return new_state;
    }

    double new_uct = uct(state_hash, new_state_hash);

    if (new_uct > max_uct) {
      max_uct = new_uct;
      best = new_state;
    }
  }
  return selection(best, new_state_hash, is_white);
}

void backpropagate(const std::string &state_hash, int score) {
  // if (parent(state_hash)) return;
  // hashmap[state_hash].second += 1;
  hashmap[state_hash].first += score;
  
  // TODO: récupérer l'état parent et lui propager ..
}

int playout(bt_t state, bool is_white) {
    while (!state.endgame()) {
        bt_move_t move = state.get_rand_move();
        state = applyMove(state, move);
    }

    int result = state.endgame();

    if (result == WHITE && is_white) return 1;
    if (result == BLACK && !is_white) return 1;
    return -1;
}

bt_move_t mcts(bt_t &state, bool is_white) {
  std::string state_hash = state.board_to_string(is_white);
  hashmap[state_hash] = {0, 0};

  for (int i = 0; i < NB_ITER; ++i) {
    bt_t new_state = selection(state, state_hash, is_white);
    std::string new_state_hash = new_state.board_to_string(is_white);

    int score = playout(new_state, is_white);
    backpropagate(new_state_hash, score);
  }
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
