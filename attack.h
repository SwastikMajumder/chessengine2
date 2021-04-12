#ifndef ATTACK_H_INCLUDED
#define ATTACK_H_INCLUDED


#include "bitboard.h"


/** Typedefs **/

/* Attack direction */
typedef int ATTACK_DIRECTION;


/** Skeletal representation of all functions in the program **/

BITBOARD pseudoMove_nonSliding(COLOR, PIECE_TYPE, int);                 /* Pseudo attack of non sliding pieces */
BITBOARD pseudoMove_sliding(ATTACK_DIRECTION, int);                     /* Pseudo attack of sliding pieces */
BITBOARD pseudoCenter_knight(COLOR);                                    /* Center controlling position of knight */
BITBOARD pseudoCenter_sliding(COLOR, PIECE_TYPE);                       /* Center controlling positions for the sliding pieces */


/** Directions **/

/* Upwards */
#define N +1
#define E +1

/* Downwards */
#define W -1
#define S -1

/* Nowhere */
#define X 0


/** Ranged attacks **/
/* Usage
 * Ax(FILE, RANK, NORTH / SOUTH / -, EAST / WEST / -)
 * NORTH - N
 * EAST  - E
 * WEST  - W
 * SOUTH - S
 *   -   - X
*/

#define A1(a, b, c, d) AN(a + 1 * d, b + 1 * c)                         /* Single step */
#define A2(a, b, c, d) AN(a + 2 * d, b + 2 * c) | A1(a, b, c, d)        /* Double step */
#define A3(a, b, c, d) AN(a + 3 * d, b + 3 * c) | A2(a, b, c, d)        /* Triple step */
#define A4(a, b, c, d) AN(a + 4 * d, b + 4 * c) | A3(a, b, c, d)        /* Four step */
#define A5(a, b, c, d) AN(a + 5 * d, b + 5 * c) | A4(a, b, c, d)        /* Five step */
#define A6(a, b, c, d) AN(a + 6 * d, b + 6 * c) | A5(a, b, c, d)        /* Six step */
#define A7(a, b, c, d) AN(a + 7 * d, b + 7 * c) | A6(a, b, c, d)        /* Seven step is maximum because further we fall out of board */


/** Pawn attacks **/
/* Attack west only if not in file a
 * Attack east only if not in file h
*/

#define PA(a, b, c) (a == 'a' ? 0 : A1(a, b, c, W)) | (a == 'h' ? 0 : A1(a, b, c, E))


/** Knight attacks **/
/*
 * If file more than a and rank less than 7 then attack NORTH x 2 + WEST is allowed
 * If file less than h and rank less than 7 then attack NORTH x 2 + EAST is allowed
 * If file less than g and rank less than 8 then attack NORTH + EAST x 2 is allowed
 * If file less than g and rank more than 1 then attack SOUTH + EAST x 2 is allowed
 * If file less than h and rank more than 2 then attack SOUTH x 2 + EAST is allowed
 * If file more than a and rank more than 2 then attack SOUTH x 2 + WEST is allowed
 * If file more than b and rank more than 1 then attack SOUTH + WEST x 2 is allowed
 * If file more than c and rank less than 8 then attack NORTH + WEST x 2 is allowed
*/

#define H(a, b) (a > 'a' && b < '7' ? A1(a, b, N * 2, W) : 0) | \
                (a < 'h' && b < '7' ? A1(a, b, N * 2, E) : 0) | \
                (a < 'g' && b < '8' ? A1(a, b, N, E * 2) : 0) | \
                (a < 'g' && b > '1' ? A1(a, b, S, E * 2) : 0) | \
                (a < 'h' && b > '2' ? A1(a, b, S * 2, E) : 0) | \
                (a > 'a' && b > '2' ? A1(a, b, S * 2, W) : 0) | \
                (a > 'b' && b > '1' ? A1(a, b, S, W * 2) : 0) | \
                (a > 'b' && b < '8' ? A1(a, b, N, W * 2) : 0)


/** King attacks **/
/*
 * If not on file a then WEST attack is allowed
 * If not on file h then EAST attack is allowed
 * If not on rank 1 then SOUTH attack is allowed
 * If not on rank 8 then NORTH attack is allowed
 * If not on file a or rank 8 then NORTH - WEST attack is allowed
 * If not on file a or rank 1 then SOUTH - WEST attack is allowed
 * If not on file h or rank 8 then NORTH - EAST attack is allowed
 * If not on file h or rank 1 then SOUTH - EAST attack is allowed
*/

#define K(a, b) (a == 'a' ? 0 : A1(a, b, X, W)) | \
                (a == 'h' ? 0 : A1(a, b, X, E)) | \
                (b == '1' ? 0 : A1(a, b, S, X)) | \
                (b == '8' ? 0 : A1(a, b, N, X)) | \
                (a == 'a' || b == '8' ? 0 : A1(a, b, N, W)) | \
                (a == 'a' || b == '1' ? 0 : A1(a, b, S, W)) | \
                (a == 'h' || b == '8' ? 0 : A1(a, b, N, E)) | \
                (a == 'h' || b == '1' ? 0 : A1(a, b, S, E))


#endif /* ATTACK_H_INCLUDED */
