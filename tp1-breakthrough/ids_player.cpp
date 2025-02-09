#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include "mybt.h"

#define IDS_MAX_DEPTH 5


bt_t B;
int boardwidth = 0;
int boardheight = 0;
bool white_turn = true;
int DLS_MAX_DEPTH = 5;

// Table de hashage pour stocker les profondeurs des états explorés
std::unordered_map<std::string, int> hashmap;

// Tableau pour stocker les coups par profondeur
std::vector<bt_move_t> solution;
int solution_size = 0;

bt_t best_solution;
bool solved = false;

#ifndef VERBOSE_IDS_PLAYER
#define VERBOSE_IDS_PLAYER
bool verbose = true;
bool showboard_at_each_move = false;
#endif

void help() {
  fprintf(stderr, "  quit\n");
  fprintf(stderr, "  echo ON | OFF\n");
  fprintf(stderr, "  help\n");
  fprintf(stderr, "  name <PLAYER_NAME>\n");
  fprintf(stderr, "  newgame <NBCOL> <NBLINE>\n");
  fprintf(stderr, "  genmove\n");
  fprintf(stderr, "  play <L0C0L1C1>\n");
  fprintf(stderr, "  showboard\n");
}

void name() {
  printf("= ids_player\n\n");
}

void newgame() {
  if((boardheight < 1 || boardheight > 10) && (boardwidth < 1 || boardwidth > 10)) {
    fprintf(stderr, "boardsize is %d %d ???\n", boardheight, boardwidth);
    printf("= \n\n");
    return;
  }

  B.init(boardheight, boardwidth);
  white_turn = true;

  if(verbose) fprintf(stderr, "ready to play on %dx%d board\n", boardheight, boardwidth);

  printf("= \n\n");
}

void showboard() {
  B.print_board(stderr);
  printf("= \n\n");
}

double heuristique(const bt_t &state) {
  // TODO: implémenter state.evaluate(); // renverra une valeur entre 0 et 1 ?
  return static_cast<double>(rand()) / static_cast<double>(RAND_MAX);
}

// Obtenir les coups possibles
std::vector<bt_move_t> nextMoves(bt_t &state) {
  state.update_moves();
  std::vector<bt_move_t> moves(state.moves, state.moves + state.nb_moves);
  return moves;
}

// Appliquer un coup et retourner le nouvel état
bt_t applyMove(const bt_t &state, const bt_move_t &move) {
  bt_t new_state = state;
  new_state.play(move);
  return new_state;
}

// Recherche en profondeur limitée (DLS)
void DLS(bt_t &state, int depth, bool is_white) {
    if (solution_size != 0) return;

    std::string state_hash = state.board_to_string(is_white);
    hashmap[state_hash] = depth;

    if (heuristique(best_solution) > heuristique(state)) {
      best_solution = state;
    }

    int game_status = state.endgame();
    if ((game_status == WHITE && is_white) || (game_status == BLACK && !is_white)) { // victoire trouvée
      solution_size = depth; 
      return;
    }

    if (depth == DLS_MAX_DEPTH) return;

    std::vector<bt_move_t> moves = nextMoves(state);
    for (bt_move_t move : moves) {
      bt_t new_solution = applyMove(state, move);

      std::string new_hash = new_solution.board_to_string(is_white);
      if (hashmap.find(new_hash) == hashmap.end() || hashmap[new_hash] > depth) {
        solution[depth] = move;
        DLS(new_solution, depth + 1, is_white);
      }

      if (solution_size != 0) break;
    }
}

// Recherche IDS
bt_move_t IDS(bt_t& state, bool is_white) {
  solution.clear();
  solution_size = 0;
  best_solution = state;

  for (int depth = 1; depth <= IDS_MAX_DEPTH; depth++) {
    hashmap.clear();
    DLS_MAX_DEPTH = depth;
    solved = false;
    DLS(state, 0, is_white);

    if (solution_size != 0) break;
  }

  return solution[0];  // Le premier coup menant à la solution trouvée
}

void genmove() {
  int ret = B.endgame();
  if (ret != EMPTY) {
    fprintf(stderr, "game finished\n");
    if(ret == WHITE) fprintf(stderr, "white player wins\n");
    else fprintf(stderr, "black player wins\n");
    printf("= \n\n");
    return;
  }

  bt_move_t best_move = IDS(B, white_turn);
  B.play(best_move);

  if (verbose) {
    best_move.print(stderr, white_turn, B.nbl);
    fprintf(stderr, "\n");
  }

  white_turn = !white_turn;
  printf("= %s\n\n", best_move.tostr(B.nbl).c_str());
}

// Jouer un coup donné
void play(char a, char b, char c, char d) {
  bt_move_t m;
  m.line_i = boardheight-(a-'0');
  m.col_i = b-'a';
  m.line_f = boardheight-(c-'0');
  m.col_f = d-'a';

  if(B.can_play(m)) {
    B.play(m);
    if(verbose) {
      m.print(stderr, white_turn, B.nbl);
      fprintf(stderr, "\n");
    }
    white_turn = !white_turn;
  } else {
    fprintf(stderr, "CANT play %d %d %d %d ?\n", m.line_i, m.col_i, m.line_f, m.col_f);
  }

  if(showboard_at_each_move) showboard();
  printf("= \n\n");
}

int main(int _ac, char** _av) {
  bool echo_on = false;
  setbuf(stdout, 0);
  setbuf(stderr, 0);

  if(verbose) fprintf(stderr, "ids_player started\n");
  char a,b,c,d; // for play cmd
  
  for (std::string line; std::getline(std::cin, line);) {
    if (verbose) fprintf(stderr, "ids_player receive %s\n", line.c_str());
    if (echo_on) if(verbose) fprintf(stderr, "%s\n", line.c_str());
    if (line.compare("quit") == 0) { printf("= \n\n"); break; }
    else if( line.compare("echo ON") == 0) echo_on = true;
    else if( line.compare("echo OFF") == 0) echo_on = false;
    else if (line.compare("help") == 0) help();
    else if (line.compare("name") == 0) name();
    else if (sscanf(line.c_str(), "newgame %d %d", &boardheight, &boardwidth) == 2) newgame();
    else if (line.compare("genmove") == 0) genmove();
    else if (sscanf(line.c_str(), "play %c%c%c%c\n", &a,&b,&c,&d) == 4) play(a,b,c,d);
    else if (line == "showboard") showboard();
    else if (line.compare(0,2,"//") == 0) ; // just comments
    else fprintf(stderr, "???\n");
    if (echo_on) printf(">");
  }

  if(verbose) fprintf(stderr, "bye.\n");

  return 0;
}
