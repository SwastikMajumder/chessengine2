/**********************************************************************
*********************** Chess Engine - Chandra ************************
***********************************************************************
 * Wrote in ANSI C
 * Negamax with alpha beta pruning
 * Follows UCI protocol
 * To do
  -> Better move ordering
  -> Three fold repetition
  -> Mate count
  -> Stalemate
  -> Iterative deepening
  -> Calculating with given move time
  -> Better evaluation like passed and isolated pawns
  -> King safety
  -> Killer Moves
  -> Null move
  -> Futility pruning
  -> Late Move Reductions
  -> Faster move generation
  -> Transposition Tables
  -> Time man for tournament time limits
  -> Incremental time limits
  -> Window searching
  -> Fifty move rule
  -> Magic bit boards
  -> Reduce repetitions in code
***********************************************************************
***********************************************************************
**********************************************************************/


#include <stdio.h>
#include <time.h>
#include <signal.h>


#include "bitboard.h"
#include "bitbase.h"
#include "search.h"
#include "evaluation.h"


#define MAXIMUM_INPUT_LENGTH 0xFFF


/** Program details **/

char stringUciName[] = "Chandra 000082718";
char stringUciAuthor[] = "Swastik Mozumder";


/** Skeletal representation of the functions in the program **/

bool strscmp(char *, char *);                                           /* Check if the string matches the other from the start */
SCORE initScore(BOARD *, UCI_DATA *);                                   /* Initialize the score */


/** Check if the string matches the other from the start **/
/* Check if the string matches the other from the start
 * If the comparer string ends return true
*/

bool strscmp(char *input, char *comparer){
  int i=0;
  do {
    if (input[i] != comparer[i])
      return false;
  } while (comparer[++i] != '\0');
  return true;
}


/** Initialize the score **/
/* Check the phase of the game
 * Add positional and material values of the pieces in the board
*/

int initScore(BOARD *board, UCI_DATA *uciData){
  SCORE score = 0;

  PIECE_TYPE pieceType;
  COLOR color;

  GAME_PHASE gamePhase;

  /* See what phase of game is going on */
  GAME_PHASE_VALUE gamePhaseValue = 0;
  for (color = INIT_COLOR; color < LAST_COLOR; ++color){
    for (pieceType = INIT_PIECE; pieceType < LAST_PIECE; ++pieceType){
      gamePhaseValue += countBit(board -> Board[color][pieceType]) * pieceGamePhaseValue(pieceType);
    }
  }

  gamePhase = currentGamePhase(gamePhaseValue);

  /* Calculate evaluation for white */
  for (pieceType = INIT_PIECE; pieceType < LAST_PIECE; ++pieceType){

    BITBOARD pieceList = board -> Board[WHITE][pieceType];
    BITBOARD piece;

    while ((piece = next(&pieceList))){
      score -= pieceValue(gamePhase, pieceType);
      score -= positionalValue(WHITE, gamePhase, pieceType, CONVERT(piece));
    }

  }

  /* Calculate evaluation for black */
  for (pieceType = INIT_PIECE; pieceType < LAST_PIECE; ++pieceType){

    BITBOARD pieceList = board -> Board[BLACK][pieceType];
    BITBOARD piece;

    while ((piece = next(&pieceList))){
      score += pieceValue(gamePhase, pieceType);
      score += positionalValue(BLACK, gamePhase, pieceType, CONVERT(piece));
    }

  }

  uciData -> Score = score + evaluation(board, gamePhaseValue);         /* Add the material, position and the special evaluation score */

  return gamePhaseValue;
}


/** Main function **/
/* Signal the caller
 * Take input in loop
 * Analyze the command
 * Do change according to the command
*/

int main (){
  char input[MAXIMUM_INPUT_LENGTH] = {'\0'};
  char *moveList;

  BOARD board;
  UCI_DATA uciData;

  GAME_PHASE_VALUE gamePhaseValue = 0;

  PV pv;

  COLOR color = BLACK;
  BITBOARD enPassant = NO_MOVE;

  int depth;

  resetBoard(&board);

  signal(SIGINT, SIG_IGN);                                              /* Signal the caller that I am an engine */

  /* Tell everyone about the author :D */
  printf(stringUciName);
  printf(" by ");
  printf(stringUciAuthor);
  putchar('\n');
  fflush(stdout);

  /* While can get messages */
  while (fgets(input, MAXIMUM_INPUT_LENGTH, stdin)){
    if (strscmp(input, "isready")){                                     /* We are always ready */
      printf("readyok\n");
    }
    else if (strscmp(input, "position startpos moves ")){               /* We are getting the position of the board */
      resetBoard(&board);                                               /* Reset the board */
      moveList = input + 24;                                            /* Ignore the command part */
      enPassant = NO_MOVE;                                              /* Initial board setup there is no en passant */
      color = BLACK;                                                    /* White always plays the first */
      for (;;){                                                         /* Check the whole list */
        enPassant = playMove_algebraicNotation(&board, enPassant, color = !color, moveList);
        if (enPassant & (BITBOARD) FLAG_PROMOTION){                     /* There was a promotion */
          ++moveList;                                                   /* If there was a promotion means move list will have a character */
          enPassant = NO_MOVE;                                          /* It cannot be en passant */
        }
        if (moveList[4] != ' ')                                         /* List finished */
          break;                                                        /* Stop seeing next */
        moveList += 5;                                                  /* Iterate the list */
      }
    }
    else if (strscmp(input, "position startpos") || strscmp(input, "ucinewgame")){
      resetBoard(&board);                                               /* Reset the board */
      color = BLACK;                                                    /* White plays first */
    }
    else if (strscmp(input, "uci")){                                    /* We are following the uci protocol */
      printf("id name ");
      printf(stringUciName);
      printf("\nid author ");
      printf(stringUciAuthor);
      printf("\n\nuciok\n");
    }
    else if (strscmp(input, "d")){                                      /* This is not mentioned in the uci protocol but used in debugging */
      printBoard(&board);                                               /* Print the board in readable format */
    }
    else if (strscmp(input, "go depth ")){                              /* Search within limited depth */
      sscanf(input, "go depth %d", &depth);                             /* Check what depth was requested */
      uciData.Nodes = 0;                                                /* We start from zero nodes */
      uciData.Ply = depth;                                              /* Install the depth */
      gamePhaseValue = initScore(&board, &uciData);                     /* Install the score */
      if (color == BLACK){                                              /* If we are playing white */
        uciData.Score = -uciData.Score;                                 /* We are always considering the positive for black, so we negate the score */
      }
      uciData.InitTime = clock();                                       /* See what is the time now */
      negamax(&board, enPassant, NO_MOVE,                               /* Pass board and en passant, we assume that castling legal one */
              !color, NO_MOVE,                                          /* Flip the color but no quiescence search */
              -INFINITY, +INFINITY, 0, 0, gamePhaseValue,               /* Start the worst possible assured score and start evaluation as 0 */
              uciData.Ply, 0,                                           /* We start from no ply and depth as requested */
              &pv, &uciData);                                           /* Structure for principle variation and uci data */
      printf("bestmove ");
      PRINT_PV_MOVE( . , 0);
      printf("ponder ");
      PRINT_PV_MOVE( . , 1);
      putchar('\n');
    }
    else if (strscmp(input, "quit")){                                   /* Quit if requested */
      return 0;
    }
    fflush(stdout);
  }

  return 0;
}
