#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>


#include "bitbase.h"
#include "bitboard.h"
#include "attack.h"
#include "bitboard.h"
#include "search.h"
#include "evaluation.h"


/** Skeletal representation of all the functions in the program **/

/* Search positions in the game using alpha beta pruning */
SCORE negamax(BOARD *, BITBOARD, BITBOARD, COLOR, BITBOARD, SCORE, SCORE, SCORE, int, int, PV *, UCI_DATA *);

bool kingInCheck(BOARD *board, COLOR color){
  BITBOARD occupiedFriend = board -> Occupied[color];
  BITBOARD occupiedOpponent = board -> Occupied[!color];
  BITBOARD occupied = occupiedFriend | occupiedOpponent;
  PIECE_TYPE pieceTypeFrom;
  for (pieceTypeFrom = INIT_PIECE_NON_SLIDING; pieceTypeFrom < LAST_PIECE_NON_SLIDING - 1; ++pieceTypeFrom){  /* For all the non sliding pieces */
    BITBOARD fromList = board -> Board[color][pieceTypeFrom]; // For all pieces of a type
    BITBOARD from;
    while ((from = next(&fromList))){                                                                     /* Iterate through the pieces */
      int fromIndex = CONVERT(from);                                                                      /* Convert the bit board to index */
      BITBOARD toList = pseudoMove_nonSliding(color, pieceTypeFrom, fromIndex) &                          /* Pseudo attack, never fails */
                        ~occupiedFriend & occupiedOpponent;                                 /* Only consider captures */
      BITBOARD to;
      while ((to = next(&toList))){                                                                     /* Convert the bit board to index */
        PIECE_TYPE pieceTypeTo = identifyPiece(board, !color, to);
        if (pieceTypeTo == KING){
            return true;
        }
      }
    }
  }
  for (pieceTypeFrom = INIT_PIECE_SLIDING; pieceTypeFrom < LAST_PIECE_SLIDING; ++pieceTypeFrom){          /* For all the sliding pieces */
    BITBOARD fromList = board -> Board[color][pieceTypeFrom];                                             /* Consider all the pieces of the same kind */
    BITBOARD from;
    while ((from = next(&fromList))){                                                                     /* Iterate through the pieces */
      ATTACK_DIRECTION attackDirection;                                                                   /* Attack direction of the sliding pieces */
      int fromIndex = CONVERT(from);                                                                      /* Convert bit board to index */
      for (attackDirection = INIT_ATTACK_DIRECTION_UPWARDS(pieceTypeFrom);                                /* Iterate through all the attack directions */
           attackDirection < LAST_ATTACK_DIRECTION_UPWARDS(pieceTypeFrom); ++attackDirection){            /* in which the piece attack upwards */
        BITBOARD obstacle;
        if ((obstacle = pseudoMove_sliding(attackDirection, fromIndex) & occupied)){                      /* If there are obstacles */
          if ((obstacle = RIGHT(obstacle) & occupiedOpponent)){
            PIECE_TYPE pieceTypeTo;                                                                /* Convert bit board to index */
            pieceTypeTo = identifyPiece(board, !color, obstacle);                                         /* Identify the captured piece */
            if (pieceTypeTo == KING){
                return true;
            }
          }
        }
      }
      for (attackDirection = INIT_ATTACK_DIRECTION_DOWNWARDS(pieceTypeFrom);                              /* Iterate through the attack directions */
           attackDirection < LAST_ATTACK_DIRECTION_DOWNWARDS(pieceTypeFrom); ++attackDirection){          /* in which the piece attack downwards */
        BITBOARD obstacle;
        if ((obstacle = pseudoMove_sliding(attackDirection, fromIndex) & occupied)){                      /* If there are obstacles */
          if ((obstacle = LEFT(obstacle) & occupiedOpponent)){
            PIECE_TYPE pieceTypeTo;                                                           /* Convert bit board to index */
            pieceTypeTo = identifyPiece(board, !color, obstacle);                                         /* Identify the captured piece */
            if (pieceTypeTo == KING){
                return true;
            }
          }
        }
      }
    }
  }
  return false;
}


/** Illegal moves **/
/* If king is captured or castling is done in wrong condition
 * Terminate by returning a very high score by the opponent
 * This will result to know us that node is bad and will not be chosen
 * End the principle variation there
*/

#define KING_CAPTURE_ERROR(pieceType, pv){ \
  if (pieceType == KING){ \
    (pv) -> NumberOfMoves = 0; \
    return INFINITY - 1 - ply; \
  } \
}

#define CASTLING_ERROR(castling, toList, pv){ \
  if (toList & castling){ \
    (pv) -> NumberOfMoves = 0; \
    return INFINITY - 2; \
  } \
}


/** Search **/
/* Searching using negamax with alpha beta pruning
 * We know the fact that whatever situation is good for the other player is bad for us so score negation
 * If there is a better node we store its score
 * If we have a better minimum assured score we store it
  * We store it in the principle variation (it may not be the final principle variation)
  * We copy the principle variation to our parent principle variation
  * If we are at the root node
  * We print the whole principle variation search from there
*/

#define SEARCH(board, enPassant, castling, \
               color, quiescence, alpha, beta, bestValue,  \
               gameValue, \
               gameEvaluation, \
               depth, \
               ply, pv, pvChild, uciData, \
               pvParam){ \
               \
  int value = -negamax((board), (enPassant), (castling), \
                       !(color), (quiescence), -(*(beta)), -(*(alpha)), \
                       (color) == WHITE ? (gameValue) - (gameEvaluation) : \
                                          (gameValue) + (gameEvaluation), \
                       (depth), \
                        (ply) + 1, &(pvChild), (uciData)); \
  if (-value + INFINITY - 3 == ply && kingInCheck(board, !color) == false){ \
    value = -(uciData) -> Score; \
  } \
  if (ply < 3 && (uciData) -> MoveTime != -1 && \
            (uciData) -> MoveTime <= (int)(1000*((double)(clock() - (uciData) -> InitTime) / CLOCKS_PER_SEC)) + 100){ \
                return TIME_OVER; \
  } \
  if (value > *(bestValue)){ \
    *(bestValue) = value; \
  } \
  if (value > *(alpha)){ \
    { pvParam } \
    memcpy((pv) -> Move + 1, (pvChild).Move, (pvChild).NumberOfMoves * sizeof(int)); \
    (pv) -> NumberOfMoves = (pvChild).NumberOfMoves + 1; \
    if ((ply) == 0){ \
      int i; \
      if ((pv) -> NumberOfMoves > 0){ \
        printf("info depth %d seldepth %d score cp %d nodes %lu nps %lu time %u pv ", \
               (uciData) -> Ply, (pv) -> NumberOfMoves, value + (uciData) -> Score, \
               (uciData) -> Nodes, (unsigned long int)((double) (uciData) -> Nodes / \
               ((double)(clock() - (uciData) -> InitTime) / CLOCKS_PER_SEC)), \
               (unsigned)((double)(clock() - (uciData) -> InitTime) / CLOCKS_PER_SEC)); \
        for (i=0; i < (pv) -> NumberOfMoves; ++i){ \
          PRINT_PV_MOVE(->, i); \
        } \
        putchar('\n'); \
        fflush(stdout); \
      } \
    } \
    *(alpha) = value; \
  } \
}


/** Alpha-Beta pruning **/
/* Alpha-Beta pruning
 * If we already have a assured score
 * The current node can result worse condition
 * We no more search the node and simply return the value
*/

#define ALPHA_BETA_CUT_OFF(alpha, beta, bestValue){ \
  if ((alpha) >= (beta)){ \
    return bestValue; \
  } \
}


/** Parameters for principle variation **/
/* A move is stored in a single variable as we know a index will surely fit in 6 bits
 * Storing a move requires 12 bits
 * We turn on another bit in order to signify if it was a promotion
 * We always assume that promotion was a queen (no under promotion supported yet)
*/

#define PV_PARAM(fromIndex, toIndex) { \
  (pv) -> Move[0] = (toIndex) << 6 | (fromIndex); \
}

#define PV_PARAM_PROMOTION(fromIndex, toIndex) { \
  (pv) -> Move[0] = (toIndex) << 6 | (fromIndex) | FLAG_PROMOTION; \
}


/** Search further nodes using negamax search with alpha beta pruning **/

SCORE negamax(BOARD *board, BITBOARD enPassant, BITBOARD castling,                                        /* The board and rights */
              COLOR color, BITBOARD quiescence, SCORE alpha, SCORE beta,                                  /* Search */
              SCORE gameValue,
              int depth, int ply,                                                                         /* Depth */
              PV *pv, UCI_DATA *uciData){                                                                 /* Data */

  PIECE_TYPE pieceTypeFrom;
  BITBOARD pawnList;

  int bestValue;

  /* Storing what places of board are occupied in separate variables */
  BITBOARD occupiedFriend = board -> Occupied[color];
  BITBOARD occupiedOpponent = board -> Occupied[!color];
  BITBOARD occupied = occupiedFriend | occupiedOpponent;

  BITBOARD rookVirgin;
  bool kingVirgin;


  PV pvChild;                                                                                             /* Principle variation of child nodes */


  uciData -> Nodes++;                                                                                     /* Increase node count */


  bestValue = -INFINITY;                                                                                  /* We start from worst possible score */


  if (depth == 0){                                                                                        /* If depth is exhausted */
    pv -> NumberOfMoves = 0;                                                                              /* End of principle variation */

    /* Score is always positive for black */
    return (color == WHITE ? -1 : 1) *                                                                    /* Return relative score */
           gameValue;
  }


  /* Promotion
   * Capture promotion
    * Store all pawns in seventh rank
    * For all such pawns
      * Store how many captures it can do
      * For all such captures
        * Check what piece was captured
        * Terminate if king was captured
        * Play the move
        * Search; if depth one or quiescence search, continue quiescence search
        * Undo the move
   * Movement promotion
    * Store all pawn in seventh rank which can promote
    * For all such pawns
      * Play the move
      * Search; if depth one or quiescence search, continue quiescence search
      * Undo the move
  */
  {
    BITBOARD fromList = board -> Board[color][PAWN] & RANK(color, 7);                                     /* Store all pawns in seventh rank */
    BITBOARD from;
    while ((from = next(&fromList))){                                                                     /* For all such pawns */
      int fromIndex = CONVERT(from);
      BITBOARD toList = pseudoMove_nonSliding(color, PAWN, fromIndex) & occupiedOpponent;                 /* If can capture some piece */
      BITBOARD to;
      while ((to = next(&toList))){                                                                       /* For all capture promotions */
        int toIndex = CONVERT(to);                                                                        /* Convert bit board to index */
        PIECE_TYPE pieceTypeTo = identifyPiece(board, !color, to);                                        /* Check what piece is captured */
        KING_CAPTURE_ERROR(pieceTypeTo, pv);                                                              /* Terminate if king was captured */
        PLAY_MOVE_PROMOTION_CAPTURE(board, QUEEN, color, pieceTypeTo, from, to);                          /* Play the move, promote to queen */
        if (depth == 1 || quiescence){
          SEARCH(board, NO_MOVE, NO_MOVE,
                 color, NO_MOVE, &alpha, &beta, &bestValue,
                 gameValue,
                 promotionCaptureEvaluation(QUEEN, color, pieceTypeTo, fromIndex, toIndex),
                 1, ply, pv, pvChild, uciData,
                 { PV_PARAM_PROMOTION(fromIndex, toIndex) });
        } else {
          SEARCH(board, NO_MOVE, NO_MOVE,
                 color, NO_MOVE, &alpha, &beta, &bestValue,
                 gameValue,
                 promotionCaptureEvaluation(QUEEN, color, pieceTypeTo, fromIndex, toIndex),
                 depth - 1, ply, pv, pvChild, uciData,
                 { PV_PARAM_PROMOTION(fromIndex, toIndex) });
        }
        UNDO_MOVE_PROMOTION_CAPTURE(board, QUEEN, color, pieceTypeTo, from, to);                          /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    }
    if (color == WHITE){                                                                                  /* If player is white */
      fromList = board -> Board[WHITE][PAWN] & RANK(WHITE, 7) & ~(occupied >> BOARD_WIDTH);               /* If it can promote */
      while ((from = next(&fromList))){                                                                   /* For all such pawns */
        int fromIndex = CONVERT(from);                                                                    /* Convert bit board to index */
        BITBOARD to = from << BOARD_WIDTH;                                                                /* See where it will go after promotion */
        int toIndex = CONVERT(to);                                                                        /* Convert bit board to index */
        PLAY_MOVE_PROMOTION_MOVEMENT(board, QUEEN, WHITE, from, to);                                      /* Play the move, promote to queen */
        if (depth == 1 || quiescence){
          SEARCH(board, NO_MOVE, NO_MOVE,
                 WHITE, NO_MOVE, &alpha, &beta, &bestValue,
                 gameValue,
                 promotionMovementEvaluation(QUEEN, WHITE, fromIndex, toIndex),
                 1, ply, pv, pvChild, uciData,
                 { PV_PARAM_PROMOTION(fromIndex, toIndex) });
        } else {
          SEARCH(board, NO_MOVE, NO_MOVE,
                 WHITE, NO_MOVE, &alpha, &beta, &bestValue,
                 gameValue,
                 promotionMovementEvaluation(QUEEN, WHITE, fromIndex, toIndex),
                 depth - 1, ply, pv, pvChild, uciData,
                 { PV_PARAM_PROMOTION(fromIndex, toIndex) });
        }
        UNDO_MOVE_PROMOTION_MOVEMENT(board, QUEEN, WHITE, from, to);                                      /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    } else {                                                                                              /* If player is black */
      fromList = board -> Board[BLACK][PAWN] & RANK(BLACK, 7) & ~(occupied << BOARD_WIDTH);               /* If it can promote */
      while ((from = next(&fromList))){                                                                   /* For all such pawns */
        int fromIndex = CONVERT(from);                                                                    /* Convert bit board to index */
        BITBOARD to = from >> BOARD_WIDTH;                                                                /* See where it will go after promotion */
        int toIndex = CONVERT(to);                                                                        /* Convert bit board to index */
        PLAY_MOVE_PROMOTION_MOVEMENT(board, QUEEN, BLACK, from, to);                                      /* Promote to queen */
        if (depth == 1 || quiescence){
          SEARCH(board, NO_MOVE, NO_MOVE,
                 BLACK, NO_MOVE, &alpha, &beta, &bestValue,
                 gameValue,
                 promotionMovementEvaluation(QUEEN, BLACK, fromIndex, toIndex),
                 1, ply, pv, pvChild, uciData,
                 { PV_PARAM_PROMOTION(fromIndex, toIndex) });
        } else {
          SEARCH(board, NO_MOVE, NO_MOVE,
                 BLACK, NO_MOVE, &alpha, &beta, &bestValue,
                 gameValue,
                 promotionMovementEvaluation(QUEEN, BLACK, fromIndex, toIndex),
                 depth - 1, ply, pv, pvChild, uciData,
                 { PV_PARAM_PROMOTION(fromIndex, toIndex) });
        }
        UNDO_MOVE_PROMOTION_MOVEMENT(board, QUEEN, BLACK, from, to);                                      /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    }
  }

  /* Captures we can take with our non sliding pieces
   * For all non sliding piece
    * For all pieces of the same kind
    * Generate moves - pseudo move & captures (consider only one square in quiescence search)
      * For all captures
        * If king is captured return infinity
        * Play the move
        * Search; if depth one or quiescence search, continue quiescence search
        * Undo the move
  */
  for (pieceTypeFrom = INIT_PIECE_NON_SLIDING; pieceTypeFrom < LAST_PIECE_NON_SLIDING; ++pieceTypeFrom){  /* For all the non sliding pieces */
    BITBOARD fromList = board -> Board[color][pieceTypeFrom] &
                        (pieceTypeFrom == PAWN ? ~RANK(color, 7) : ~NO_MOVE);                             /* Consider all the pieces of the same kind */
    BITBOARD from;
    while ((from = next(&fromList))){                                                                     /* Iterate through the pieces */
      int fromIndex = CONVERT(from);                                                                      /* Convert the bit board to index */
      BITBOARD toList = pseudoMove_nonSliding(color, pieceTypeFrom, fromIndex) &                          /* Pseudo attack, never fails */
                        ~occupiedFriend & occupiedOpponent & ~quiescence;                                 /* Only consider captures */
      BITBOARD to;
      CASTLING_ERROR(castling, toList, pv);
      while ((to = next(&toList))){                                                                       /* Iterate through the captures */
        int toIndex = CONVERT(to);                                                                        /* Convert the bit board to index */
        PIECE_TYPE pieceTypeTo = identifyPiece(board, !color, to);                                        /* Identify the captured piece */
        KING_CAPTURE_ERROR(pieceTypeTo, pv);                                                              /* Terminate if king was captured */
        PLAY_MOVE_CAPTURE(board, &rookVirgin, &kingVirgin, color, pieceTypeFrom, pieceTypeTo, from, to);  /* Play the move */
        if (depth == 1 || quiescence){
          SEARCH(board, NO_MOVE, NO_MOVE,
                 color, ~to, &alpha, &beta, &bestValue,
                 gameValue,
                 captureEvaluation(color,  pieceTypeFrom, pieceTypeTo, fromIndex, toIndex),
                 1, ply, pv, pvChild, uciData,
                 { PV_PARAM(fromIndex, toIndex) });
        } else {
          SEARCH(board, NO_MOVE, NO_MOVE,
                 color, NO_MOVE, &alpha, &beta, &bestValue,
                 gameValue, captureEvaluation(color, pieceTypeFrom, pieceTypeTo, fromIndex, toIndex),
                 depth - 1, ply, pv, pvChild, uciData,
                 { PV_PARAM(fromIndex, toIndex) });
        }
        UNDO_MOVE_CAPTURE(board, &rookVirgin, &kingVirgin, color, pieceTypeFrom, pieceTypeTo, from, to);  /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    }
  }

  /* Captures we can take with our sliding pieces
   * For all sliding pieces
    * For each of the piece
      * For all upward attack
        * If there are obstacles and the rightmost obstacle is a capture
        * Identify the captured piece
        * Play the move
        * Search; if depth one or quiescence search, continue quiescence search
        * Undo the move
      * For all downward attack
        * If there are obstacles and the leftmost obstacle is a capture
        * Identify the capture piece
        * Play the move
        * Search; if depth one or quiescence search, continue quiescence search
        * Undo the move
  */
  for (pieceTypeFrom = INIT_PIECE_SLIDING; pieceTypeFrom < LAST_PIECE_SLIDING; ++pieceTypeFrom){          /* For all the sliding pieces */
    BITBOARD fromList = board -> Board[color][pieceTypeFrom];                                             /* Consider all the pieces of the same kind */
    BITBOARD from;
    while ((from = next(&fromList))){                                                                     /* Iterate through the pieces */
      ATTACK_DIRECTION attackDirection;                                                                   /* Attack direction of the sliding pieces */
      int fromIndex = CONVERT(from);                                                                      /* Convert bit board to index */
      for (attackDirection = INIT_ATTACK_DIRECTION_UPWARDS(pieceTypeFrom);                                /* Iterate through all the attack directions */
           attackDirection < LAST_ATTACK_DIRECTION_UPWARDS(pieceTypeFrom); ++attackDirection){            /* in which the piece attack upwards */
        BITBOARD obstacle;
        if ((obstacle = pseudoMove_sliding(attackDirection, fromIndex) & occupied)){                      /* If there are obstacles */
          if ((obstacle = RIGHT(obstacle) & occupiedOpponent & ~quiescence)){                             /* If the right most obstacle is a capture */
            int toIndex;
            PIECE_TYPE pieceTypeTo;
            CASTLING_ERROR(castling, obstacle, pv);
            toIndex = CONVERT(obstacle);                                                                  /* Convert bit board to index */
            pieceTypeTo = identifyPiece(board, !color, obstacle);                                         /* Identify the captured piece */
            KING_CAPTURE_ERROR(pieceTypeTo, pv);
            PLAY_MOVE_CAPTURE(board, &rookVirgin, &kingVirgin,                                            /* Play the move */
                              color, pieceTypeFrom, pieceTypeTo, from, obstacle);
            if (depth == 1 || quiescence){
              SEARCH(board, NO_MOVE, NO_MOVE,
                     color, ~obstacle, &alpha, &beta, &bestValue,
                     gameValue,
                     captureEvaluation(color, pieceTypeFrom, pieceTypeTo, fromIndex, toIndex),
                     1, ply, pv, pvChild, uciData,
                     { PV_PARAM(fromIndex, toIndex) });
            } else {
              SEARCH(board, NO_MOVE, NO_MOVE,
                     color, NO_MOVE, &alpha, &beta, &bestValue,
                     gameValue,
                     captureEvaluation(color, pieceTypeFrom, pieceTypeTo, fromIndex, toIndex),
                     depth - 1, ply, pv, pvChild, uciData,
                     { PV_PARAM(fromIndex, toIndex) });
            }
            UNDO_MOVE_CAPTURE(board, &rookVirgin, &kingVirgin,
                              color, pieceTypeFrom, pieceTypeTo, from, obstacle);
            ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);                                                   /* Undo the move */
          }
        }
      }
      for (attackDirection = INIT_ATTACK_DIRECTION_DOWNWARDS(pieceTypeFrom);                              /* Iterate through the attack directions */
           attackDirection < LAST_ATTACK_DIRECTION_DOWNWARDS(pieceTypeFrom); ++attackDirection){          /* in which the piece attack downwards */
        BITBOARD obstacle;
        if ((obstacle = pseudoMove_sliding(attackDirection, fromIndex) & occupied)){                      /* If there are obstacles */
          if ((obstacle = LEFT(obstacle) & occupiedOpponent & ~quiescence)){                              /* If the left most obstacle is a capture */
            int toIndex;
            PIECE_TYPE pieceTypeTo;
            CASTLING_ERROR(castling, obstacle, pv);
            toIndex = CONVERT(obstacle);                                                                  /* Convert bit board to index */
            pieceTypeTo = identifyPiece(board, !color, obstacle);                                         /* Identify the captured piece */
            KING_CAPTURE_ERROR(pieceTypeTo, pv);
            PLAY_MOVE_CAPTURE(board, &rookVirgin, &kingVirgin,                                            /* Play the move */
                              color, pieceTypeFrom, pieceTypeTo, from, obstacle);
            if (depth == 1 || quiescence){
              SEARCH(board, NO_MOVE, NO_MOVE,
                     color, ~obstacle, &alpha, &beta, &bestValue,
                     gameValue,
                     captureEvaluation(color, pieceTypeFrom, pieceTypeTo, fromIndex, toIndex),
                     1, ply, pv, pvChild, uciData,
                     { PV_PARAM(fromIndex, toIndex) });
            } else {
              SEARCH(board, NO_MOVE, NO_MOVE,
                     color, NO_MOVE, &alpha, &beta, &bestValue,
                     gameValue,
                     captureEvaluation(color, pieceTypeFrom, pieceTypeTo, fromIndex, toIndex),
                     depth - 1, ply, pv, pvChild, uciData,
                     { PV_PARAM(fromIndex, toIndex) });
            }
            UNDO_MOVE_CAPTURE(board, &rookVirgin, &kingVirgin,
                              color, pieceTypeFrom, pieceTypeTo, from, obstacle);
            ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);                                                   /* Undo the move */
          }
        }
      }
    }
  }

  /* Castling */
  if (color == WHITE){
    if (board -> KingVirgin[WHITE] &&
        (board -> RookVirgin[WHITE] & AN('h', '1')) &&
        !(occupied & (AN('f', '1') | AN('g', '1'))) &&
        (board -> Board[WHITE][KING] & AN('e', '1')) &&
        (board -> Board[WHITE][ROOK] & AN('h', '1'))){
      board -> KingVirgin[WHITE] = false;
      board -> RookVirgin[WHITE] &= ~AN('h', '1');
      board -> Board[WHITE][KING] = AN('g', '1');
      board -> Board[WHITE][ROOK] ^= AN('h', '1') | AN('f', '1');
      board -> Occupied[WHITE] ^= AN('h', '1') | AN('f', '1') | AN('g', '1') | AN('e', '1');
      SEARCH(board, 3, AN('e', '1') | AN('f', '1'),
             color, NO_MOVE, &alpha, &beta, &bestValue,
             gameValue,
             castling_OO_Evaluation(WHITE),
             depth - 1, ply, pv, pvChild, uciData,
             { pv -> Move[0] = 62 << 6 | 60; });
      board -> KingVirgin[WHITE] = true;
      board -> RookVirgin[WHITE] |= AN('h', '1');
      board -> Board[WHITE][KING] = AN('e', '1');
      board -> Board[WHITE][ROOK] ^= AN('h', '1') | AN('f', '1');
      board -> Occupied[WHITE] ^= AN('h', '1') | AN('f', '1') | AN('g', '1') | AN('e', '1');
      ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
    }
    if (board -> KingVirgin[WHITE] &&
        (board -> RookVirgin[WHITE] & AN('a', '1')) &&
        !(occupied & (AN('b', '1') | AN('c', '1') | AN('d', '1'))) &&
        (board -> Board[WHITE][KING] & AN('e', '1')) &&
        (board -> Board[WHITE][ROOK] & AN('a', '1'))){
      board -> KingVirgin[WHITE] = false;
      board -> RookVirgin[WHITE] &= ~AN('a', '1');
      board -> Board[WHITE][KING] = AN('c', '1');
      board -> Board[WHITE][ROOK] ^= AN('a', '1') | AN('d', '1');
      board -> Occupied[WHITE] ^= AN('a', '1') | AN('d', '1') | AN('c', '1') | AN('e', '1');
      SEARCH(board, NO_MOVE, AN('e', '1') | AN('d', '1'),
             color, NO_MOVE, &alpha, &beta, &bestValue,
             gameValue,
             castling_OOO_Evaluation(WHITE),
             depth - 1, ply, pv, pvChild, uciData,
             { pv -> Move[0] = 58 << 6 | 60; });
      board -> KingVirgin[WHITE] = true;
      board -> RookVirgin[WHITE] |= AN('a', '1');
      board -> Board[WHITE][KING] = AN('e', '1');
      board -> Board[WHITE][ROOK] ^= AN('a', '1') | AN('d', '1');
      board -> Occupied[WHITE] ^= AN('a', '1') | AN('d', '1') | AN('c', '1') | AN('e', '1');
      ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
    }
  } else {
    if (board -> KingVirgin[BLACK] &&
        (board -> RookVirgin[BLACK] & AN('h', '8')) &&
        !(occupied & (AN('f', '8') | AN('g', '8'))) &&
        (board -> Board[BLACK][KING] & AN('e', '8')) &&
        (board -> Board[BLACK][ROOK] & AN('h', '8'))){
      board -> KingVirgin[BLACK] = false;
      board -> RookVirgin[BLACK] &= ~AN('h', '8');
      board -> Board[BLACK][KING] = AN('g', '8');
      board -> Board[BLACK][ROOK] ^= AN('h', '8') | AN('f', '8');
      board -> Occupied[BLACK] ^= AN('h', '8') | AN('f', '8') | AN('g', '8') | AN('e', '8');
      SEARCH(board, NO_MOVE, AN('e', '8') | AN('f', '8'),
             color, NO_MOVE, &alpha, &beta, &bestValue,
             gameValue,
             castling_OO_Evaluation(BLACK),
             depth - 1, ply, pv, pvChild, uciData,
             { pv -> Move[0] = 6 << 6 | 4; });
      board -> KingVirgin[BLACK] = true;
      board -> RookVirgin[BLACK] |= AN('h', '8');
      board -> Board[BLACK][KING] = AN('e', '8');
      board -> Board[BLACK][ROOK] ^= AN('h', '8') | AN('f', '8');
      board -> Occupied[BLACK] ^= AN('h', '8') | AN('f', '8') | AN('g', '8') | AN('e', '8');
      ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
    }
    if (board -> KingVirgin[BLACK] &&
        (board -> RookVirgin[BLACK] & AN('a', '8')) &&
        !(occupied & (AN('b', '8') | AN('c', '8') | AN('d', '8'))) &&
        (board -> Board[BLACK][KING] & AN('e', '8')) &&
        (board -> Board[BLACK][ROOK] & AN('a', '8'))){
      board -> KingVirgin[BLACK] = false;
      board -> RookVirgin[BLACK] &= ~AN('a', '8');
      board -> Board[BLACK][KING] = AN('c', '8');
      board -> Board[BLACK][ROOK] ^= AN('a', '8') | AN('d', '8');
      board -> Occupied[BLACK] ^= AN('a', '8') | AN('d', '8') | AN('c', '8') | AN('e', '8');
      SEARCH(board, NO_MOVE, AN('e', '8') | AN('d', '8'),
             color, NO_MOVE, &alpha, &beta, &bestValue,
             gameValue,
             castling_OOO_Evaluation(BLACK),
             depth - 1, ply, pv, pvChild, uciData,
             { pv -> Move[0] = 2 << 6 | 4; });
      board -> KingVirgin[BLACK] = true;
      board -> RookVirgin[BLACK] |= AN('a', '8');
      board -> Board[BLACK][KING] = AN('e', '8');
      board -> Board[BLACK][ROOK] ^= AN('a', '8') | AN('d', '8');
      board -> Occupied[BLACK] ^= AN('a', '8') | AN('d', '8') | AN('c', '8') | AN('e', '8');
      ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
    }
  }

  /* En passant capture
   * If en passant is there and there are pawns in fifth rank
   * Calculate the destination of the pawn
   * See if any of our pawns can attack that location
   * For all such pawns
    * Play the move
    * Search
    * Undo the move
  */
  if (enPassant && (board -> Board[color][PAWN] & RANK(color, 5))){                                       /* If en passant and we have pawn in fifth */
    int enPassantIndex = CONVERT(enPassant);
    if (color == WHITE){                                                                                  /* If color is white */
      BITBOARD to = enPassant << BOARD_WIDTH;                                                             /* Calculate the destination */
      int toIndex = CONVERT(to);                                                                          /* Convert bit board to index */
      BITBOARD fromList = board -> Board[WHITE][PAWN] & pseudoMove_nonSliding(BLACK, PAWN, toIndex);      /* See what pawns can attack the destination */
      BITBOARD from;
      while ((from = next(&fromList))){                                                                   /* For all such pawns */
        int fromIndex = CONVERT(from);                                                                    /* Convert bit board to index */
        PLAY_MOVE_EN_PASSANT(board, enPassant, WHITE, from, to);                                          /* Play the move */
        SEARCH(board, NO_MOVE, NO_MOVE,
               WHITE, NO_MOVE, &alpha, &beta, &bestValue,
               gameValue,
               enPassantEvaluation(enPassantIndex, WHITE, fromIndex, toIndex),
               depth - 1, ply, pv, pvChild, uciData,
               { PV_PARAM(fromIndex, toIndex) });
        UNDO_MOVE_EN_PASSANT(board, enPassant, WHITE, from, to);                                          /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    } else {                                                                                              /* If the color is black */
      BITBOARD to = enPassant >> BOARD_WIDTH;                                                             /* Calculate the destination */
      int toIndex = CONVERT(to);                                                                          /* Convert the bit board to index */
      BITBOARD fromList = board -> Board[BLACK][PAWN] & pseudoMove_nonSliding(WHITE, PAWN, toIndex);      /* See pawns can attack the destination */
      BITBOARD from;
      while ((from = next(&fromList))){                                                                   /* For all such pawns */
        int fromIndex = CONVERT(from);                                                                    /* Convert bit board to index */
        PLAY_MOVE_EN_PASSANT(board, enPassant, BLACK, from, to);                                          /* Play the move */
        SEARCH(board, NO_MOVE, NO_MOVE,
               BLACK, NO_MOVE, &alpha, &beta, &bestValue,
               gameValue,
               enPassantEvaluation(enPassantIndex, BLACK, fromIndex, toIndex),
               depth - 1, ply, pv, pvChild, uciData,
               { PV_PARAM(fromIndex, toIndex) });
        UNDO_MOVE_EN_PASSANT(board, enPassant, BLACK, from, to);                                          /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    }
  }

  pawnList = board -> Board[color][PAWN] & (FILE_D | FILE_E) & ~RANK(color, 7);                           /* All the center pawns */

  /* Double moves of all center pawns
   * For all center pawn which not in seventh rank (we already did calculated it) which can do a double move
    * Check where it will go and play the move
    * Search
    * Undo the move
  */
  {
    BITBOARD fromList;
    if (color == WHITE){                                                                                  /* If the color is white */
      BITBOARD from;
      fromList = pawnList & RANK(WHITE, 2) &                                                              /* If the pawn is in second rank */
                 ~(occupied >> BOARD_WIDTH | occupied >> (BOARD_WIDTH * 2));                              /* And no obstacles in its way */
      while ((from = next(&fromList))){                                                                   /* For all capable pawns */
        int fromIndex = CONVERT(from);                                                                    /* Convert bit board to index */
        BITBOARD to = from << (BOARD_WIDTH * 2);                                                          /* Calculate where it will go */
        int toIndex = CONVERT(to);                                                                        /* Convert bit board to index */
        PLAY_MOVE(board, &rookVirgin, &kingVirgin, WHITE, PAWN, from, to);                                /* Play the move */
        SEARCH(board, to, NO_MOVE,
               WHITE, NO_MOVE, &alpha, &beta, &bestValue,
               gameValue,
               movementEvaluation(WHITE, PAWN, fromIndex, toIndex),
               depth - 1, ply, pv, pvChild, uciData,
               { PV_PARAM(fromIndex, toIndex) });
        UNDO_MOVE(board, &rookVirgin, &kingVirgin, WHITE, PAWN, from, to);                                /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    } else {
      BITBOARD from;
      fromList = pawnList & RANK(BLACK, 2) &                                                              /* If the pawn is in second rank */
                 ~(occupied << BOARD_WIDTH | occupied << (BOARD_WIDTH * 2));                              /* And there are no obstacles */
      while ((from = next(&fromList))){                                                                   /* For every capable pawn */
        int fromIndex = CONVERT(from);                                                                    /* Convert bit board to index */
        BITBOARD to = from >> (BOARD_WIDTH * 2);                                                          /* Calculate destination */
        int toIndex = CONVERT(to);                                                                        /* Convert bit board to index */
        PLAY_MOVE(board, &rookVirgin, &kingVirgin, BLACK, PAWN, from, to);                                /* Play the move */
        SEARCH(board, to, NO_MOVE,
               BLACK, NO_MOVE, &alpha, &beta, &bestValue,
               gameValue,
               movementEvaluation(BLACK, PAWN, fromIndex, toIndex),
               depth - 1, ply, pv, pvChild, uciData,
               { PV_PARAM(fromIndex, toIndex) });
        UNDO_MOVE(board, &rookVirgin, &kingVirgin, BLACK, PAWN, from, to);                                /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    }
  }

  /* Knights which can control the center
   * For all our knights
    * Destination - pseudo & center & no capture
    * For all destinations
      * Play the move
      * Search
      * Undo the move
  */
  {
    BITBOARD fromList = board -> Board[color][KNIGHT];                                                    /* Consider all of our knights */
    BITBOARD from;
    while ((from = next(&fromList))){                                                                     /* For each of them */
      int fromIndex = CONVERT(from);                                                                      /* Convert bit board to index */
      BITBOARD toList = pseudoMove_nonSliding(color, KNIGHT, fromIndex) &                                 /* Pseudo attack, never fails */
                        ~occupied &                                                                       /* Do not consider captures */
                        pseudoCenter_knight(color);                                                       /* Consider only center control moves */
      BITBOARD to;
      while ((to = next(&toList))){                                                                       /* For every possible good destination */
        int toIndex = CONVERT(to);                                                                        /* Convert bit board to index */
        PLAY_MOVE(board, &rookVirgin, &kingVirgin, color, KNIGHT, from, to);                              /* Play the move */
        SEARCH(board, NO_MOVE, NO_MOVE,
               color, NO_MOVE, &alpha, &beta, &bestValue,
               gameValue,
               movementEvaluation(color, KNIGHT, fromIndex, toIndex),
               depth - 1, ply, pv, pvChild, uciData,
               { PV_PARAM(fromIndex, toIndex) });
        UNDO_MOVE(board, &rookVirgin, &kingVirgin, color, KNIGHT, from, to);                              /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    }
  }

  /* Sliding pieces which can control the center
   * For all sliding pieces except queen
    * For all pieces of the same kind
      * For all upwards attack
        * If there is an attack which exists
          * If there are obstacles
            * Everything right to the rightmost obstacle & center should be considered
          * Else if no obstacles
            * Everything & center should be considered
          * For all destinations
            * Play the move
            * Search
            * Undo the move
      * For all downwards attack
        * If there is an attack which exists
          * If there are obstacles
            * Everything left to the leftmost obstacle & center should be considered
          * Else if no obstacles
            * Everything & center should be considered
          * For all destinations
            * Play the move
            * Search
            * Undo the move
  */
  for (pieceTypeFrom = INIT_PIECE_SLIDING; pieceTypeFrom < QUEEN; ++pieceTypeFrom){                       /* For all the sliding pieces */
    BITBOARD fromList = board -> Board[color][pieceTypeFrom];                                             /* Consider all the pieces of the same kind */
    BITBOARD from;
    while ((from = next(&fromList))){                                                                     /* Iterate through the pieces */
      ATTACK_DIRECTION attackDirection;                                                                   /* Attack direction of the sliding pieces */
      int fromIndex = CONVERT(from);                                                                      /* Convert bit board to index */
      for (attackDirection = INIT_ATTACK_DIRECTION_UPWARDS(pieceTypeFrom);                                /* Iterate through all the attack directions */
           attackDirection < LAST_ATTACK_DIRECTION_UPWARDS(pieceTypeFrom); ++attackDirection){            /* in which the piece attack upwards */
        BITBOARD attack;
        if ((attack = pseudoMove_sliding(attackDirection, fromIndex))){                                   /* If attack exists */
          BITBOARD obstacle;
          BITBOARD toList;
          BITBOARD to;
          if ((obstacle = attack & occupied)){                                                            /* If there are obstacles */
            toList = (RIGHT(obstacle) - 1) &                                                              /* Every square which is right of obstacle */
                      attack & pseudoCenter_sliding(color, pieceTypeFrom);                                /* If it falls in attack and in center */
          } else {                                                                                        /* Else if no obstacles */
            toList = attack & pseudoCenter_sliding(color, pieceTypeFrom);                                 /* We can go anywhere */
          }
          while ((to = next(&toList))){                                                                   /* For all the destination in the list */
            int toIndex = CONVERT(to);                                                                    /* Convert bit board to index */
            PLAY_MOVE(board, &rookVirgin, &kingVirgin, color, pieceTypeFrom, from, to);                   /* Play the move */
            SEARCH(board, NO_MOVE, NO_MOVE,
                   color, NO_MOVE, &alpha, &beta, &bestValue,
                   gameValue,
                   movementEvaluation(color, pieceTypeFrom, fromIndex, toIndex),
                   depth - 1, ply, pv, pvChild, uciData,
                   { PV_PARAM(fromIndex, toIndex) });
            UNDO_MOVE(board, &rookVirgin, &kingVirgin, color, pieceTypeFrom, from, to);                    /* Undo the move */
            ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
          }
        }
      }
      for (attackDirection = INIT_ATTACK_DIRECTION_DOWNWARDS(pieceTypeFrom);                              /* Iterate through all the attack directions */
           attackDirection < LAST_ATTACK_DIRECTION_DOWNWARDS(pieceTypeFrom); ++attackDirection){          /* in which the piece attack upwards */
        BITBOARD attack;
        if ((attack = pseudoMove_sliding(attackDirection, fromIndex))){                                   /* If there is a attack which exists */
          BITBOARD obstacle;
          BITBOARD toList;
          BITBOARD to;
          if ((obstacle = attack & occupied)){                                                            /* If there are obstacles */
            toList = LEFT(obstacle);                                                                      /* Consider the leftmost obstacle */
            toList = ~((toList - 1) | toList) &                                                           /* Do not consider obstacle and right of it */
                     attack & pseudoCenter_sliding(color, pieceTypeFrom);                                 /* Consider rest of attack and in center */
          } else {
            toList = attack & pseudoCenter_sliding(color, pieceTypeFrom);                                 /* Consider the whole attack if no obstacle */
          }
          while ((to = next(&toList))){
            int toIndex = CONVERT(to);                                                                    /* Convert bit board to index */
            PLAY_MOVE(board, &rookVirgin, &kingVirgin, color, pieceTypeFrom, from, to);                   /* Play the move */
            SEARCH(board, NO_MOVE, NO_MOVE,
                   color, NO_MOVE, &alpha, &beta, &bestValue,
                   gameValue,
                   movementEvaluation(color, pieceTypeFrom, fromIndex, toIndex),
                   depth - 1, ply, pv, pvChild, uciData,
                   { PV_PARAM(fromIndex, toIndex) });
            UNDO_MOVE(board, &rookVirgin, &kingVirgin, color, pieceTypeFrom, from, to);                    /* Undo the move */
            ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
          }
        }
      }
    }
  }

  /* Single moves of all center pawns
   * For all center pawn which not in seventh rank (we already did calculated it) which can do a single move
    * Check where it will go and play the move
    * Search
    * Undo the move
  */
  {
    BITBOARD fromList;
    if (color == WHITE){                                                                                  /* If color is white */
      BITBOARD from;
      fromList = pawnList & ~(occupied >> BOARD_WIDTH);                                                   /* If no obstacles in the path */
      while ((from = next(&fromList))){                                                                   /* For all capable pawns */
        int fromIndex = CONVERT(from);                                                                    /* Convert bit board to index */
        BITBOARD to = from << BOARD_WIDTH;                                                                /* Calculate destination */
        int toIndex = CONVERT(to);                                                                        /* Convert bit board to index */
        PLAY_MOVE(board, &rookVirgin, &kingVirgin, WHITE, PAWN, from, to);                                /* Play the move */
        SEARCH(board, NO_MOVE, NO_MOVE,
               WHITE, NO_MOVE, &alpha, &beta, &bestValue,
               gameValue,
               movementEvaluation(WHITE, PAWN, fromIndex, toIndex),
               depth - 1, ply, pv, pvChild, uciData,
               { PV_PARAM(fromIndex, toIndex) });
        UNDO_MOVE(board, &rookVirgin, &kingVirgin, WHITE, PAWN, from, to);                                /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    } else {                                                                                              /* If color is black */
      BITBOARD from;
      fromList = pawnList & ~(occupied << BOARD_WIDTH);                                                   /* If no obstacles in the path */
      while ((from = next(&fromList))){                                                                   /* For all capable pawns */
        int fromIndex = CONVERT(from);                                                                    /* Convert bit board to index */
        BITBOARD to = from >> BOARD_WIDTH;                                                                /* Calculate destination */
        int toIndex = CONVERT(to);                                                                        /* Convert bit board to index */
        PLAY_MOVE(board, &rookVirgin, &kingVirgin, BLACK, PAWN, from, to);                                /* Play the move */
        SEARCH(board, NO_MOVE, NO_MOVE,
               BLACK, NO_MOVE, &alpha, &beta, &bestValue,
               gameValue,
               movementEvaluation(BLACK, PAWN, fromIndex, toIndex),
               depth - 1, ply, pv, pvChild, uciData,
               { PV_PARAM(fromIndex, toIndex) });
        UNDO_MOVE(board, &rookVirgin, &kingVirgin, BLACK, PAWN, from, to);                                /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    }
  }

  /* For all non sliding pieces except pawn
    * For all pieces
      * For all pieces of the same kind
        * Destination - pseudo & no center & no capture
        * For all destinations
          * Play the move
          * Search
          * Undo the move
  */
  for (pieceTypeFrom = KNIGHT; pieceTypeFrom < LAST_PIECE_NON_SLIDING; ++pieceTypeFrom){                  /* For knights and our king */
    BITBOARD fromList = board -> Board[color][pieceTypeFrom];                                             /* Consider all the pieces of the same kind */
    BITBOARD from;
    while ((from = next(&fromList))){                                                                     /* Iterate through the pieces */
      int fromIndex = CONVERT(from);                                                                      /* Convert bit board to index */
      BITBOARD toList = pseudoMove_nonSliding(color, pieceTypeFrom, fromIndex) &                          /* Pseudo attack, never fails */
                        ~occupied &                                                                       /* Do not consider captures */
                        (pieceTypeFrom & KNIGHT ? ~pseudoCenter_knight(color) : ~NO_MOVE);                /* Do not consider center moves for knight */
      BITBOARD to;
      CASTLING_ERROR(castling, toList, pv);
      while ((to = next(&toList))){                                                                       /* Iterate through the moves */
        int toIndex = CONVERT(to);                                                                        /* Convert bit board to index */
        PLAY_MOVE(board, &rookVirgin, &kingVirgin, color, pieceTypeFrom, from, to);                       /* Play the move */
        SEARCH(board, NO_MOVE, NO_MOVE,
               color, NO_MOVE, &alpha, &beta, &bestValue,
               gameValue,
               movementEvaluation(color, pieceTypeFrom, fromIndex, toIndex),
               depth - 1, ply, pv, pvChild, uciData,
               { PV_PARAM(fromIndex, toIndex) });
        UNDO_MOVE(board, &rookVirgin, &kingVirgin, color, pieceTypeFrom, from, to);                       /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    }
  }

  pawnList = board -> Board[color][PAWN] & ~(FILE_D | FILE_E) & ~RANK(color, 7);                          /* All the except center pawns */

  /* Double moves of all except center pawns
   * For all except center pawn which not in seventh rank (we already did calculated it) which can do a double move
    * Check where it will go and play the move
    * Search
    * Undo the move
  */
  {
    BITBOARD fromList;
    if (color == WHITE){                                                                                  /* If the color is white */
      BITBOARD from;
      fromList = pawnList & RANK(WHITE, 2) &                                                              /* If the pawn is in second rank */
                 ~(occupied >> BOARD_WIDTH | occupied >> (BOARD_WIDTH * 2));                              /* And no obstacles in its way */
      while ((from = next(&fromList))){                                                                   /* For all capable pawns */
        int fromIndex = CONVERT(from);                                                                    /* Convert bit board to index */
        BITBOARD to = from << (BOARD_WIDTH * 2);                                                          /* Calculate where it will go */
        int toIndex = CONVERT(to);                                                                        /* Convert bit board to index */
        PLAY_MOVE(board, &rookVirgin, &kingVirgin, WHITE, PAWN, from, to);                                /* Play the move */
        SEARCH(board, to, NO_MOVE,
               WHITE, NO_MOVE, &alpha, &beta, &bestValue,
               gameValue,
               movementEvaluation(WHITE, PAWN, fromIndex, toIndex),
               depth - 1, ply, pv, pvChild, uciData,
               { PV_PARAM(fromIndex, toIndex) });
        UNDO_MOVE(board, &rookVirgin, &kingVirgin, WHITE, PAWN, from, to);                                /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    } else {
      BITBOARD from;
      fromList = pawnList & RANK(BLACK, 2) &                                                              /* If the pawn is in second rank */
                 ~(occupied << BOARD_WIDTH | occupied << (BOARD_WIDTH * 2));                              /* And there are no obstacles */
      while ((from = next(&fromList))){                                                                   /* For every capable pawn */
        int fromIndex = CONVERT(from);                                                                    /* Convert bit board to index */
        BITBOARD to = from >> (BOARD_WIDTH * 2);                                                          /* Calculate destination */
        int toIndex = CONVERT(to);                                                                        /* Convert bit board to index */
        PLAY_MOVE(board, &rookVirgin, &kingVirgin, BLACK, PAWN, from, to);                                /* Play the move */
        SEARCH(board, to, NO_MOVE,
               BLACK, NO_MOVE, &alpha, &beta, &bestValue,
               gameValue,
               movementEvaluation(BLACK, PAWN, fromIndex, toIndex),
               depth - 1, ply, pv, pvChild, uciData,
               { PV_PARAM(fromIndex, toIndex) });
        UNDO_MOVE(board, &rookVirgin, &kingVirgin, BLACK, PAWN, from, to);                                /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    }
  }

  /* Sliding pieces except which can control the center
   * For all sliding pieces except queen
    * For all pieces of the same kind
      * For all upwards attack
        * If there is an attack which exists
          * If there are obstacles
            * Everything right to the rightmost obstacle & no center should be considered
          * Else if no obstacles
            * Everything & no center should be considered
          * For all destinations
            * Play the move
            * Search
            * Undo the move
      * For all downwards attack
        * If there is an attack which exists
          * If there are obstacles
            * Everything left to the leftmost obstacle & no center should be considered
          * Else if no obstacles
            * Everything & no center should be considered
          * For all destinations
            * Play the move
            * Search
            * Undo the move
  */
  for (pieceTypeFrom = INIT_PIECE_SLIDING; pieceTypeFrom < LAST_PIECE_SLIDING; ++pieceTypeFrom){          /* For all the sliding pieces */
    BITBOARD fromList = board -> Board[color][pieceTypeFrom];                                             /* Consider all the pieces of the same kind */
    BITBOARD from;
    while ((from = next(&fromList))){                                                                     /* Iterate through the pieces */
      ATTACK_DIRECTION attackDirection;                                                                   /* Attack direction of the sliding pieces */
      int fromIndex = CONVERT(from);                                                                      /* Convert bit board to index */
      for (attackDirection = INIT_ATTACK_DIRECTION_UPWARDS(pieceTypeFrom);                                /* Iterate through all the attack directions */
           attackDirection < LAST_ATTACK_DIRECTION_UPWARDS(pieceTypeFrom); ++attackDirection){            /* in which the piece attack upwards */
        BITBOARD attack;
        if ((attack = pseudoMove_sliding(attackDirection, fromIndex))){                                   /* If attack exists */
          BITBOARD obstacle;
          BITBOARD toList;
          BITBOARD to;
          if ((obstacle = attack & occupied)){                                                            /* If there are obstacles */
            toList = (RIGHT(obstacle) - 1) &                                                              /* Every square which is right of obstacle */
                      attack & ~pseudoCenter_sliding(color, pieceTypeFrom);                               /* If it falls in attack and in center */
          } else {                                                                                        /* Else if no obstacles */
            toList = attack & ~pseudoCenter_sliding(color, pieceTypeFrom);                                /* We can go anywhere */
          }
          CASTLING_ERROR(castling, toList, pv);
          while ((to = next(&toList))){                                                                   /* For all the destination in the list */
            int toIndex = CONVERT(to);                                                                    /* Convert bit board to index */
            PLAY_MOVE(board, &rookVirgin, &kingVirgin, color, pieceTypeFrom, from, to);                   /* Play the move */
            SEARCH(board, NO_MOVE, NO_MOVE,
                   color, NO_MOVE, &alpha, &beta, &bestValue,
                   gameValue,
                   movementEvaluation(color, pieceTypeFrom, fromIndex, toIndex),
                   depth - 1, ply, pv, pvChild, uciData,
                   { PV_PARAM(fromIndex, toIndex) });
            UNDO_MOVE(board, &rookVirgin, &kingVirgin, color, pieceTypeFrom, from, to);                    /* Undo the move */
            ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
          }
        }
      }
      for (attackDirection = INIT_ATTACK_DIRECTION_DOWNWARDS(pieceTypeFrom);                              /* Iterate through all the attack directions */
           attackDirection < LAST_ATTACK_DIRECTION_DOWNWARDS(pieceTypeFrom); ++attackDirection){          /* in which the piece attack upwards */
        BITBOARD attack;
        if ((attack = pseudoMove_sliding(attackDirection, fromIndex))){                                   /* If there is a attack which exists */
          BITBOARD obstacle;
          BITBOARD toList;
          BITBOARD to;
          if ((obstacle = attack & occupied)){                                                            /* If there are obstacles */
            toList = LEFT(obstacle);                                                                      /* Consider the leftmost obstacle */
            toList = ~((toList - 1) | toList) &                                                           /* Do not consider obstacle and right of it */
                     attack & ~pseudoCenter_sliding(color, pieceTypeFrom);                                /* Consider rest of attack and in center */
          } else {
            toList = attack & ~pseudoCenter_sliding(color, pieceTypeFrom);                                /* Consider the whole attack if no obstacle */
          }
          CASTLING_ERROR(castling, toList, pv);
          while ((to = next(&toList))){
            int toIndex = CONVERT(to);                                                                    /* Convert bit board to index */
            PLAY_MOVE(board, &rookVirgin, &kingVirgin, color, pieceTypeFrom, from, to);                   /* Play the move */
            SEARCH(board, NO_MOVE, NO_MOVE,
                   color, NO_MOVE, &alpha, &beta, &bestValue,
                   gameValue,
                   movementEvaluation(color, pieceTypeFrom, fromIndex, toIndex),
                   depth - 1, ply, pv, pvChild, uciData,
                   { PV_PARAM(fromIndex, toIndex) });
            UNDO_MOVE(board, &rookVirgin, &kingVirgin, color, pieceTypeFrom, from, to);                    /* Undo the move */
            ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
          }
        }
      }
    }
  }

  /* Single moves of all except center pawns
   * For all except center pawn which not in seventh rank (we already did calculated it) which can do a single move
    * Check where it will go and play the move
    * Search
    * Undo the move
  */
  {
    BITBOARD fromList;
    if (color == WHITE){                                                                                  /* If color is white */
      BITBOARD from;
      fromList = pawnList & ~(occupied >> BOARD_WIDTH);                                                   /* If no obstacles in the path */
      while ((from = next(&fromList))){                                                                   /* For all capable pawns */
        int fromIndex = CONVERT(from);                                                                    /* Convert bit board to index */
        BITBOARD to = from << BOARD_WIDTH;                                                                /* Calculate destination */
        int toIndex = CONVERT(to);                                                                        /* Convert bit board to index */
        PLAY_MOVE(board, &rookVirgin, &kingVirgin, WHITE, PAWN, from, to);                                /* Play the move */
        SEARCH(board, NO_MOVE, NO_MOVE,
               WHITE, NO_MOVE, &alpha, &beta, &bestValue,
               gameValue,
               movementEvaluation(WHITE, PAWN, fromIndex, toIndex),
               depth - 1, ply, pv, pvChild, uciData,
               { PV_PARAM(fromIndex, toIndex) });
        UNDO_MOVE(board, &rookVirgin, &kingVirgin, WHITE, PAWN, from, to);                                /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    } else {                                                                                              /* If color is black */
      BITBOARD from;
      fromList = pawnList & ~(occupied << BOARD_WIDTH);                                                   /* If no obstacles in the path */
      while ((from = next(&fromList))){                                                                   /* For all capable pawns */
        int fromIndex = CONVERT(from);                                                                    /* Convert bit board to index */
        BITBOARD to = from >> BOARD_WIDTH;                                                                /* Calculate destination */
        int toIndex = CONVERT(to);                                                                        /* Convert bit board to index */
        PLAY_MOVE(board, &rookVirgin, &kingVirgin, BLACK, PAWN, from, to);                                /* Play the move */
        SEARCH(board, NO_MOVE, NO_MOVE,
               BLACK, NO_MOVE, &alpha, &beta, &bestValue,
               gameValue,
               movementEvaluation(BLACK, PAWN, fromIndex, toIndex),
               depth - 1, ply, pv, pvChild, uciData,
               { PV_PARAM(fromIndex, toIndex) });
        UNDO_MOVE(board, &rookVirgin, &kingVirgin, BLACK, PAWN, from, to);                                /* Undo the move */
        ALPHA_BETA_CUT_OFF(alpha, beta, bestValue);
      }
    }
  }

  return bestValue;                                                                                       /* Fail soft */
}
