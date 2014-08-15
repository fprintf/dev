#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

/* TODO
 * Everything pretty much, see main()
 * for current functions that are available
 * (nothing is NOT working at least :))
 */



/* I prefer a naming convention (when using typedef) of _T as it
 * prevents invading the standards namespace and is still fairly
 * recognizable as a "type" and personally preferable to t_type which 
 * is another commonly used option */
typedef long double money_T;

/*
 * TODO
 * Do the below using m4 macro so we don't have to 
 * duplicate the order and names of the types 
 * in the enum and then again in the string map
 */

enum bet_type {
    /*  SENTIAL */
    BET_t_invalid=0,

    /* Multi-roll resolved bets (bets rolled until 7 is hit or crap out) */
    /* --> Line bets */
    BET_t_pass, 
    BET_t_dont_pass,
    BET_t_come,
    BET_t_dont_come,

    /* --> Place and Lay (Buy not yet implemented) */
    BET_t_place_4,
    BET_t_place_5,
    BET_t_place_6,
    BET_t_place_8,
    BET_t_place_9,
    BET_t_place_10,
    BET_t_lay_4,
    BET_t_lay_5,
    BET_t_lay_6,
    BET_t_lay_8,
    BET_t_lay_9,
    BET_t_lay_10,

    /* --> Hardways */
    BET_t_hard_10,
    BET_t_hard_8,
    BET_t_hard_6,
    BET_t_hard_4,


    /* Single Bets */
    BET_t_field,
    BET_t_yoleven, /* single yo bet (11) */
    BET_t_midnight, /* single midnight bet (12) */
    BET_t_snake_eyes /* single crap bet (2)  */
};

#define PLAYER_MAX_BETS 100 /* there is less then 100 different types of bets in craps */

/* Below working refers to
 * whether or not the bet is active
 * when the come out roll is throwed
 * or 
 *
 * Players can place or buy any point number (4, 5, 6, 8, 9, 10) by placing their wager in the come area and telling the dealer how much and on what number(s), "30 on the 6", "5 on the 5" or "25 buy the 10". Both place and buy bets are bets that the number bet on will be rolled before a 7 is rolled. These bets are considered working bets, and will continue to be paid out each time a shooter rolls the place or buy point number. By rules, place bets are NOT working on the come out roll but can be "turned on" by the player.
 * */
#define BETFL_ODDS 0x1  /* if this is set then "odds_value" will have the odds bet on top of this bet */
#define BETFL_OFF  0x2  /* bet is off (not working) */
#define BET_TYPE_HAS_ODDS(x) ((x) > 0 && (x) < BET_t_hard_10)

static const struct bet {
    enum bet_type type; /* what the bet is placed on come, pass, dont pass, 6, etc.. */
    money_T value; /* value of the bet */
    money_T odds_value; /* if this bet has an odds bet on it, the value of that is stored here */
    short int flags;     /* see above */
} bet_initializer = {
    .type = BET_t_invalid,
    .value = 0.0,
    .odds_value = 0.0,
    .flags = 0
};

#define PLAYERFL_TOWEL 0x1 /* player has requested all his active bets be turned off */

static const struct player {
    short int id; /* we are assuming a low MAX_PLAYERS value here (short int) */
    struct bet bets[PLAYER_MAX_BETS]; 
    char * name;
    money_T cash;
    short int flags;
} player_initializer = {
    .id = 0, 
    .bets = {{0}},
    .name = NULL, 
    .cash = 0.0,
    .flags = 0
};


void bet_display(const struct bet * bet)
{
    const char * type;
    
    type= bet_type_to_string[(int)bet->type];
    fprintf(stdout, "%-20s $%.2Lf", type, bet->value);
    if (bet->flags & BETFL_ODDS) 
        fprintf(stdout, " [ODDS $%.2Lf]", bet->odds_value);
    fprintf(stdout, "\n");
}

int player_placeBet(register struct player * player, register struct bet * bet)
{
    register int x;
    int e = -1;

    do {
        /* Set odds if caller didn't but passed an odds value */
        if (bet->odds_value > 0)
            bet->flags |= BETFL_ODDS;

        if (bet->type < 0 && bet->type > PLAYER_MAX_BETS)
            break;
        x = (int)bet->type;

        /* Update the bet type (player may have existing bet) */
        player->bets[x].type = bet->type;
        player->bets[x].value += bet->value;
        player->bets[x].flags |= bet->flags;

        /* Some bets (field, hardways, single roll bets) dont have odds bets 
         * quietly ignore an attempt to set an odds bet on one of these */
        /* also ignore attempts to place odds bets when no existing bet */
        if (player->bets[x].value > 0 && BET_TYPE_HAS_ODDS(x)) 
            player->bets[x].odds_value += bet->odds_value;
        else
            e = 1;

        e = 0; /* Success */
    } while (0);

    return e;
}

void player_display(const struct player * p)
{
    size_t i;

    for (i = 0; i < PLAYER_MAX_BETS; ++i) 
        if (p->bets[i].type != BET_t_invalid && p->bets[i].value != 0)
            bet_display(&p->bets[i]);
}

#define BOARDFL_COMEOUT 0x1
#define BOARD_MAX_PLAYERS 15

static const struct board {
    const char ** display;
    struct player players[BOARD_MAX_PLAYERS]; /* used to determine next shooter */
    short int shooter_id;               /* player id of the current shooter */
    short int flags;                   /* flags (see above) */
} board_initializer = {
    .display = NULL,
    .players = {{0}},
    .shooter_id = 0,
    .flags = 0
};

void board_display(const struct board * b)
{
    const char ** p = b->display;
    while (*p) 
        puts(*p++);
}

void game_loop(struct board * board)
{
    struct player * np;

    /* 1. get players playing */
    /* 2. start game loop */
    
}

int main(void)
{
    const char * pretty_dispboard[] = {
        " PASS DP DC DC   L4 L5 L6 L8 L9 L10 ",
        " PASS DP DC DC   P4 P5 P6 P8 P9 P10 ",
        " PASS DP DC DC   COME COME COME COME",
        " PASS DP DC DC   FIELD (2 3 4 10 12)",
        NULL
    };
    struct board board = board_initializer;
    board.display = pretty_dispboard;
    int player_count = 0;
    struct player * tmp;
    struct bet bet = {
        .value = 3.00,
        .type = BET_t_pass
    };
    
    /* Ok the general flow of the game should go as follows:
     * 1. Game begins all players enter their names
     * 2. Game state is set to COMEOUT (as this will be the comeout roll)
     * 3. A shooter is selected:
     *    a. Select the first player.
     *    b. If player passes, move to the next player in list
     *    c. If this is the last player in the list ask if they want
     *    us to automatically roll the dice (for now this is the only option
     *    these options will be more for the phone app which allows
     *    us to use the accelerometer to "throw" the dice)
     * 4. Bets will now be accepted
     *    * Place/Lay bets will be off by default unless requested by player
     *    * Pass/Dont Pass bets are on and can not be requested off
     *    * This only applies to the comeout roll, however a Player may request
     *    * Odds bets are only accepted once a point is established (ie state is _not_ COMEOUT)
     *    that all his bets be "turned off" at any point if he has to leave the table for some reason
     * 5. Dice shooted
     * 6. Any and all resolvable bets are resolved immediately
     * 7. If player hits 7 or 11 the game state stays in COMEOUT and goto step 4
     * 8. If player hits 2 3 or 12 the game state stays in COMEOUT and goto step 3
     * 9. If player hits any other number, a point is set COMEOUT is turned off goto step 4
     * 8. Go to step 3
     *
     */
    tmp = malloc(sizeof *tmp);
    if (!tmp)
        return -1;

    /* Do some shit so compiler shuts the fuck up... about unused (in this case YET to use) variables */
    *tmp = player_initializer;
    ++player_count;
    board.players[0] = *tmp;

    board_display(&board);

    /* Placing bet twice results in increment of bet  of that type */
    player_placeBet(tmp, &bet);
    player_placeBet(tmp, &bet);
    bet.type = BET_t_hard_4;
    bet.value = 100.0;
    player_placeBet(tmp, &bet);

    /* now lets place an odds bet on our pass line bet */
    bet.type = BET_t_pass;
    bet.value = 0;
    bet.odds_value = 500; /* we don't even have to set the odds bit flag, the func does it for us */
    player_placeBet(tmp, &bet);

    player_display(tmp);
    free(tmp);

    return 0;
}
