#include "bitboard.h"
#include "attack.h"


/** Attacks of the pawn **/
/* Take account of file and h too */

const BITBOARD pseudo_pawnAttack[TOTAL_COLOR][BOARD_SIZE] =
{
  /* White Pawn */
  {
    NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE,
    PA('a', '7', N), PA('b', '7', N), PA('c', '7', N), PA('d', '7', N), PA('e', '7', N), PA('f', '7', N), PA('g', '7', N), PA('h', '7', N),
    PA('a', '6', N), PA('b', '6', N), PA('c', '6', N), PA('d', '6', N), PA('e', '6', N), PA('f', '6', N), PA('g', '6', N), PA('h', '6', N),
    PA('a', '5', N), PA('b', '5', N), PA('c', '5', N), PA('d', '5', N), PA('e', '5', N), PA('f', '5', N), PA('g', '5', N), PA('h', '5', N),
    PA('a', '4', N), PA('b', '4', N), PA('c', '4', N), PA('d', '4', N), PA('e', '4', N), PA('f', '4', N), PA('g', '4', N), PA('h', '4', N),
    PA('a', '3', N), PA('b', '3', N), PA('c', '3', N), PA('d', '3', N), PA('e', '3', N), PA('f', '3', N), PA('g', '3', N), PA('h', '3', N),
    PA('a', '2', N), PA('b', '2', N), PA('c', '2', N), PA('d', '2', N), PA('e', '2', N), PA('f', '2', N), PA('g', '2', N), PA('h', '2', N),
    NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE,
  },

  /* Black Pawn */
  {
    NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE,
    PA('a', '7', S), PA('b', '7', S), PA('c', '7', S), PA('d', '7', S), PA('e', '7', S), PA('f', '7', S), PA('g', '7', S), PA('h', '7', S),
    PA('a', '6', S), PA('b', '6', S), PA('c', '6', S), PA('d', '6', S), PA('e', '6', S), PA('f', '6', S), PA('g', '6', S), PA('h', '6', S),
    PA('a', '5', S), PA('b', '5', S), PA('c', '5', S), PA('d', '5', S), PA('e', '5', S), PA('f', '5', S), PA('g', '5', S), PA('h', '5', S),
    PA('a', '4', S), PA('b', '4', S), PA('c', '4', S), PA('d', '4', S), PA('e', '4', S), PA('f', '4', S), PA('g', '4', S), PA('h', '4', S),
    PA('a', '3', S), PA('b', '3', S), PA('c', '3', S), PA('d', '3', S), PA('e', '3', S), PA('f', '3', S), PA('g', '3', S), PA('h', '3', S),
    PA('a', '2', S), PA('b', '2', S), PA('c', '2', S), PA('d', '2', S), PA('e', '2', S), PA('f', '2', S), PA('g', '2', S), PA('h', '2', S),
    NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE,
  }
};


/** Attacks of non sliding pieces **/
/* Attacks of all non sliding pieces except for pawn
 * Take account of edge squares too
*/

const BITBOARD pseudo_nonSlidingAttack[TOTAL_PIECE_NON_SLIDING - 1][BOARD_SIZE] =
{
  /* Knight */
  {
    H('a', '8'), H('b', '8'), H('c', '8'), H('d', '8'), H('e', '8'), H('f', '8'), H('g', '8'), H('h', '8'),
    H('a', '7'), H('b', '7'), H('c', '7'), H('d', '7'), H('e', '7'), H('f', '7'), H('g', '7'), H('h', '7'),
    H('a', '6'), H('b', '6'), H('c', '6'), H('d', '6'), H('e', '6'), H('f', '6'), H('g', '6'), H('h', '6'),
    H('a', '5'), H('b', '5'), H('c', '5'), H('d', '5'), H('e', '5'), H('f', '5'), H('g', '5'), H('h', '5'),
    H('a', '4'), H('b', '4'), H('c', '4'), H('d', '4'), H('e', '4'), H('f', '4'), H('g', '4'), H('h', '4'),
    H('a', '3'), H('b', '3'), H('c', '3'), H('d', '3'), H('e', '3'), H('f', '3'), H('g', '3'), H('h', '3'),
    H('a', '2'), H('b', '2'), H('c', '2'), H('d', '2'), H('e', '2'), H('f', '2'), H('g', '2'), H('h', '2'),
    H('a', '1'), H('b', '1'), H('c', '1'), H('d', '1'), H('e', '1'), H('f', '1'), H('g', '1'), H('h', '1'),
  },
  {
    /* King */
    K('a', '8'), K('b', '8'), K('c', '8'), K('d', '8'), K('e', '8'), K('f', '8'), K('g', '8'), K('h', '8'),
    K('a', '7'), K('b', '7'), K('c', '7'), K('d', '7'), K('e', '7'), K('f', '7'), K('g', '7'), K('h', '7'),
    K('a', '6'), K('b', '6'), K('c', '6'), K('d', '6'), K('e', '6'), K('f', '6'), K('g', '6'), K('h', '6'),
    K('a', '5'), K('b', '5'), K('c', '5'), K('d', '5'), K('e', '5'), K('f', '5'), K('g', '5'), K('h', '5'),
    K('a', '4'), K('b', '4'), K('c', '4'), K('d', '4'), K('e', '4'), K('f', '4'), K('g', '4'), K('h', '4'),
    K('a', '3'), K('b', '3'), K('c', '3'), K('d', '3'), K('e', '3'), K('f', '3'), K('g', '3'), K('h', '3'),
    K('a', '2'), K('b', '2'), K('c', '2'), K('d', '2'), K('e', '2'), K('f', '2'), K('g', '2'), K('h', '2'),
    K('a', '1'), K('b', '1'), K('c', '1'), K('d', '1'), K('e', '1'), K('f', '1'), K('g', '1'), K('h', '1')
  }
};


/** Pseudo attack of queen (so includes rook and bishop) **/
/* Ranged attacks, for sliding pieces */

const BITBOARD pseudo_rangedAttack[TOTAL_ATTACK_DIRECTION][BOARD_SIZE] =
{
  /* Upwards */
  {
    NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE,
    A1('a', '7', N, E), A1('b', '7', N, E), A1('c', '7', N, E), A1('d', '7', N, E), A1('e', '7', N, E), A1('f', '7', N, E), A1('g', '7', N, E), NO_MOVE,
    A2('a', '6', N, E), A2('b', '6', N, E), A2('c', '6', N, E), A2('d', '6', N, E), A2('e', '6', N, E), A2('f', '6', N, E), A1('g', '6', N, E), NO_MOVE,
    A3('a', '5', N, E), A3('b', '5', N, E), A3('c', '5', N, E), A3('d', '5', N, E), A3('e', '5', N, E), A2('f', '5', N, E), A1('g', '5', N, E), NO_MOVE,
    A4('a', '4', N, E), A4('b', '4', N, E), A4('c', '4', N, E), A4('d', '4', N, E), A3('e', '4', N, E), A2('f', '4', N, E), A1('g', '4', N, E), NO_MOVE,
    A5('a', '3', N, E), A5('b', '3', N, E), A5('c', '3', N, E), A4('d', '3', N, E), A3('e', '3', N, E), A2('f', '3', N, E), A1('g', '3', N, E), NO_MOVE,
    A6('a', '2', N, E), A6('b', '2', N, E), A5('c', '2', N, E), A4('d', '2', N, E), A3('e', '2', N, E), A2('f', '2', N, E), A1('g', '2', N, E), NO_MOVE,
    A7('a', '1', N, E), A6('b', '1', N, E), A5('c', '1', N, E), A4('d', '1', N, E), A3('e', '1', N, E), A2('f', '1', N, E), A1('g', '1', N, E), NO_MOVE,
  },
  {
    NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE,
    NO_MOVE, A1('b', '7', N, W), A1('c', '7', N, W), A1('d', '7', N, W), A1('e', '7', N, W), A1('f', '7', N, W), A1('g', '7', N, W), A1('h', '7', N, W),
    NO_MOVE, A1('b', '6', N, W), A2('c', '6', N, W), A2('d', '6', N, W), A2('e', '6', N, W), A2('f', '6', N, W), A2('g', '6', N, W), A2('h', '6', N, W),
    NO_MOVE, A1('b', '5', N, W), A2('c', '5', N, W), A3('d', '5', N, W), A3('e', '5', N, W), A3('f', '5', N, W), A3('g', '5', N, W), A3('h', '5', N, W),
    NO_MOVE, A1('b', '4', N, W), A2('c', '4', N, W), A3('d', '4', N, W), A4('e', '4', N, W), A4('f', '4', N, W), A4('g', '4', N, W), A4('h', '4', N, W),
    NO_MOVE, A1('b', '3', N, W), A2('c', '3', N, W), A3('d', '3', N, W), A4('e', '3', N, W), A5('f', '3', N, W), A5('g', '3', N, W), A5('h', '3', N, W),
    NO_MOVE, A1('b', '2', N, W), A2('c', '2', N, W), A3('d', '2', N, W), A4('e', '2', N, W), A5('f', '2', N, W), A6('g', '2', N, W), A6('h', '2', N, W),
    NO_MOVE, A1('b', '1', N, W), A2('c', '1', N, W), A3('d', '1', N, W), A4('e', '1', N, W), A5('f', '1', N, W), A6('g', '1', N, W), A7('h', '1', N, W),
  },
  {
    NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE,
    A1('a', '7', N, X), A1('b', '7', N, X), A1('c', '7', N, X), A1('d', '7', N, X), A1('e', '7', N, X), A1('f', '7', N, X), A1('g', '7', N, X), A1('h', '7', N, X),
    A2('a', '6', N, X), A2('b', '6', N, X), A2('c', '6', N, X), A2('d', '6', N, X), A2('e', '6', N, X), A2('f', '6', N, X), A2('g', '6', N, X), A2('h', '6', N, X),
    A3('a', '5', N, X), A3('b', '5', N, X), A3('c', '5', N, X), A3('d', '5', N, X), A3('e', '5', N, X), A3('f', '5', N, X), A3('g', '5', N, X), A3('h', '5', N, X),
    A4('a', '4', N, X), A4('b', '4', N, X), A4('c', '4', N, X), A4('d', '4', N, X), A4('e', '4', N, X), A4('f', '4', N, X), A4('g', '4', N, X), A4('h', '4', N, X),
    A5('a', '3', N, X), A5('b', '3', N, X), A5('c', '3', N, X), A5('d', '3', N, X), A5('e', '3', N, X), A5('f', '3', N, X), A5('g', '3', N, X), A5('h', '3', N, X),
    A6('a', '2', N, X), A6('b', '2', N, X), A6('c', '2', N, X), A6('d', '2', N, X), A6('e', '2', N, X), A6('f', '2', N, X), A6('g', '2', N, X), A6('h', '2', N, X),
    A7('a', '1', N, X), A7('b', '1', N, X), A7('c', '1', N, X), A7('d', '1', N, X), A7('e', '1', N, X), A7('f', '1', N, X), A7('g', '1', N, X), A7('h', '1', N, X)
  },
  {
    NO_MOVE, A1('b', '8', X, W), A2('c', '8', X, W), A3('d', '8', X, W), A4('e', '8', X, W), A5('f', '8', X, W), A6('g', '8', X, W), A7('h', '8', X, W),
    NO_MOVE, A1('b', '7', X, W), A2('c', '7', X, W), A3('d', '7', X, W), A4('e', '7', X, W), A5('f', '7', X, W), A6('g', '7', X, W), A7('h', '7', X, W),
    NO_MOVE, A1('b', '6', X, W), A2('c', '6', X, W), A3('d', '6', X, W), A4('e', '6', X, W), A5('f', '6', X, W), A6('g', '6', X, W), A7('h', '6', X, W),
    NO_MOVE, A1('b', '5', X, W), A2('c', '5', X, W), A3('d', '5', X, W), A4('e', '5', X, W), A5('f', '5', X, W), A6('g', '5', X, W), A7('h', '5', X, W),
    NO_MOVE, A1('b', '4', X, W), A2('c', '4', X, W), A3('d', '4', X, W), A4('e', '4', X, W), A5('f', '4', X, W), A6('g', '4', X, W), A7('h', '4', X, W),
    NO_MOVE, A1('b', '3', X, W), A2('c', '3', X, W), A3('d', '3', X, W), A4('e', '3', X, W), A5('f', '3', X, W), A6('g', '3', X, W), A7('h', '3', X, W),
    NO_MOVE, A1('b', '2', X, W), A2('c', '2', X, W), A3('d', '2', X, W), A4('e', '2', X, W), A5('f', '2', X, W), A6('g', '2', X, W), A7('h', '2', X, W),
    NO_MOVE, A1('b', '1', X, W), A2('c', '1', X, W), A3('d', '1', X, W), A4('e', '1', X, W), A5('f', '1', X, W), A6('g', '1', X, W), A7('h', '1', X, W)
  },

  /* Downwards */
  {
    NO_MOVE, A1('b', '8', S, W), A2('c', '8', S, W), A3('d', '8', S, W), A4('e', '8', S, W), A5('f', '8', S, W), A6('g', '8', S, W), A7('h', '8', S, W),
    NO_MOVE, A1('b', '7', S, W), A2('c', '7', S, W), A3('d', '7', S, W), A4('e', '7', S, W), A5('f', '7', S, W), A6('g', '7', S, W), A6('h', '7', S, W),
    NO_MOVE, A1('b', '6', S, W), A2('c', '6', S, W), A3('d', '6', S, W), A4('e', '6', S, W), A5('f', '6', S, W), A5('g', '6', S, W), A5('h', '6', S, W),
    NO_MOVE, A1('b', '5', S, W), A2('c', '5', S, W), A3('d', '5', S, W), A4('e', '5', S, W), A4('f', '5', S, W), A4('g', '5', S, W), A4('h', '5', S, W),
    NO_MOVE, A1('b', '4', S, W), A2('c', '4', S, W), A3('d', '4', S, W), A3('e', '4', S, W), A3('f', '4', S, W), A3('g', '4', S, W), A3('h', '4', S, W),
    NO_MOVE, A1('b', '3', S, W), A2('c', '3', S, W), A2('d', '3', S, W), A2('e', '3', S, W), A2('f', '3', S, W), A2('g', '3', S, W), A2('h', '3', S, W),
    NO_MOVE, A1('b', '2', S, W), A1('c', '2', S, W), A1('d', '2', S, W), A1('e', '2', S, W), A1('f', '2', S, W), A1('g', '2', S, W), A1('h', '2', S, W),
    NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE,
  },
  {
    A7('a', '8', S, E), A6('b', '8', S, E), A5('c', '8', S, E), A4('d', '8', S, E), A3('e', '8', S, E), A2('f', '8', S, E), A1('g', '8', S, E), NO_MOVE,
    A6('a', '7', S, E), A6('b', '7', S, E), A5('c', '7', S, E), A4('d', '7', S, E), A3('e', '7', S, E), A2('f', '7', S, E), A1('g', '7', S, E), NO_MOVE,
    A5('a', '6', S, E), A5('b', '6', S, E), A5('c', '6', S, E), A4('d', '6', S, E), A3('e', '6', S, E), A2('f', '6', S, E), A1('g', '6', S, E), NO_MOVE,
    A4('a', '5', S, E), A4('b', '5', S, E), A4('c', '5', S, E), A4('d', '5', S, E), A3('e', '5', S, E), A2('f', '5', S, E), A1('g', '5', S, E), NO_MOVE,
    A3('a', '4', S, E), A3('b', '4', S, E), A3('c', '4', S, E), A3('d', '4', S, E), A3('e', '4', S, E), A2('f', '4', S, E), A1('g', '4', S, E), NO_MOVE,
    A2('a', '3', S, E), A2('b', '3', S, E), A2('c', '3', S, E), A2('d', '3', S, E), A2('e', '3', S, E), A2('f', '3', S, E), A1('g', '3', S, E), NO_MOVE,
    A1('a', '2', S, E), A1('b', '2', S, E), A1('c', '2', S, E), A1('d', '2', S, E), A1('e', '2', S, E), A1('f', '2', S, E), A1('g', '2', S, E), NO_MOVE,
    NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE,
  },
  {
    A7('a', '8', S, X), A7('b', '8', S, X), A7('c', '8', S, X), A7('d', '8', S, X), A7('e', '8', S, X), A7('f', '8', S, X), A7('g', '8', S, X), A7('h', '8', S, X),
    A6('a', '7', S, X), A6('b', '7', S, X), A6('c', '7', S, X), A6('d', '7', S, X), A6('e', '7', S, X), A6('f', '7', S, X), A6('g', '7', S, X), A6('h', '7', S, X),
    A5('a', '6', S, X), A5('b', '6', S, X), A5('c', '6', S, X), A5('d', '6', S, X), A5('e', '6', S, X), A5('f', '6', S, X), A5('g', '6', S, X), A5('h', '6', S, X),
    A4('a', '5', S, X), A4('b', '5', S, X), A4('c', '5', S, X), A4('d', '5', S, X), A4('e', '5', S, X), A4('f', '5', S, X), A4('g', '5', S, X), A4('h', '5', S, X),
    A3('a', '4', S, X), A3('b', '4', S, X), A3('c', '4', S, X), A3('d', '4', S, X), A3('e', '4', S, X), A3('f', '4', S, X), A3('g', '4', S, X), A3('h', '4', S, X),
    A2('a', '3', S, X), A2('b', '3', S, X), A2('c', '3', S, X), A2('d', '3', S, X), A2('e', '3', S, X), A2('f', '3', S, X), A2('g', '3', S, X), A2('h', '3', S, X),
    A1('a', '2', S, X), A1('b', '2', S, X), A1('c', '2', S, X), A1('d', '2', S, X), A1('e', '2', S, X), A1('f', '2', S, X), A1('g', '2', S, X), A1('h', '2', S, X),
    NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE, NO_MOVE
  },
  {
    A7('a', '8', X, E), A6('b', '8', X, E), A5('c', '8', X, E), A4('d', '8', X, E), A3('e', '8', X, E), A2('f', '8', X, E), A1('g', '8', X, E), NO_MOVE,
    A7('a', '7', X, E), A6('b', '7', X, E), A5('c', '7', X, E), A4('d', '7', X, E), A3('e', '7', X, E), A2('f', '7', X, E), A1('g', '7', X, E), NO_MOVE,
    A7('a', '6', X, E), A6('b', '6', X, E), A5('c', '6', X, E), A4('d', '6', X, E), A3('e', '6', X, E), A2('f', '6', X, E), A1('g', '6', X, E), NO_MOVE,
    A7('a', '5', X, E), A6('b', '5', X, E), A5('c', '5', X, E), A4('d', '5', X, E), A3('e', '5', X, E), A2('f', '5', X, E), A1('g', '5', X, E), NO_MOVE,
    A7('a', '4', X, E), A6('b', '4', X, E), A5('c', '4', X, E), A4('d', '4', X, E), A3('e', '4', X, E), A2('f', '4', X, E), A1('g', '4', X, E), NO_MOVE,
    A7('a', '3', X, E), A6('b', '3', X, E), A5('c', '3', X, E), A4('d', '3', X, E), A3('e', '3', X, E), A2('f', '3', X, E), A1('g', '3', X, E), NO_MOVE,
    A7('a', '2', X, E), A6('b', '2', X, E), A5('c', '2', X, E), A4('d', '2', X, E), A3('e', '2', X, E), A2('f', '2', X, E), A1('g', '2', X, E), NO_MOVE,
    A7('a', '1', X, E), A6('b', '1', X, E), A5('c', '1', X, E), A4('d', '1', X, E), A3('e', '1', X, E), A2('f', '1', X, E), A1('g', '1', X, E), NO_MOVE
  }
};


/** Center controlling position for the pieces **/

/* Important position for knight */
const BITBOARD pseudo_knightCenterControl[TOTAL_COLOR] =
{
  AN('c', '3') | AN('f', '3') | AN('d', '4') | AN('e', '4') | AN('d', '5') | AN('e', '5'),  /* White */
  AN('c', '6') | AN('f', '6') | AN('d', '5') | AN('e', '5') | AN('d', '4') | AN('e', '4')   /* Black */
};

/* Important position for the sliding pieces */
const BITBOARD pseudo_slidingCenterControl[TOTAL_COLOR][TOTAL_PIECE_SLIDING] =
{
  /* White */
  {
    AN('c', '4') | AN('f', '4'),  /* Bishop */
    AN('e', '1'),   /* Rook */
    NO_MOVE
  },
  /* Black */
  {
    AN('c', '5') | AN('f', '5') | AN('d', '5') | AN('e', '5') | AN('d', '6') | AN('e', '6'),  /* Bishop */
    AN('e', '8'),   /* Rook */
    NO_MOVE
  }
};


/** Pseudo attack of non sliding pieces **/
/* Pseudo attack of pawn, knight and king; pawn handled separately */

BITBOARD pseudoMove_nonSliding(COLOR color, PIECE_TYPE pieceType, int index){
  if (pieceType == PAWN){                                               /* If the piece is pawn */
    return pseudo_pawnAttack[color][index];                             /* Return pawn attack */
  } else {
    return pseudo_nonSlidingAttack[pieceType - 1][index];               /* Else return from attacks of knight and king */
  }
}


/** Pseudo attack of sliding pieces **/
/* Pseudo attack of bishop, rook and queen */

BITBOARD pseudoMove_sliding(ATTACK_DIRECTION attackDirection, int index){
  return pseudo_rangedAttack[attackDirection][index];                   /* Return sliding attack */
}


/** Center controlling position of knight **/
/* Important position for knight */

BITBOARD pseudoCenter_knight(COLOR color){
  return pseudo_knightCenterControl[color];
}


/** Center controlling positions for the sliding pieces **/
/* Important position for bishop, rook and queen */

BITBOARD pseudoCenter_sliding(COLOR color, PIECE_TYPE pieceType){
  return pseudo_slidingCenterControl[color][pieceType - TOTAL_PIECE + TOTAL_PIECE_SLIDING];
}
