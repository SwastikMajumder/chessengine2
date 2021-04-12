#include "attack.h"
#include "bitboard.h"
#include "evaluation.h"


/** Piece values **/
/* Piece values are given for both in the opening and in the end game
 * Piece values are given in centi-pawns (one hundredth value of a pawn
*/

const SCORE score_pieceValue[TOTAL_GAME_PHASE][TOTAL_PIECE] =
{
  {
    100,  /* Pawn */
    300,  /* Knight */
    0,    /* Dummy value for king */
    340,  /* Bishop */
    500,  /* Rook */
    900   /* Queen */
  },
  {
    100,  /* Pawn */
    300,  /* Knight */
    0,    /* Dummy value for king */
    340,  /* Bishop */
    500,  /* Rook */
    900   /* Queen */
  },
};


/** Positional value */
/* Positional values are given for both in the opening and in the end game
 * Positional values are given in centi-pawns (one hundredth value of a pawn
*/

const SCORE score_positionalValue[TOTAL_GAME_PHASE][TOTAL_PIECE][BOARD_SIZE] =
{
  /* Middle Game */
  {
    /* Pawn */
    {
      0,   0,   0,   0,   0,   0,   0,   0,
      5,  10,  15,  20,  20,  15,  10,   5,
      4,   8,  12,  16,  16,  12,   8,   4,
      3,   6,   9,  12,  12,   9,   6,   3,
      2,   4,   6,   8,   8,   6,   4,   2,
      1,   2,   3, -10, -10,   3,   2,   1,
      0,   0,   0, -40, -40,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0
    },

    /* Knight */
    {
      -10, -10, -10, -10, -10, -10, -10, -10,
      -10,   0,   0,   0,   0,   0,   0, -10,
      -10,   0,   5,   5,   5,   5,   0, -10,
      -10,   0,   5,  10,  10,   5,   0, -10,
      -10,   0,   5,  10,  10,   5,   0, -10,
      -10,   0,   5,   5,   5,   5,   0, -10,
      -10,   0,   0,   0,   0,   0,   0, -10,
      -10, -30, -10, -10, -10, -10, -30, -10
    },

    /* King */
    {
      -40, -40, -40, -40, -40, -40, -40, -40,
      -40, -40, -40, -40, -40, -40, -40, -40,
      -40, -40, -40, -40, -40, -40, -40, -40,
      -40, -40, -40, -40, -40, -40, -40, -40,
      -40, -40, -40, -40, -40, -40, -40, -40,
      -40, -40, -40, -40, -40, -40, -40, -40,
      -20, -20, -20, -20, -20, -20, -20, -20,
        0,  20,  40, -20,   0, -20,  40,  20
    },

    /* Bishop */
    {
      -10, -10, -10, -10, -10, -10, -10, -10,
      -10,   0,   0,   0,   0,   0,   0, -10,
      -10,   0,   5,   5,   5,   5,   0, -10,
      -10,   0,   5,   4,   4,   5,   0, -10,
      -10,   0,   6,   4,   4,   6,   0, -10,
      -10,   0,   5,   5,   5,   5,   0, -10,
      -10,   0,   0,   0,   0,   0,   0, -10,
      -10, -10, -20, -10, -10, -20, -10, -10
    },

    /* Rook */
    {
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0
    },

    /* Queen */
    {
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0
    }
  },

  /* End Game */
  {
    /* Pawn */
    {
      0,   0,   0,   0,   0,   0,   0,   0,
      5,  10,  15,  20,  20,  15,  10,   5,
      4,   8,  12,  16,  16,  12,   8,   4,
      3,   6,   9,  12,  12,   9,   6,   3,
      2,   4,   6,   8,   8,   6,   4,   2,
      1,   2,   3, -10, -10,   3,   2,   1,
      0,   0,   0, -40, -40,   0,   0,   0,
      0,   0,   0,   0,   0,   0,   0,   0
    },

    /* Knight */
    {
      -10, -10, -10, -10, -10, -10, -10, -10,
      -10,   0,   0,   0,   0,   0,   0, -10,
      -10,   0,   5,   5,   5,   5,   0, -10,
      -10,   0,   5,  10,  10,   5,   0, -10,
      -10,   0,   5,  10,  10,   5,   0, -10,
      -10,   0,   5,   5,   5,   5,   0, -10,
      -10,   0,   0,   0,   0,   0,   0, -10,
      -10, -30, -10, -10, -10, -10, -30, -10
    },

    /* King */
    {
      0,  10,  20,  30,  30,  20,  10,   0,
     10,  20,  30,  40,  40,  30,  20,  10,
     20,  30,  40,  50,  50,  40,  30,  20,
     30,  40,  50,  60,  60,  50,  40,  30,
     30,  40,  50,  60,  60,  50,  40,  30,
     20,  30,  40,  50,  50,  40,  30,  20,
     10,  20,  30,  40,  40,  30,  20,  10,
      0,  10,  20,  30,  30,  20,  10,   0
    },

    /* Bishop */
    {
      -10, -10, -10, -10, -10, -10, -10, -10,
      -10,   0,   0,   0,   0,   0,   0, -10,
      -10,   0,   5,   5,   5,   5,   0, -10,
      -10,   0,   5,  10,  10,   5,   0, -10,
      -10,   0,   5,  10,  10,   5,   0, -10,
      -10,   0,   5,   5,   5,   5,   0, -10,
      -10,   0,   0,   0,   0,   0,   0, -10,
      -10, -10, -20, -10, -10, -20, -10, -10
    },

    /* Rook */
    {
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0
    },

    /* Queen */
    {
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0,
        0,   0,   0,   0,   0,   0,   0,   0
    }
  }
};


/** Constants which decide the phase of the game **/

const GAME_PHASE_VALUE gamePhaseLimitValue = 2400;                      /* If value less than the limit it is end game else middle game */

/* Values which make the game middle game */
const GAME_PHASE_VALUE pieceCorrespondingGamePhaseValue[TOTAL_PIECE] =
{
  100, 300, 0, 300, 500, 900
};


/** Check which game phase a game phase value represents **/
/* If value is below than a limit then it is end game else it is middle game */

GAME_PHASE currentGamePhase(GAME_PHASE_VALUE gamePhaseValue){
  if (gamePhaseLimitValue > gamePhaseValue){
    return END_GAME;
  } else {
    return MIDDLE_GAME;
  }
}


/** Return a piece value **/
/* Return a piece value by considering the game phase */

SCORE pieceValue(GAME_PHASE gamePhase, PIECE_TYPE pieceType){
  return score_pieceValue[gamePhase][pieceType];
}


/** Return a positional value **/
/* Return a positional value by considering the color and the game phase */

SCORE positionalValue(COLOR color, GAME_PHASE gamePhase, PIECE_TYPE pieceType, int index){
  return score_positionalValue[gamePhase][pieceType][color == WHITE ?   /* If positional value is requested for white */
                                          index :                       /* Return directly */
                                          index ^ (0x7 << 3)];          /* Change the rank if black */
}


/** Return game phase value for a piece type **/
/* Return how much a piece affects the game phase */

GAME_PHASE_VALUE pieceGamePhaseValue(PIECE_TYPE pieceType){
  return pieceCorrespondingGamePhaseValue[pieceType];
}


/** Movement evaluation **/
/* Return how good is movement of a piece */

SCORE movementEvaluation(COLOR color, GAME_PHASE gamePhase, PIECE_TYPE pieceTypeFrom, int fromIndex, int toIndex){
  return positionalValue(color, gamePhase, pieceTypeFrom, toIndex) -                    /* We came in here */
         positionalValue(color, gamePhase, pieceTypeFrom, fromIndex);                   /* We left that place */
}


/** Capture evaluation **/
/* Return how good is capture by a piece */

SCORE captureEvaluation(COLOR color, GAME_PHASE gamePhase, PIECE_TYPE pieceTypeFrom, PIECE_TYPE pieceTypeTo, int fromIndex, int toIndex){
  return movementEvaluation(color, gamePhase, pieceTypeFrom, fromIndex, toIndex) + /* Movement evaluation */
         pieceValue(gamePhase, pieceTypeTo) +                                      /* Value of the captured piece */
         positionalValue(!color, gamePhase, pieceTypeTo, toIndex);                 /* Positional value of the captured piece */
}


/** Captures can affect game phase **/
/* Return how much captures can affect the phase of the game */

GAME_PHASE_VALUE captureGamePhaseEvaluation(PIECE_TYPE pieceTypeTo){
  return pieceGamePhaseValue(pieceTypeTo);
}


/** En passant capture evaluation **/
/* Return how much a en passant capture is good */

SCORE enPassantEvaluation(int enPassantIndex, COLOR color, GAME_PHASE gamePhase, int fromIndex, int toIndex){
  return movementEvaluation(color, gamePhase, PAWN, fromIndex, toIndex) +          /* Movement evaluation */
         pieceValue(gamePhase, PAWN) +                                             /* Value of the captured piece */
         positionalValue(!color, gamePhase, PAWN, enPassantIndex);                 /* Positional value of the captured piece */
}


/** En passant can affect game phase **/
/* Return how much a en passant can affect the phase of a game */

GAME_PHASE_VALUE enPassantGamePhaseEvaluation(){
  return pieceGamePhaseValue(PAWN);
}


/** Simple promotion evaluation **/
/* Return the score for a simple promotion */

SCORE promotionMovementEvaluation(PIECE_TYPE promotionPiece, COLOR color, GAME_PHASE gamePhase, int fromIndex, int toIndex){
  return pieceValue(gamePhase, promotionPiece) - pieceValue(gamePhase, PAWN) +     /* Promotion piece value */
         positionalValue(color, gamePhase, promotionPiece, toIndex) -              /* Positional value of the promoting piece */
         positionalValue(color, gamePhase, PAWN, fromIndex);                       /* Positional value of the pawn */
}


/** Simple promotion can affect the phase of the game **/
/* Check how much a simple promotion can affect the phase of the game */

GAME_PHASE_VALUE promotionMovementGamePhaseEvaluation(PIECE_TYPE promotionPiece){
  return pieceGamePhaseValue(promotionPiece);
}


/** Capture promotion evaluation **/
/* Return the score for a capture promotion */

SCORE promotionCaptureEvaluation(PIECE_TYPE promotionPiece, COLOR color, GAME_PHASE gamePhase, PIECE_TYPE pieceTypeTo, int fromIndex, int toIndex){
  return promotionMovementEvaluation(promotionPiece, color, gamePhase, fromIndex, toIndex) +
         pieceValue(gamePhase, pieceTypeTo) +
         positionalValue(!color, gamePhase, pieceTypeTo, toIndex);
}

/** Capture promotion can affect the phase of the game **/
/* Check how much a capture promotion can affect the phase of the game */

GAME_PHASE_VALUE promotionCaptureGamePhaseEvaluation(PIECE_TYPE promotionPiece, PIECE_TYPE pieceTypeTo){
  return pieceGamePhaseValue(promotionPiece) + pieceGamePhaseValue(pieceTypeTo);
}


/** Castling **/
/* For evaluating castling, add from position square tables and add some extra scores */

/* King side castling (considered better than queen side castling) */
SCORE castling_OO_Evaluation(COLOR color, GAME_PHASE gamePhase){
  const SCORE score_castling_OO = 35;
  if (color == WHITE){
   return positionalValue(WHITE, gamePhase, KING, 62) - positionalValue(WHITE, gamePhase, KING, 60) +
          positionalValue(WHITE, gamePhase, ROOK, 61) - positionalValue(WHITE, gamePhase, ROOK, 63) + score_castling_OO;
  } else {
    return positionalValue(BLACK, gamePhase, KING, 6) - positionalValue(BLACK, gamePhase, KING, 4) +
           positionalValue(BLACK, gamePhase, ROOK, 5) - positionalValue(BLACK, gamePhase, ROOK, 7) + score_castling_OO;
  }
}

/* Queen side castling */
SCORE castling_OOO_Evaluation(COLOR color, GAME_PHASE gamePhase){
  const SCORE score_castling_OOO = 30;
  if (color == WHITE){
   return positionalValue(WHITE, gamePhase, KING, 58) - positionalValue(WHITE, gamePhase, KING, 60) +
          positionalValue(WHITE, gamePhase, ROOK, 59) - positionalValue(WHITE, gamePhase, ROOK, 56) + score_castling_OOO;
  } else {
    return positionalValue(BLACK, gamePhase, KING, 2) - positionalValue(BLACK, gamePhase, KING, 4) +
           positionalValue(BLACK, gamePhase, ROOK, 3) - positionalValue(BLACK, gamePhase, ROOK, 0) + score_castling_OOO;
  }
}


/** Special evaluation functions **/
/* Check if there are double pawns
 * Check if the pawns of the king are bad
 * Return score
*/


/** Macro to evaluate the pawn of the king **/
/* Check the color requested
 * If it is not moved yet, no problem return zero
 * Else if it moved one step return a worse score
 * Else if it moved two steps return worse score
 * Else if there are pawns on the file (if the pawn of the king moved far ahead), return worse score
 * Else return the worst score (if there no pawns in the file
*/

#define KING_PAWN_SCORE(color, pawn, file) \
  (color == WHITE ? \
                    ((pawn) & AN((file), '2')) ? 0 : \
                    ((pawn) & AN((file), '3')) ? -10 : \
                    ((pawn) & FILE_A >> ((file) - 'a')) ? -20 : \
                     -25 \
                  : ((pawn) & AN((file), '7')) ? 0 : \
                    ((pawn) & AN((file), '6')) ? -10 : \
                    ((pawn) & FILE_A >> ((file) - 'a')) ? -20 : \
                    -25 \
  )


/** Main special evaluation function **/
/* Evaluate double pawns
 * Evaluation king pawns
 * Compute king safety
 * Return score
*/

SCORE evaluation(BOARD *board, GAME_PHASE_VALUE gamePhaseValue){
  const SCORE doublePawn_penalty = -10;                                 /* Double pawn bad */

  SCORE score = 0;                                                      /* Start from no score */

  BITBOARD pawnWhite = board -> Board[WHITE][PAWN];                     /* Store white pawns */
  BITBOARD pawnBlack = board -> Board[BLACK][PAWN];                     /* Store black pawns */

  /* Score from no score */
  SCORE kingWhite = 0;
  SCORE kingBlack = 0;

  BITBOARD file;
  for (file = FILE_A; file; file >>= 1){                                /* For all files */
    BITBOARD filePawnWhite = file & pawnWhite;                          /* The white pawns in that file */
    BITBOARD filePawnBlack = file & pawnBlack;                          /* The black pawns in that file */
    if (filePawnBlack && (filePawnBlack & (filePawnBlack - 1))){        /* If there are pawns and double pawns */
      score += doublePawn_penalty;                                      /* Penalty */
    }
    if (filePawnWhite && (filePawnWhite & (filePawnWhite - 1))){        /* If there are pawns and double pawns */
      score -= doublePawn_penalty;                                      /* Penalty */
    }
  }

  /* We will add penalty for king side pawns if the king is in the king side else we will add for the queen side pawns */

  /* For the white king */
  if (board -> Board[WHITE][KING] & (FILE_H | FILE_G | FILE_F | FILE_E)){
    if (!(pawnWhite & AN('h', '3')))                                    /* The h3 is never bad */
      kingWhite += KING_PAWN_SCORE(WHITE, pawnWhite, 'h');
    kingWhite += KING_PAWN_SCORE(WHITE, pawnWhite, 'g');                /* Our engine is not expert in KID */
    kingWhite += KING_PAWN_SCORE(WHITE, pawnWhite, 'f') >> 1;           /* The file f has less problem */
  } else {
    if (!(pawnWhite & AN('a', '3')))
      kingWhite += KING_PAWN_SCORE(WHITE, pawnWhite, 'a');              /* The a3 is never bad */
    kingWhite += KING_PAWN_SCORE(WHITE, pawnWhite, 'b');                /* Our engine is not expert in KID */
    kingWhite += KING_PAWN_SCORE(WHITE, pawnWhite, 'c') >> 1;           /* The file a has less problem */
  }

  /* For the black king */
  if (board -> Board[BLACK][KING] & (FILE_H | FILE_G | FILE_F | FILE_E)){
    if (!(pawnBlack & AN('h', '6')))                                    /* The h3 is never bad */
      kingBlack += KING_PAWN_SCORE(BLACK, pawnBlack, 'h');
    kingBlack += KING_PAWN_SCORE(BLACK, pawnBlack, 'g');                /* Our engine is not expert in KID */
    kingBlack += KING_PAWN_SCORE(BLACK, pawnBlack, 'f') >> 1;           /* The file f has less problem */
  } else {
    if (!(pawnBlack & AN('a', '6')))
      kingBlack += KING_PAWN_SCORE(BLACK, pawnBlack, 'a');              /* The a3 is never bad */
    kingBlack += KING_PAWN_SCORE(BLACK,  pawnBlack, 'b');               /* Our engine is not expert in KID */
    kingBlack += KING_PAWN_SCORE(BLACK, pawnBlack, 'c') >> 1;           /* The file a has less problem */
  }

  /* The penalty should be high only if there enough pieces to attack our king */
  kingBlack *= (gamePhaseValue >> 1) / 3100;
  kingWhite *= (gamePhaseValue >> 1) / 3100;

  return score + kingBlack - kingWhite;                                 /* We return score positive for black */
}
