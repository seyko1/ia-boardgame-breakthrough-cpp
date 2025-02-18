#include <cstdio>
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include "mybt.h"

#define IDS_MAX_DEPTH 2
#define PLAYER_NAME "fg_player"

bt_t B;
int boardwidth = 0;
int boardheight = 0;
bool white_turn = true;
int DLS_MAX_DEPTH = 5;

// Table de hashage pour stocker les profondeurs des états explorés
std::unordered_map<std::string, int> hashmap;

// Tableau pour stocker les coups par profondeur
std::vector<bt_move_t> solution;
std::vector<bt_move_t> solution_copy;
int solution_size = 0;

bt_t best_solution;
bool solved = false;

#ifndef VERBOSE_IDS_PLAYER
#define VERBOSE_IDS_PLAYER
bool verbose = false;
bool debug = false;
bool showboard_at_each_move = false;
#endif

void help() {
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

void name() {
  printf("= %s\n\n", PLAYER_NAME);
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

// Renvoie une valeur d'évaluation de la position du jeu
double heuristique(const bt_t &state, int depth, bool is_white, bool is_current_state) {
  int white_pieces = 0, black_pieces = 0, white_distance = 0, black_distance = 0;
  double avantage_pieces, avantage_distance, result;

  int max_pieces = state.nbc * 2;
  // Distance totale cumulée dans l'état de départ avec deux lignes rempli de pions.
  int max_distance = (state.nbl - 1) * state.nbc + (state.nbl - 2) * state.nbc;

  for (int i = 0; i < boardheight; i++) {
    for (int j = 0; j < boardwidth; j++) {
      if (state.board[i][j] == WHITE) {
        white_pieces++;
        white_distance += (boardheight - i - 1);
      }
      else if (state.board[i][j] == BLACK) {
        black_pieces++;
        black_distance += i;  // distance à la première ligne (ligne d'arrivée des noirs)
      }
    }
  }

  avantage_pieces = static_cast<double>(white_pieces - black_pieces) / max_pieces;

  avantage_distance = static_cast<double>(white_distance - black_distance) / max_distance;


  result = 0.3 * avantage_pieces + 0.7 * avantage_distance;

  if (debug && is_current_state) {
    for (int i = 0; i < depth; i++) fprintf(stderr, "\t");
    fprintf(stderr, "avantage_pieces : %f\n", avantage_pieces);
    for (int i = 0; i < depth; i++) fprintf(stderr, "\t");
    fprintf(stderr, "avantage_distance : %f\n", avantage_distance);
    for (int i = 0; i < depth; i++) fprintf(stderr, "\t");
    fprintf(stderr, "result : %f\n", result);
  }
  return result;
}

// Obtenir les coups possibles
std::vector<bt_move_t> nextMoves(bt_t &state) {
  state.update_moves();
  std::vector<bt_move_t> moves(state.moves, state.moves + state.nb_moves);
  return moves;
}

void printMove(bt_move_t move, int depth) {
  char a, b, c, d;

  a = '0' + (boardheight - move.line_i);
  b = 'a' + move.col_i;
  c = '0' + (boardheight - move.line_f);
  d = 'a' + move.col_f;

  std::string str = std::string(1, a) +
                    std::string(1, b) +
                    std::string(1, c) +
                    std::string(1, d);

  for (int i = 0; i < depth; i++) fprintf(stderr, "\t");
  fprintf(stderr, "(%d)%s\n", depth, str.c_str());
}

// Appliquer un coup et retourner le nouvel état
bt_t applyMove(const bt_t &state, const bt_move_t &move) {
  bt_t new_state = state;
  new_state.play(move);
  return new_state;
}

// Trouve la meilleure solution pour un état donné du jeu
void DLS(bt_t &state, int depth, bool is_white) {
    if (solution_size != 0) return;

    std::string state_hash = state.board_to_string(is_white);
    hashmap[state_hash] = depth;

    double currentH = heuristique(state, depth, is_white, true);
    double bestH    = heuristique(best_solution, depth, is_white, false);

    // Chercher une valeur supérieure pour les pions blanc, inférieure sinon. 
    bool bestSolutionFound = is_white ? currentH > bestH : currentH < bestH;

    if (bestSolutionFound) {
      best_solution = state;
      // stocke la séquence de coups menant à cet état
      solution_copy = solution;
      if (debug) {
        for (int i = 0; i < depth; i++) fprintf(stderr, "\t");
        fprintf(stderr, "\x1B[31mMeilleure solution trouvée\x1B[0m\n");
      }
    }

    int game_status = state.endgame();
    if ((game_status == WHITE && is_white) || (game_status == BLACK && !is_white)) { // victoire trouvée
      solution_size = depth; 
      return;
    }

    if (depth == DLS_MAX_DEPTH) return;

    std::vector<bt_move_t> moves = nextMoves(state);

    if (debug) {
      for (int i = 0; i < depth; i++) fprintf(stderr, "\t");
      fprintf(stderr, "\x1B[33m%ld coups trouvés\x1B[0m\n", moves.size());
    }

    for (bt_move_t move : moves) {
      if (debug) printMove(move, depth);
      bt_t new_solution = applyMove(state, move);

      std::string new_hash = new_solution.board_to_string(is_white);

      // Si l'état new_solution n'a jamais été visité auparavant.
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

    if (debug) {
      fprintf(stderr, "\x1B[36mProfondeur max : %d\x1B[0m\n", depth); 
    }

    solution.resize(DLS_MAX_DEPTH);
    solved = false;
    DLS(state, 0, is_white);

    if (solution_size != 0) break;
  }

  return solution_copy[0];  // Le premier coup menant à la solution trouvée
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
  setbuf(stdout, 0);
  setbuf(stderr, 0);

  if(verbose) fprintf(stderr, "%s started\n", PLAYER_NAME);
  char a,b,c,d; // for play cmd
  
  for (std::string line; std::getline(std::cin, line);) {
    if (verbose) fprintf(stderr, "%s receive %s\n", PLAYER_NAME, line.c_str());
    if (line.compare("quit") == 0) { printf("= \n\n"); break; }
    else if( line.compare("debug ON") == 0) debug = true;
    else if( line.compare("debug OFF") == 0) debug = false;
    else if( line.compare("verbose ON") == 0) verbose = true;
    else if( line.compare("verbose OFF") == 0) verbose = false;
    else if (line.compare("help") == 0) help();
    else if (line.compare("name") == 0) name();
    else if (sscanf(line.c_str(), "newgame %d %d", &boardheight, &boardwidth) == 2) newgame();
    else if (line.compare("genmove") == 0) genmove();
    else if (sscanf(line.c_str(), "play %c%c%c%c\n", &a,&b,&c,&d) == 4) play(a,b,c,d);
    else if (line == "showboard") showboard();
    else if (line.compare(0,2,"//") == 0) ; // just comments
    else fprintf(stderr, "???\n");
  }

  if(verbose) fprintf(stderr, "bye.\n");

  return 0;
}
