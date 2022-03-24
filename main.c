/**********************************************************************
*********************** Chess Engine - Phalp **************************
**********************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>


#include "bitboard.h"
#include "bitbase.h"
#include "search.h"
#include "evaluation.h"


#define MAXIMUM_INPUT_LENGTH 0xFFF


/** Program details **/

char stringUciName[] = "Phalp 22032400";
char stringUciAuthor[] = "Swastik Majumder";


/** Skeletal representation of the functions in the program **/

bool strscmp(char *, char *);                                           /* Check if the string matches the other from the start */
void initScore(BOARD *, UCI_DATA *);                                   /* Initialize the score */


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

void initScore(BOARD *board, UCI_DATA *uciData){
  SCORE score = 0;

  PIECE_TYPE pieceType;


  /* Calculate evaluation for white */
  for (pieceType = INIT_PIECE; pieceType < LAST_PIECE; ++pieceType){

    BITBOARD pieceList = board -> Board[WHITE][pieceType];
    BITBOARD piece;

    while ((piece = next(&pieceList))){
      score -= pieceValue(pieceType);
      score -= positionalValue(WHITE, pieceType, CONVERT(piece));
    }

  }

  /* Calculate evaluation for black */
  for (pieceType = INIT_PIECE; pieceType < LAST_PIECE; ++pieceType){

    BITBOARD pieceList = board -> Board[BLACK][pieceType];
    BITBOARD piece;

    while ((piece = next(&pieceList))){
      score += pieceValue(pieceType);
      score += positionalValue(BLACK, pieceType, CONVERT(piece));
    }

  }

  uciData -> Score = score;        /* Add the material, position and the special evaluation score */
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

  int pvMove[MAX_DEPTH];

  PV pv;
  PV pv_tmp;

  COLOR color = BLACK;
  BITBOARD enPassant = NO_MOVE;

  int depth;
  int depthIncr;
  int moveTime;

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
      memset(pvMove, 0, sizeof(int) * MAX_DEPTH);
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
      memset(pvMove, 0, sizeof(int) * MAX_DEPTH);
      resetBoard(&board);                                               /* Reset the board */
      color = BLACK;                                                    /* White plays first */
    }
    else if (strscmp(input, "uci")){                                    /* We are following the uci protocol */
      printf("id name ");
      printf(stringUciName);
      printf("\nid author ");
      printf(stringUciAuthor);
      printf("\nuciok\n");
    }
    else if (strscmp(input, "d")){                                      /* This is not mentioned in the uci protocol but used in debugging */
      printBoard(&board);                                               /* Print the board in readable format */
    }
    else if (strscmp(input, "go movetime ")){
      sscanf(input, "go movetime %d", &moveTime);                             /* Check what depth was requested */
      uciData.MoveTime = moveTime;
      uciData.Nodes = 0;                                                 /* Install the depth */
      initScore(&board, &uciData);                     /* Install the score */
      if (color == BLACK){                                              /* If we are playing white */
        uciData.Score = -uciData.Score;                                 /* We are always considering the positive for black, so we negate the score */
      }
      uciData.InitTime = clock();                                       /* See what is the time now */
      int scoreData;
      int doneState = 0;
      int i;
      for (depthIncr=1; ; ++depthIncr){
        uciData.Ply=depthIncr;
        scoreData=doneState;
        doneState = negamax(&board, enPassant, NO_MOVE,
              !color, NO_MOVE,
              -INFINITY, +INFINITY, 0,
              depthIncr, 0,
              &pv_tmp, &uciData, pvMove);
        //printf("Outside\n");
        pvMove[depthIncr - 1] = pv_tmp.Move[0];
        if (doneState != TIME_OVER){
            memcpy(&pv, &pv_tmp, sizeof(PV));
        } else {
            break;
        }
      }
      printf("info depth %d seldepth %d score cp %d nodes %lu nps %lu time %u pv ",
               uciData.Ply, pv.NumberOfMoves, scoreData + uciData.Score,
               uciData.Nodes, (unsigned long int)((double) uciData.Nodes /
               ((double)(clock() - uciData.InitTime) / CLOCKS_PER_SEC)),
               (unsigned)((double)(clock() - uciData.InitTime) / CLOCKS_PER_SEC));
        for (i=0; i < pv.NumberOfMoves; ++i){
          PRINT_PV_MOVE( . , i);
        }
      putchar('\n');
      printf("bestmove ");
      PRINT_PV_MOVE( . , 0);
      printf("ponder ");
      PRINT_PV_MOVE( . , 1);
      putchar('\n');
    }
    else if (strscmp(input, "go wtime")){
        int wtime;
        int btime;
        int winc;
        int binc;
        int movestogo;
        if (sscanf(input, "go wtime %d btime %d winc %d binc %d movestogo %d", &wtime, &btime, &winc, &binc, &movestogo) != 5){
            movestogo = 20;
        }
        if (color == WHITE){
            moveTime = btime/movestogo;
        } else {
            moveTime = wtime/movestogo;
        }
        uciData.MoveTime = moveTime;
      uciData.Nodes = 0;                                                 /* Install the depth */
      initScore(&board, &uciData);                     /* Install the score */
      if (color == BLACK){                                              /* If we are playing white */
        uciData.Score = -uciData.Score;                                 /* We are always considering the positive for black, so we negate the score */
      }
      uciData.InitTime = clock();                                       /* See what is the time now */
      int scoreData;
      int doneState = 0;
      int i;
      for (depthIncr=1; ; ++depthIncr){
        uciData.Ply=depthIncr;
        scoreData=doneState;
        doneState = negamax(&board, enPassant, NO_MOVE,
              !color, NO_MOVE,
              -INFINITY, +INFINITY, 0,
              depthIncr, 0,
              &pv_tmp, &uciData, pvMove);
        pvMove[depthIncr - 1] = pv_tmp.Move[0];
        if (doneState != TIME_OVER){
            memcpy(&pv, &pv_tmp, sizeof(PV));
        } else {
            break;
        }
      }
      printf("info depth %d seldepth %d score cp %d nodes %lu nps %lu time %u pv ",
               uciData.Ply, pv.NumberOfMoves, scoreData + uciData.Score,
               uciData.Nodes, (unsigned long int)((double) uciData.Nodes /
               ((double)(clock() - uciData.InitTime) / CLOCKS_PER_SEC)),
               (unsigned)((double)(clock() - uciData.InitTime) / CLOCKS_PER_SEC));
        for (i=0; i < pv.NumberOfMoves; ++i){
          PRINT_PV_MOVE( . , i);
        }
      putchar('\n');
      printf("bestmove ");
      PRINT_PV_MOVE( . , 0);
      printf("ponder ");
      PRINT_PV_MOVE( . , 1);
      putchar('\n');
    }
    else if (strscmp(input, "go depth ")){                              /* Search within limited depth */
      sscanf(input, "go depth %d", &depth);                                   /* Check what depth was requested */
      uciData.Nodes = 0;                                                /* We start from zero nodes */
      uciData.MoveTime = -1;                                            /* Install the depth */
      initScore(&board, &uciData);                     /* Install the score */
      if (color == BLACK){                                              /* If we are playing white */
        uciData.Score = -uciData.Score;                                 /* We are always considering the positive for black, so we negate the score */
      }
      uciData.InitTime = clock();                                         /* See what is the time now */
      int scoreData;
      int doneState = 0;
      int i;
      for (depthIncr=1; depthIncr <= depth; ++depthIncr){
        uciData.Ply=depthIncr;
        scoreData=doneState;
        doneState = negamax(&board, enPassant, NO_MOVE,
              !color, NO_MOVE,
              -INFINITY, +INFINITY, 0,
              depthIncr, 0,
              &pv_tmp, &uciData, pvMove);
        //printf("Outside\n");
        pvMove[depthIncr - 1] = pv_tmp.Move[0];
        memcpy(&pv, &pv_tmp, sizeof(PV));
      }
      printf("info depth %d seldepth %d score cp %d nodes %lu nps %lu time %u pv ",
               uciData.Ply, pv.NumberOfMoves, scoreData + uciData.Score,
               uciData.Nodes, (unsigned long int)((double) uciData.Nodes /
               ((double)(clock() - uciData.InitTime) / CLOCKS_PER_SEC)),
               (unsigned)((double)(clock() - uciData.InitTime) / CLOCKS_PER_SEC));
        for (i=0; i < pv.NumberOfMoves; ++i){
          PRINT_PV_MOVE( . , i);
        }
      putchar('\n');
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
