#include "attack.h"
#include "bitboard.h"
#include "evaluation.h"


/** Piece values **/
/* Piece values are given in centi-pawns (one hundredth value of a pawn */

const SCORE score_pieceValue[TOTAL_PIECE] =
{
    100,  /* Pawn */
    280,  /* Knight */
    0,    /* Dummy value for king */
    320,  /* Bishop */
    479,  /* Rook */
    929   /* Queen */
};


/** Positional value */
/* Positional values are given for both in the opening and in the end game
 * Positional values are given in centi-pawns (one hundredth value of a pawn
*/

const SCORE score_positionalValue[TOTAL_PIECE][BOARD_SIZE] =
{
    /* Pawn */
    {    0,   0,   0,   0,   0,   0,   0,   0,
        78,  83,  86,  73, 102,  82,  85,  90,
         7,  29,  21,  44,  40,  31,  44,   7,
       -17,  16,  -2,  15,  14,   0,  15, -13,
       -26,   3,  10,   9,   6,   1,   0, -23,
       -22,   9,   5, -11, -10,  -2,   3, -19,
       -31,   8,  -7, -37, -36, -14,   3, -31,
         0,   0,   0,   0,   0,   0,   0,   0
    },

    /* Knight */
    {
       -66, -53, -75, -75, -10, -55, -58, -70,
        -3,  -6, 100, -36,   4,  62,  -4, -14,
        10,  67,   1,  74,  73,  27,  62,  -2,
        24,  24,  45,  37,  33,  41,  25,  17,
        -1,   5,  31,  21,  22,  35,   2,   0,
       -18,  10,  13,  22,  18,  15,  11, -14,
       -23, -15,   2,   0,   2,   0, -23, -20,
       -74, -23, -26, -24, -19, -35, -22, -69
    },


    /* King */
    {
         4,  54,  47, -99, -99,  60,  83, -62,
       -32,  10,  55,  56,  56,  55,  10,   3,
       -62,  12, -57,  44, -67,  28,  37, -31,
       -55,  50,  11,  -4, -19,  13,   0, -49,
       -55, -43, -52, -28, -51, -47,  -8, -50,
       -47, -42, -43, -79, -64, -32, -29, -32,
        -4,   3, -14, -50, -57, -18,  13,   4,
        17,  30,  -3, -14,   6,  -1,  40,  18
    },

    /* Bishop */
    {
       -59, -78, -82, -76, -23,-107, -37, -50,
       -11,  20,  35, -42, -39,  31,   2, -22,
        -9,  39, -32,  41,  52, -10,  28, -14,
        25,  17,  20,  34,  26,  25,  15,  10,
        13,  10,  17,  23,  17,  16,   0,   7,
        14,  25,  24,  15,   8,  25,  20,  15,
        19,  20,  11,   6,   7,   6,  20,  16,
        -7,   2, -15, -12, -14, -15, -10, -10
    },

    /* Rook */
    {
        35,  29,  33,   4,  37,  33,  56,  50,
        55,  29,  56,  67,  55,  62,  34,  60,
        19,  35,  28,  33,  45,  27,  25,  15,
         0,   5,  16,  13,  18,  -4,  -9,  -6,
       -28, -35, -16, -21, -13, -29, -46, -30,
       -42, -28, -42, -25, -25, -35, -26, -46,
       -53, -38, -31, -26, -29, -43, -44, -53,
       -30, -24, -18,   5,  -2, -18, -31, -32
    },

    /* Queen */
    {
         6,   1,  -8,-104,  69,  24,  88,  26,
        14,  32,  60, -10,  20,  76,  57,  24,
        -2,  43,  32,  60,  72,  63,  43,   2,
         1, -16,  22,  17,  25,  20, -13,  -6,
       -14, -15,  -2,  -5,  -1, -10, -20, -22,
       -30,  -6, -13, -11, -16, -11, -16, -27,
       -36, -18,   0, -19, -15, -15, -21, -38,
       -39, -30, -31, -13, -31, -36, -34, -42
    }
};


/** Return a piece value **/
/* Return a piece value by considering the game phase */

SCORE pieceValue(PIECE_TYPE pieceType){
  return score_pieceValue[pieceType];
}


/** Return a positional value **/
/* Return a positional value by considering the color and the game phase */

SCORE positionalValue(COLOR color, PIECE_TYPE pieceType, int index){
  return score_positionalValue[pieceType][color == WHITE ?   /* If positional value is requested for white */
                              index :                        /* Return directly */
                              index ^ (0x7 << 3)];           /* Change the rank if black */
}


/** Movement evaluation **/
/* Return how good is movement of a piece */

SCORE movementEvaluation(COLOR color, PIECE_TYPE pieceTypeFrom, int fromIndex, int toIndex){
  return positionalValue(color, pieceTypeFrom, toIndex) -                    /* We came in here */
         positionalValue(color, pieceTypeFrom, fromIndex);                   /* We left that place */
}


/** Capture evaluation **/
/* Return how good is capture by a piece */

SCORE captureEvaluation(COLOR color, PIECE_TYPE pieceTypeFrom, PIECE_TYPE pieceTypeTo, int fromIndex, int toIndex){
  return movementEvaluation(color, pieceTypeFrom, fromIndex, toIndex) + /* Movement evaluation */
         pieceValue(pieceTypeTo) +                                      /* Value of the captured piece */
         positionalValue(!color, pieceTypeTo, toIndex);                 /* Positional value of the captured piece */
}


/** En passant capture evaluation **/
/* Return how much a en passant capture is good */

SCORE enPassantEvaluation(int enPassantIndex, COLOR color, int fromIndex, int toIndex){
  return movementEvaluation(color, PAWN, fromIndex, toIndex) +          /* Movement evaluation */
         pieceValue(PAWN) +                                             /* Value of the captured piece */
         positionalValue(!color, PAWN, enPassantIndex);                 /* Positional value of the captured piece */
}


/** Simple promotion evaluation **/
/* Return the score for a simple promotion */

SCORE promotionMovementEvaluation(PIECE_TYPE promotionPiece, COLOR color, int fromIndex, int toIndex){
  return pieceValue(promotionPiece) - pieceValue(PAWN) +     /* Promotion piece value */
         positionalValue(color, promotionPiece, toIndex) -              /* Positional value of the promoting piece */
         positionalValue(color, PAWN, fromIndex);                       /* Positional value of the pawn */
}


/** Capture promotion evaluation **/
/* Return the score for a capture promotion */

SCORE promotionCaptureEvaluation(PIECE_TYPE promotionPiece, COLOR color, PIECE_TYPE pieceTypeTo, int fromIndex, int toIndex){
  return promotionMovementEvaluation(promotionPiece, color, fromIndex, toIndex) +
         pieceValue(pieceTypeTo) +
         positionalValue(!color, pieceTypeTo, toIndex);
}


/** Castling **/
/* For evaluating castling, add from position square tables and add some extra scores */

/* King side castling (considered better than queen side castling) */
SCORE castling_OO_Evaluation(COLOR color){
  if (color == WHITE){
   return positionalValue(WHITE, KING, 62) - positionalValue(WHITE, KING, 60) +
          positionalValue(WHITE, ROOK, 61) - positionalValue(WHITE, ROOK, 63);
  } else {
    return positionalValue(BLACK, KING, 6) - positionalValue(BLACK, KING, 4) +
           positionalValue(BLACK, ROOK, 5) - positionalValue(BLACK, ROOK, 7);
  }
}

/* Queen side castling */
SCORE castling_OOO_Evaluation(COLOR color){
  if (color == WHITE){
   return positionalValue(WHITE, KING, 58) - positionalValue(WHITE, KING, 60) +
          positionalValue(WHITE, ROOK, 59) - positionalValue(WHITE, ROOK, 56);
  } else {
    return positionalValue(BLACK, KING, 2) - positionalValue(BLACK, KING, 4) +
           positionalValue(BLACK, ROOK, 3) - positionalValue(BLACK, ROOK, 0);
  }
}
