#ifndef BET_TYPES__H_
#define BET_TYPES__H_

/*
 * BET TYPES global visibility
 */

/* Right/wrong bets (most common) */
enum {
    BET_pass,
    BET_dont_pass,
    BET_come,
    BET_dont_come,

    /* --> Place and Lay (Buy not yet implemented) */
    BET_place_4,
    BET_place_5,
    BET_place_6,
    BET_place_8,
    BET_place_9,
    BET_place_10,
    BET_lay_4,
    BET_lay_5,
    BET_lay_6,
    BET_lay_8,
    BET_lay_9,
    BET_lay_10,

    /* --> Hardways */
    BET_hard_10,
    BET_hard_8,
    BET_hard_6,
    BET_hard_4,

    /* Single Bets */
    BET_field,
    BET_yoleven,
    BET_midnight,

    /* DO NOT ADD ANY MORE AFTER BET_TYPE_MAX */
    BET_TYPE_MAX
};

#endif
