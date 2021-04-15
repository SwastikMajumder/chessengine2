#ifndef BITBOARD_H_INCLUDED
#define BITBOARD_H_INCLUDED


#include <stdbool.h>
#include <stdint.h>


/** Typedefs **/

/* BITBOARD - The last bit that is 0x1 represent the square h1 and the first bit 0x1 << 63 represents a8 */
typedef uint64_t BITBOARD;

/* Type stores the score or evaluation of a node */
typedef int SCORE;

/** Enumerated typedefs **/

/* Color */
typedef enum {
  WHITE = 0,                                                            /* Plays first in the starting position */
  BLACK = 1                                                             /* Plays second in the starting position */
} COLOR;

/* Piece type */
typedef enum {
  INVALID_PIECE_TYPE = -1,                                              /* Invalid piece */
  PAWN = 0, KNIGHT = 1, KING = 2,                                       /* Non sliding pieces including the king */
  BISHOP = 3, ROOK = 4, QUEEN  = 5                                      /* Sliding pieces */
} PIECE_TYPE;


/** To allow unsigned subtraction underflow **/
/* If this is defined then x & (x - 1) should be false if x is unsigned and set 0 */

#define ALLOW_UNSIGNED_SUBTRACTION_UNDERFLOW


/** No move **/
/* No move allowed or invalid */

#define NO_MOVE ((BITBOARD)0)



/** Flags **/

/* Flag for knowing if the move is a promotion */
#define FLAG_PROMOTION (0x1 << 12)


/** Limits **/

#define INFINITY 16000                                                  /* No score is higher than this */

#define BOARD_SIZE 64                                                   /* Number of possible location in the bit board */
#define BOARD_WIDTH 8                                                   /* Width of the square bit board */

#define TOTAL_COLOR 2                                                   /* Number of variety colors present in bit board */

#define TOTAL_PIECE_SLIDING 3                                           /* Total pieces whose attack or movement range can be shorted by other pieces */
#define TOTAL_PIECE_NON_SLIDING 3                                       /* Total pieces whose attack or movement range is not affected by others */

#define TOTAL_PIECE (TOTAL_PIECE_SLIDING + TOTAL_PIECE_NON_SLIDING)     /* Total piece is the sum of sliding and non sliding pieces */

#define TOTAL_ATTACK_DIRECTION 8                                        /* Total possible attack direction (exception knight, we consider sliding one */

#define TOTAL_CASTLING_POSSIBILITY 2                                    /* How many ways the king can castle */

#define MAX_DEPTH 1024                                                  /* Maximum possible depth (large number) */


/** Time Out **/
/* Time is finished when the engine was to return move in a time */

#define TIME_OVER -INFINITY


/** Iteration constants **/

#define INIT_COLOR             WHITE                                    /* We start from seeing the white color piece */
#define LAST_COLOR             (BLACK + 1)                              /* Last color is black */

#define INIT_PIECE             PAWN                                     /* Initial piece we consider is pawn */
#define LAST_PIECE             (QUEEN + 1)                              /* Last piece is queen and is sliding */

#define INIT_PIECE_NON_SLIDING INIT_PIECE                               /* Pawn is a non sliding we start from there */
#define LAST_PIECE_NON_SLIDING (KING + 1)                               /* Last piece which is non sliding is king */

#define INIT_PIECE_SLIDING     BISHOP                                   /* Initial sliding piece we start from bishop */
#define LAST_PIECE_SLIDING     LAST_PIECE                               /* Queen is the last piece we consider */

#define INIT_GAME_PHASE MIDDLE_GAME
#define LAST_GAME_PHASE (END_GAME + 1)


/** Iteration of pseudo attack directions **/
/* These attack are represented in bit board, a sliding piece can never go or attack outside these pseudo attack,.
   but may not go to all the possible locations */
/* Upward attacks and downwards attack
   * Upwards
     When a pseudo attack of a sliding piece is considered which attack in northern or westwards direction.
     All the individual bit iterated pseudo will be bigger than the location of the piece.
   * Downwards
     When a pseudo attack of a sliding piece is considered which attack in southern or eastwards direction.
     All the individual bit iterated pseudo will be smaller than the location of the piece.
*/
/* Index and attack directions
 * Upwards
   0 - North East attack
   1 - North West attack
   2 - North attack
   3 - West attack
 * Downwards
   4 - South West attack
   5 - South East attack
   6 - South attack
   7 - East attack
*/

#define INIT_ATTACK_DIRECTION_UPWARDS(pieceType) (!((pieceType) & 0x1) << 1)                          /* Bishop -> 0  Rook -> 2  Queen -> 0 */
#define LAST_ATTACK_DIRECTION_UPWARDS(pieceType) (((!((pieceType) & 0x2)) << 1) + 2)                  /* Bishop -> 2  Rook -> 4  Queen -> 4 */

#define INIT_ATTACK_DIRECTION_DOWNWARDS(pieceType) (INIT_ATTACK_DIRECTION_UPWARDS(pieceType) | 0x4)   /* Bishop -> 4  Rook -> 6  Queen -> 4 */
#define LAST_ATTACK_DIRECTION_DOWNWARDS(pieceType) (((!((pieceType) & 0x2)) << 1) + 6)                /* Bishop -> 6  Rook -> 8  Queen -> 8 */


/** Algebraic notation to bit board **/

#define AN(a, b) ((BITBOARD)0x1 << (('h' - (a)) | (((b) - '1') << 3)))  /* Convert algebraic to index and then shift to create bit board */


/** Ranks **/

/* The first rank */
#define FIRST_RANK (AN('a', '1') | AN('b', '1') | AN('c', '1') | AN('d', '1') | \
                    AN('e', '1') | AN('f', '1') | AN('g', '1') | AN('h', '1'))

/* Rank is relative for both colors */
#define RANK(color, rank) (color == WHITE ? \
                           FIRST_RANK << ((rank - 1) * BOARD_WIDTH) : \
                           FIRST_RANK << ((BOARD_WIDTH - rank) * BOARD_WIDTH))

/** Files **/

#define FILE_A (AN('a', '1') | AN('a', '2') | AN('a', '3') | AN('a', '4') | \
                AN('a', '5') | AN('a', '6') | AN('a', '7') | AN('a', '8'))
#define FILE_B (FILE_A >> 1)
#define FILE_C (FILE_A >> 2)
#define FILE_D (FILE_A >> 3)
#define FILE_E (FILE_A >> 4)
#define FILE_F (FILE_A >> 5)
#define FILE_G (FILE_A >> 6)
#define FILE_H (FILE_A >> 7)


/** Convert bit board to index, extract left most and the right most bit **/
/*
 * CONVERT - Convert bit board to index notation
 * LEFT - Extract the leftmost bit
 * RIGHT - Extract the right most bit
*/

#if defined(__GNUC__) || defined(__GNUG__)
  #define CONVERT(input) (input ? __builtin_clzll(input) : 0)
  #define LEFT(input) (((BITBOARD) 0x1 << (BOARD_SIZE - 1)) >> CONVERT(input))
#elif defined(_MSC_VER)
  #define CONVERT(input)  _BitScanReverse64(input)
  #define LEFT(input) (((BITBOARD) 0x1 << (BOARD_SIZE - 1)) >> CONVERT(input))
#else
  const int DeBruijn[BOARD_SIZE] = {
       0, 63,  5, 62,  4, 16, 10, 61,
       3, 24, 15, 36,  9, 30, 21, 60,
       2, 12, 26, 23, 14, 45, 35, 43,
       8, 33, 29, 52, 20, 49, 41, 59,
       1,  6, 17, 11, 25, 37, 31, 22,
      13, 27, 46, 44, 34, 53, 50, 42,
       7, 18, 38, 32, 28, 47, 54, 51,
      19, 39, 48, 55, 40, 56, 57, 58,
  };
  #define CONVERT(input) DeBruijn[((BITBOARD)(input) * 0x07EDD5E59A4E28C2) >> 58]
  BITBOARD LEFT(BITBOARD input){
    input |= input >> 1;
    input |= input >> 2;
    input |= input >> 4;
    input |= input >> 8;
    input |= input >> 16;
    input |= input >> 32;
    return input - (input >> 1);
  }
#endif

#define RIGHT(input) (input ^ ((input - 1) & input))                    /* Remove last bit and see what bit was removed */


/** Structure representing our board **/
/*
 * King Virgin
   If the king of some color moves then KingVirgin[color] turns to false.
 * Rook Virgin
   Initially the this array contains the location of the virgin rook,
   when any of the rook moves the bit is removed from the board of that rook's location.
 * Board
   Two dimensional array representing our pieces in the board of both colors.
   If a piece of some is present in that location that is set to true.
*/
typedef struct BOARD {
  bool KingVirgin[TOTAL_COLOR];                                         /* If king is a virgin */
  BITBOARD RookVirgin[TOTAL_COLOR];                                     /* Location of the virgin rooks */
  BITBOARD Occupied[TOTAL_COLOR];                                       /* Occupied places in the board */
  BITBOARD Board[TOTAL_COLOR][TOTAL_PIECE];                             /* Board array */
} BOARD;


#endif /* BITBOARD_H_INCLUDED */
