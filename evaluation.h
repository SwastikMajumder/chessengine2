#ifndef EVALUATION_H_INCLUDED
#define EVALUATION_H_INCLUDED


/** Skeletal representation of all functions in the program **/

SCORE pieceValue(PIECE_TYPE);
SCORE positionalValue(COLOR, PIECE_TYPE, int);                              /* Return a positional value */
SCORE movementEvaluation(COLOR, PIECE_TYPE, int, int);                      /* Movement evaluation */
SCORE captureEvaluation(COLOR, PIECE_TYPE, PIECE_TYPE, int, int);           /* Capture evaluation */
SCORE enPassantEvaluation(int, COLOR, int, int);                            /* En passant capture evaluation */
SCORE promotionMovementEvaluation(PIECE_TYPE, COLOR, int, int);             /* Simple promotion evaluation */
SCORE promotionCaptureEvaluation(PIECE_TYPE, COLOR, PIECE_TYPE, int, int);  /* Capture promotion */
SCORE castling_OO_Evaluation(COLOR);
SCORE castling_OOO_Evaluation(COLOR);


#endif /* EVALUATION_H_INCLUDED */
