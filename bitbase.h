#ifndef BITBASE_H_INCLUDED
#define BITBASE_H_INCLUDED


#include "bitboard.h"


/** Skeletal representation of all functions in the program **/

int countBit(BITBOARD);                                                 /* Count the number of set bits in a bit board */
BITBOARD next(BITBOARD *);                                              /* Iterate bit board from backwards */
PIECE_TYPE identifyPiece(BOARD *, COLOR, BITBOARD);                     /* Identify what piece in the specified location */
void resetBoard(BOARD *);                                               /* Reset the board to starting position */
void printBoard(BOARD *);                                               /* Print the board just for representation */
BITBOARD playMove_algebraicNotation(BOARD *, BITBOARD, COLOR, char *);  /* Play move from algebraic character input */
void binaryPrint(BITBOARD);                                             /* Print bit board in binary */


/** Playing the moves **/

/* Play the usual displacing moves
 * If rook is moved it is no more a virgin
 * If the piece was king it is no more a virgin
 * Move the piece in board
 * Move the piece in occupied
*/
#define PLAY_MOVE(board, rookVirgin, kingVirgin, color, pieceType, from, to){ \
  if (pieceType == ROOK){ \
    *(rookVirgin) = (board) -> RookVirgin[color]; \
    (board) -> RookVirgin[color] &= ~from; \
  } \
  else if (pieceType == KING){ \
    *(kingVirgin) = (board) -> KingVirgin[color]; \
    (board) -> KingVirgin[color] = false; \
  } \
  (board) -> Board[color][pieceType] ^= (from) | (to); \
  (board) -> Occupied[color] ^= (from) | (to); \
}

/* Play a capture move
 * Play the move
 * Remove the captured piece from the board
 * Remove the captured piece from the occupied
*/
#define PLAY_MOVE_CAPTURE(board, rookVirgin, kingVirgin, color, pieceTypeFrom, pieceTypeTo, from, to){ \
  PLAY_MOVE(board, rookVirgin, kingVirgin, color, pieceTypeFrom, from, to); \
  (board) -> Board[!(color)][pieceTypeTo] &= ~(to); \
  (board) -> Occupied[!(color)] &= ~(to); \
}

/* Play en passant capture
 * Variable enPassant is the capture location
 * Shift the our moving pawn in board
 * Remove captured pawn
 * Shift in the occupied board
 * Remove the pawn in the occupied board
*/
#define PLAY_MOVE_EN_PASSANT(board, enPassant, color, from, to){ \
  (board) -> Board[color][PAWN] ^= (from) | (to); \
  (board) -> Board[!(color)][PAWN] &= ~(enPassant); \
  (board) -> Occupied[color] ^= (from) | (to); \
  (board) -> Occupied[!(color)] &= ~(enPassant); \
}

/* Play promotion without capture
 * Remove the pawn
 * Add the promoting piece
 * Change occupied
*/
#define PLAY_MOVE_PROMOTION_MOVEMENT(board, promotionPiece, color, from, to){ \
  (board) -> Board[color][PAWN] &= ~(from); \
  (board) -> Board[color][promotionPiece] |= to; \
  (board) -> Occupied[color] ^= (from) | (to); \
}

/* Play promotion and capture a piece
 * Play promotion
 * Remove the captured piece
 * Remove from opponent's occupied
*/
#define PLAY_MOVE_PROMOTION_CAPTURE(board, promotionPiece, color, pieceTypeTo, from, to){ \
  PLAY_MOVE_PROMOTION_MOVEMENT(board, promotionPiece, color, from, to); \
  (board) -> Board[!(color)][pieceTypeTo] &= ~(to); \
  (board) -> Occupied[!(color)] &= ~(to); \
}


/** Undoing the moves **/

/* Undo the usual moves
 * If rook or king put the initial virgin state
 * Undo the movement in the board
 * Undo the movement in the occupied
*/
#define UNDO_MOVE(board, rookVirgin, kingVirgin, color, pieceType, from, to){ \
  if (pieceType == ROOK){ \
    (board) -> RookVirgin[color] = *(rookVirgin); \
  } \
  else if (pieceType == KING){ \
    (board) -> KingVirgin[color] = *(kingVirgin); \
  } \
  (board) -> Board[color][pieceType] ^= (from) | (to); \
  (board) -> Occupied[color] ^= (from) | (to); \
}

/* Undo the capture moves
 * If the rook or king put the initial virgin state
 * Undo the movement
 * Put the captures back in the board
 * Put the captures back in occupied
*/
#define UNDO_MOVE_CAPTURE(board, rookVirgin, kingVirgin, color, pieceTypeFrom, pieceTypeTo, from, to){ \
  UNDO_MOVE(board, rookVirgin, kingVirgin, color, pieceTypeFrom, from, to); \
  (board) -> Board[!(color)][pieceTypeTo] |= to; \
  (board) -> Occupied[!(color)] |= to; \
}

/* Undo a en passant capture
 * Undo the pawn movement
 * Put the en passant capture back in the board
 * Put the en passant capture back in occupied
*/
#define UNDO_MOVE_EN_PASSANT(board, enPassant, color, from, to){ \
  (board) -> Board[color][PAWN] ^= (from) | (to); \
  (board) -> Occupied[color] ^= (from) | (to); \
  (board) -> Board[!(color)][PAWN] |= enPassant; \
  (board) -> Occupied[!(color)] |= enPassant; \
}

/* Undo promotion movement
 * Add the pawn
 * Remove the promotion piece
 * Change occupied
*/
#define UNDO_MOVE_PROMOTION_MOVEMENT(board, promotionPiece, color, from, to){ \
  (board) -> Board[color][PAWN] |= from; \
  (board) -> Board[color][promotionPiece] &= ~(to); \
  (board) -> Occupied[color] ^= (from) | (to); \
}

/* Undo promotion and capture
 * Undo promotion movement
 * Add captured piece in the board
 * Add captured piece in occupied
*/
#define UNDO_MOVE_PROMOTION_CAPTURE(board, promotionPiece, color, pieceTypeTo, from, to){ \
  UNDO_MOVE_PROMOTION_MOVEMENT(board, promotionPiece, color, from, to); \
  (board) -> Board[!(color)][pieceTypeTo] |= to; \
  (board) -> Occupied[!(color)] |= to; \
}


#endif /* BITBASE_H_INCLUDED */
