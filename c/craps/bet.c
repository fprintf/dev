#include <stdint.h>
#include <stdlib.h>

enum bet_flags {
    BET_FL_NONE,
    BET_FL_ODDS = 1 << 0, /* odds bet, on type of actual bet type */
};


#include "bet_types.h"
#define BET_INRANGE(x) ((x) >= 0 && (x) < BET_TYPE_MAX)

static const struct bet {
    enum bet_flags flags; /* Type of bet, possible odds bet  flags? */
    uint64_t cash;      /* measured in pennies */
    uint64_t odds_cash; /* measured in pennies */
} bet_initializer;

static const struct bets {
    struct bet type[BET_TYPE_MAX];
    uint64_t total_cash;  /* total cash in account */
} bets_initializer;

uint64_t bet_cash(struct bet * bet) { return bet->cash; }
uint64_t bet_odds_cash(struct bet * bet) { return bet->odds_cash; }


struct bets * bets_newtable(void)
{
    struct bets * bets;

    if ( (bets = malloc(sizeof *bets)) )
        *bets = bets_initializer;

    return bets;
}

void bets_freetable(struct bets ** bets)
{
    free(*bets); 
    *bets = NULL;
}

uint64_t bets_get_total_cash(struct bets * bets)
{
    return bets->total_cash;
}

/* Adds, returns or initializes total cash value to 'total_cash' 
 * if set to 0 just returns current total_cash value */
uint64_t bets_total_cash(struct bets * bets, uint64_t total_cash)
{
    return (bets->total_cash += total_cash);
}



/* ********************************************************************************
 * ACTUAL BET PROCESSING ROUTINES 
 * ********************************************************************************/

/*
 * Perform an operation on a bet and the associated bets structure cash values. 
 * Operations are as follows:
 * '+'  Add the bet and delete it (factor is ignored)
 * '-'  Subtract the bet and delete it (factor is ignored)
 * '*'  Multiply the bet cash (not odds cash) by factor and add it (and odds cash 1:1) then delete it
 * '@'  Multiply the bet cash  and odds cash by factor and add them hen delete bet
 */
void bet_checkbets_op(struct bets * bets, size_t type, unsigned char op, uint64_t factor)
{
    register struct bet * bet = &bets->type[type];

    /* If there is actually such a bet.. */
    if (bet->cash > 0) {
        switch(op) {
            case '+': 
                bets->total_cash += bet->cash + bet->odds_cash; 
                break;
            case '-': 
                /* Do nothing, we already subtract the bet value when
                 * we make the bet initially */
                break;
            case '*':
                bets->total_cash += factor * bet->cash + bet->odds_cash;
                break;
            case '@': 
                bets->total_cash += factor * bet->cash + factor * bet->odds_cash; 
                break;
        }
        /* Now delete the bet from the bets table */
        bet->cash = 0;
        bet->odds_cash = 0;
    }
}

int bets_checkbets_point(struct bets * bets, int pointno, int roll)
{
    if (roll == 7) {
        /* PASS line bets lose */
        bet_checkbets_op(bets, BET_pass, '-', 0);
        /* DONT pass line bets win 2:1 */
        bet_checkbets_op(bets, BET_dont_pass, '*', 2);
    } else if (roll == pointno) {
        /* Point hit, pass line (now point) wins! 2:1 payout */
        bet_checkbets_op(bets, BET_pass, '*', 2);
        /* Point hit, dont pass line (now point) loses! */
        bet_checkbets_op(bets, BET_dont_pass, '-', 0);
    }
    return roll;
}
int bets_checkbets_comeout(struct bets * bets, int roll)
{
    switch(roll) {
        case 11:/* WIN */
        case 7: /* WIN */
            /* PASS line bets win 2:1 */
            bet_checkbets_op(bets, BET_pass, '*', 2);
            /* DONT pass line bets lose  */
            bet_checkbets_op(bets, BET_dont_pass, '-', 0);
            break;


        /* CRAPPED OUT ! */
        case 2:
        case 3:
        case 12:
            /* DONT pass wins 2:1 */
            bet_checkbets_op(bets, BET_dont_pass, '*', 2);
            /* PASS loses! crapped out */
            bet_checkbets_op(bets, BET_pass, '-', 0);
        default: /* nothing .. */
            break;
    }
    return roll;
}

int bets_checkbets(struct bets * bets, int pointno, int roll)
{
    int ret;

    /* Point set */
    if (pointno > 0) 
        ret = bets_checkbets_point(bets, pointno, roll);
    else
    /* Comeout roll */
        ret = bets_checkbets_comeout(bets, roll);

    return ret;
}


struct bet * bets_add(struct bets * bets, unsigned int bet_type, uint64_t cash, uint64_t odds_cash)
{
    struct bet * cbet = NULL;
    
    if (BET_INRANGE(bet_type)) {
        /* subtract bet from cash pool */
        bets->total_cash -= cash + odds_cash;

        /* Add the bet to the board */
        cbet = &bets->type[bet_type];
        /* Add bet to existing (if any) */
        cbet->cash += cash;
        /* Copy odds cash, if any, and mark this is an ODDS bet now */
        if (odds_cash > 0) {
            cbet->odds_cash += odds_cash;
            cbet->flags = BET_FL_ODDS;
        }
    }

    return cbet;
}

struct bet * bets_get(struct bets * bets, unsigned int bet_type)
{
    struct bet * bet = NULL;
    if (BET_INRANGE(bet_type))
        bet = &bets->type[bet_type];
    return bet;
}

