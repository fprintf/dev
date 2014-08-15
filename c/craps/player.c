#include <sys/queue.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "bet.h"
#include "player.h"

static const struct player {
    struct bets * bets;
    int pointno;
    char * name;
} player_initializer;

char * player_name(struct player * ply) { return ply->name; }

/*
 * Processes players bets depending on the dice roll 'roll'
 * that happened, pointno is the set point number. If set to
 * 0, then the roll is treated as a 'comeout' roll
 */
int player_evalbet(struct player * ply, int pointno, int roll)
{
    return bets_checkbets(ply->bets, pointno, roll);
}

struct player * player_new(const char * name, uint64_t cash) 
{
    struct player * ply = NULL;

    do {
        if ( !(ply = malloc(sizeof *ply)) ) 
            break;
        /* KEEP this for safety, will always make sure everything starts at 0 */
        *ply = player_initializer;

        /* Init new bets table */
        ply->bets = bets_newtable();

        /* Copy name and cash */
        if ( (ply->name = malloc(strlen(name)+1)) )
                strcpy(ply->name,name);
        bets_total_cash(ply->bets,cash);
    } while (0);

    return ply;
}

void player_free(struct player ** ply)
{
    /* Free bets table */
    bets_freetable(&(*ply)->bets);
    /* Free player object, and set pointer to NULL */
    free((*ply)->name);
    free(*ply); *ply = NULL;
}

/* Place a new bet or add to existing */
int player_placebet(struct player * ply, unsigned int bet_type, uint64_t cash, uint64_t odds_cash)
{
    /* Add the bet to player */
    if (bets_get_total_cash(ply->bets) < cash+odds_cash)
        return 1;

    bets_add(ply->bets, bet_type, cash, odds_cash);
    return 0; /* success */
}

void player_print(struct player * ply)
{
    printf("player %s cash %lu\n", ply->name, bets_get_total_cash(ply->bets));
}

char * player_getname(struct player * ply)
{
    return ply->name;
}
uint64_t player_getcash(struct player * ply)
{
    return bets_get_total_cash(ply->bets);
}

int player_pointno(struct player * ply) { return ply->pointno; }
int player_setpoint(struct player * ply, int point) { return (ply->pointno = point); }    


/* INITIALIZE API INTERFACE */
const struct player_api player = {
    .new = player_new,
    .free = player_free,

    .name = player_getname,
    .cash = player_getcash,
    .pointno = player_pointno,
    .set_point = player_setpoint,

    .bet = player_placebet,
    .process = player_evalbet,
    .print = player_print
};



