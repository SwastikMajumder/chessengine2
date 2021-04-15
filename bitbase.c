#include <stdio.h>


#include "bitboard.h"
#include "bitbase.h"


/** Piece code for printing the board **/

const char *pieceCode[TOTAL_COLOR] =
{
  "PNKBRQ",                                                             /* Capital letters for white */
  "pnkbrq"                                                              /* Small letters for black */
};


/** Initial setup of the board in bit board representation **/

const BITBOARD initialSetup_board[TOTAL_COLOR][TOTAL_PIECE] =
{
  {
    RANK(WHITE, 2),                                                     /* The whole rank two are pawns */
    AN('b', '1') | AN('g', '1'),
    AN('e', '1'),
    AN('c', '1') | AN('f', '1'),
    AN('a', '1') | AN('h', '1'),
    AN('d', '1')
  },
  {
    RANK(BLACK, 2),                                                     /* The whole rank two are pawns */
    AN('b', '8') | AN('g', '8'),
    AN('e', '8'),
    AN('c', '8') | AN('f', '8'),
    AN('a', '8') | AN('h', '8'),
    AN('d', '8')
  }
};


/** Occupied squares **/

const BITBOARD initialSetup_occupied[TOTAL_COLOR] =
{
  RANK(WHITE, 1) | RANK(WHITE, 2),
  RANK(BLACK, 1) | RANK(BLACK, 2)
};


/** Iterate bit board from backwards direction **/

BITBOARD next(BITBOARD *list){
  BITBOARD listCopy;

#ifndef ALLOW_UNSIGNED_SUBTRACTION_UNDERFLOW
  if (*list == 0) return 0;                                             /* If list is empty return over */
#endif /* ALLOW_UNSIGNED_SUBTRACTION_UNDERFLOW */

  listCopy = *list;                                                     /* Store backup of the bit board */
  *list &= *list - 1;                                                   /* Remove the last bit */

  return listCopy ^ *list;                                              /* See what changed and iterate */
}


/** Count number of bits in a bit board **/

int countBit(BITBOARD bitboard){

  int numberOfBit = 0;

#ifdef ALLOW_UNSIGNED_SUBTRACTION_UNDERFLOW

  if (bitboard) ++numberOfBit;
  while (bitboard &= bitboard - 1){
    ++numberOfBit;
  }

#else

  for (;;){
    if (bitboard){
      ++numberOfBit;
      bitboard &= bitboard - 1;
    } else {
      break;
    }
  }

#endif /* ALLOW_UNSIGNED_SUBTRACTION_UNDERFLOW */

  return numberOfBit;

}


/** Identify what piece is in the specified location **/

PIECE_TYPE identifyPiece(BOARD *board, COLOR color, BITBOARD address){
  PIECE_TYPE pieceType;
  for (pieceType = LAST_PIECE - 1; pieceType >= INIT_PIECE; --pieceType)
    if ((board -> Board[color][pieceType]) & address)                   /* Search all the piece types */
      return pieceType;                                                 /* Return piece type if found */
  return INVALID_PIECE_TYPE;                                            /* Not found */
}


/** Reset the board to starting position **/

void resetBoard(BOARD *board){
  COLOR color;
  PIECE_TYPE pieceType;

  for (color=INIT_COLOR; color<LAST_COLOR; ++color)                     /* Do the initial setup */
    for (pieceType=INIT_PIECE; pieceType<LAST_PIECE; ++pieceType)
      board -> Board[color][pieceType] = initialSetup_board[color][pieceType];

  for (color=INIT_COLOR; color<LAST_COLOR; ++color)                     /* Fill occupied setup */
    board -> Occupied[color] = initialSetup_occupied[color];

  for (color=INIT_COLOR; color<LAST_COLOR; ++color)                     /* Initially the kings are virgin */
    board -> KingVirgin[color] = true;

  for (color=INIT_COLOR; color<LAST_COLOR; ++color)                     /* Even rooks are too */
    board -> RookVirgin[color] = board -> Board[color][ROOK];
}


/** Print the board just for representation **/

void printBoard(BOARD *board){
  char characterBoard[BOARD_SIZE] = {' '};                              /* Fill the empty spaces with space */
  COLOR color;
  PIECE_TYPE pieceType;
  int i, j;

  for (color=INIT_COLOR; color < LAST_COLOR; ++color){                  /* For all colors */
    for (pieceType=INIT_PIECE; pieceType < LAST_PIECE; ++pieceType){    /* For all pieces */
      BITBOARD pieceList = board -> Board[color][pieceType];            /* Store in the piece list */
      BITBOARD piece;
      while ((piece = next(&pieceList))){                               /* For all piece in the piece list */
        characterBoard[CONVERT(piece)] = *(pieceCode[color] + pieceType);
      }
    }
  }

  /* Print the board */
  printf("    +---+---+---+---+---+---+---+---+\n");
  printf("    | a   b   c   d   e   f   g   h |\n");
  for (i=0; i<BOARD_WIDTH; ++i){
    printf("+---+---+---+---+---+---+---+---+---+---+\n| %d ", 8 - i);
    for (j=0; j<BOARD_WIDTH; ++j){
      printf("| %c ", characterBoard[i * BOARD_WIDTH + j]);
    }
    printf("| %d |\n", 8 - i);
  }
  printf("+---+---+---+---+---+---+---+---+---+---+\n");
  printf("    | a   b   c   d   e   f   g   h |\n");
  printf("    +---+---+---+---+---+---+---+---+\n\n");
}


/** Play move from algebraic character input **/

BITBOARD playMove_algebraicNotation(BOARD *board, BITBOARD enPassant, COLOR color, char *algebraicInput){
  BITBOARD from = AN(algebraicInput[0], algebraicInput[1]);             /* From */
  BITBOARD to = AN(algebraicInput[2], algebraicInput[3]);               /* To */

  if (algebraicInput[4] == 'n' || algebraicInput[4] == 'b' ||
      algebraicInput[4] == 'r' || algebraicInput[4] == 'q'){

        PIECE_TYPE pieceTypeTo = identifyPiece(board, !color, to);      /* Identify piece to if capture */
        PIECE_TYPE promotionPiece;

        /* Identify the promoting piece */
        switch (algebraicInput[4]){
          case 'n':
            promotionPiece = KNIGHT;
            break;
          case 'b':
            promotionPiece = BISHOP;
            break;
          case 'r':
            promotionPiece = ROOK;
            break;
          case 'q':
            promotionPiece = QUEEN;
            break;
          default:
            promotionPiece = QUEEN;
            break;
        }

        if (pieceTypeTo != INVALID_PIECE_TYPE){                         /* If capture */
          PLAY_MOVE_PROMOTION_CAPTURE(board, promotionPiece, color, pieceTypeTo, from, to);
        } else {
          PLAY_MOVE_PROMOTION_MOVEMENT(board, promotionPiece, color, from, to);
        }
        printf(algebraicInput);
        return (BITBOARD) FLAG_PROMOTION;

  }
  else if (color == WHITE ? enPassant & to >> BOARD_WIDTH               /* If en passant capture */
                          : enPassant & to << BOARD_WIDTH){
    PLAY_MOVE_EN_PASSANT(board, enPassant, color, from, to);            /* Play en passant move */
  } else {
    PIECE_TYPE pieceTypeFrom = identifyPiece(board, color, from);       /* Identify piece from */
    PIECE_TYPE pieceTypeTo = identifyPiece(board, !color, to);          /* Identify piece to if capture */

    board -> Board[color][pieceTypeFrom] ^= from | to;                  /* Move the piece */
    board -> Occupied[color] ^= from | to;                              /* Move the piece in occupied square */

    if (pieceTypeTo != INVALID_PIECE_TYPE){                             /* If it is a capture */
      board -> Board[!color][pieceTypeTo] &= ~to;                       /* Remove the capture */
      board -> Occupied[!color] &= ~to;                                 /* Remove the capture from the occupied board */
    }

    if (pieceTypeFrom == PAWN){                                         /* If piece is pawn */
      if (color == WHITE &&                                             /* Check if any two step move is being done */
          from << (BOARD_WIDTH * 2) & to){
            return to;                                                  /* Return en passant location */
      }
      if (color == BLACK &&
          from >> (BOARD_WIDTH * 2) & to){
            return to;

      }
    }
    else if (pieceTypeFrom == ROOK){                                    /* If rook was moved that was no more a virgin */
      board -> RookVirgin[color] &= ~from;
    }
    else if (pieceTypeFrom == KING){                                    /* If the king was moved it is no more a virgin */
      board -> KingVirgin[color] = false;
      if (from >> 2 & to){                                              /* King side castling */
        board -> Board[color][ROOK] ^= to >> 1 | from >> 1;             /* Move the rook in board */
        board -> Occupied[color] ^= to >> 1 | from >> 1;                /* Move in occupied too */
      }
      else if (from << 2 & to){                                         /* Else if queen side castling */
        board -> Board[color][ROOK] ^= to << 2 | from << 1;             /* Move the rook in board */
        board -> Occupied[color] ^= to << 2 | from << 1;                /* Move the rook in occupied */
      }
    }
  }

  return NO_MOVE;
}


/** Print bit board in binary **/
/* Print a bit board starting from the highest bit */

void binaryPrint(BITBOARD bitboard){
  int i;

  for(i = 63; i >= 0; --i){                                             /* For all the bits */
    putchar(bitboard & ((BITBOARD)0x1 << 63) ? '1' : '0');              /* Check the leftmost bit */
    bitboard <<=1;                                                      /* Shift the bit board leftwards */
    if (!(i % BOARD_WIDTH)) putchar('\n');                              /* Put a new line if end of board width */
  }

  putchar('\n');
}


