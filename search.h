#ifndef SEARCH_H_INCLUDED
#define SEARCH_H_INCLUDED


#include <time.h>


#include "bitboard.h"


/** Print a move in principle variation **/
/* If pv itself stores the principle variation we pass . to it
 * Else if pv is a pointer to it we pass -> to it
 * Promote to queen if a promotion is found
*/

#define PRINT_PV_MOVE(ACCESS, param){ \
  putchar((pv ACCESS Move[param] & 0x7) + 'a'); \
  putchar(((pv ACCESS Move[param] >> 3 & 0x7) ^ 0x7) + '1'); \
  putchar((pv ACCESS Move[param] >> 6 & 0x7) + 'a'); \
  putchar((((pv ACCESS Move[param] >> 9) & 0x7) ^ 0x7) + '1'); \
  if (pv ACCESS Move[param] & FLAG_PROMOTION) putchar('q'); \
  putchar(' '); \
}


/** Structure which stores our principle variation **/
/* Number of moves in the principle variation and the move list */

typedef struct PV {
  int NumberOfMoves;
  int Move[MAX_DEPTH];
} PV;


/** Structure which stores our data for doing output according to uci protocol **/
/* Initial ply count, initial score, initial time and nodes */

typedef struct UCI_DATA {
  int Ply;
  SCORE Score;
  clock_t InitTime;
  unsigned long int Nodes;
} UCI_DATA;


/** Search further nodes using negamax search with alpha beta pruning **/

int negamax(BOARD *, BITBOARD, BITBOARD, COLOR, BITBOARD, SCORE, SCORE, SCORE, int, int, PV *, UCI_DATA *);


#endif /* SEARCH_H_INCLUDED */
