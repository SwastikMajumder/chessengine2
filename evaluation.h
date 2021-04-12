#ifndef EVALUATION_H_INCLUDED
#define EVALUATION_H_INCLUDED


/** Skeletal representation of all functions in the program **/

SCORE pieceValue(GAME_PHASE, PIECE_TYPE);
SCORE positionalValue(COLOR, GAME_PHASE, PIECE_TYPE, int);                              /* Return a positional value */
SCORE movementEvaluation(COLOR, GAME_PHASE, PIECE_TYPE, int, int);                      /* Movement evaluation */
SCORE captureEvaluation(COLOR, GAME_PHASE, PIECE_TYPE, PIECE_TYPE, int, int);           /* Capture evaluation */
SCORE enPassantEvaluation(int, COLOR, GAME_PHASE, int, int);                            /* En passant capture evaluation */
SCORE promotionMovementEvaluation(PIECE_TYPE, COLOR, GAME_PHASE, int, int);             /* Simple promotion evaluation */
SCORE promotionCaptureEvaluation(PIECE_TYPE, COLOR, GAME_PHASE, PIECE_TYPE, int, int);  /* Capture promotion */
SCORE castling_OO_Evaluation(COLOR, GAME_PHASE);
SCORE castling_OOO_Evaluation(COLOR, GAME_PHASE);

GAME_PHASE currentGamePhase(int);                                                       /* Check the current phase of the game */
int pieceGamePhaseValue(PIECE_TYPE);                                                    /* Return how much a piece can affect the phase of the game */
int captureGamePhaseEvaluation(PIECE_TYPE);                                             /* Return how much a capture affect the phase of the game */
int enPassantGamePhaseEvaluation();                                                     /* Return how much a en passant can affect the phase */
int promotionMovementGamePhaseEvaluation(PIECE_TYPE);                                   /* How much a simple promotion can affect the phase */
int promotionCaptureGamePhaseEvaluation(PIECE_TYPE, PIECE_TYPE);                        /* How much a capture promotion can affect the phase of game */

SCORE evaluation(BOARD *, GAME_PHASE_VALUE);                                            /* Return extra evaluation terms */


#endif /* EVALUATION_H_INCLUDED */
