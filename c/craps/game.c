#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

/* Is number craps? (2, 3 or 12) */
#define is_craps(x) ((x) == 2 || (x) == (3) || (x) == 12)
/* Is number seven or eleven? */
#define is_sevlev(x) ((x) == 7 || (x) == 11)

#include <stdint.h>
#include "bet_types.h"
#include "player.h"

struct craps_data {
    struct { int a,b,total; } dice;
    struct player * ply;
    struct player * shooter; /* current shooter */
};
/* FIXME This shouldn't be a global, it should be a value in the player struct mostlikely... */
static short stop_bets;  /* quick hack, TODO FIXME FIXME TODO FIXME TODO 
                           TODO FIXME FIXME TODO FIXME */

struct craps {
    void (*next)(struct craps * state);
    struct craps_data * game;
};

int roll_dice(void)
{
    static unsigned int seed;
    int roll;
    double scale;
    
    if (!seed) {
        struct timeval tv;
        struct timezone tz;
        if (0 == gettimeofday(&tv, &tz)) {
            seed = tv.tv_usec - tv.tv_sec + tz.tz_minuteswest;
        } else {
            fprintf(stderr, "Insufficient entropy available, using insecure time() method..\n");
            seed = time((time_t *)NULL);
        }

        srand(seed);
    }

    /* Range 1 - 6, it would be (m - n + 1) * scale + n, normally but -1 +1 cancels out here.. */
    scale = (double)rand()/RAND_MAX;
    roll = 6 * scale + 1;
    return roll;
}

void get_bets(struct craps_data * game)
{
    int fail = 0;

    do {
        char rbuf[BUFSIZ];

        if (stop_bets)
            break;

        player.print(game->ply);
        printf("enter your bet, ex: 1 500\n");
        if (fgets(rbuf, sizeof rbuf, stdin)) {
            int bet_type;
            uint64_t cash;
            int r;

            if ((r = sscanf(rbuf, "%d %lu", &bet_type, &cash)) == 2) {
                if (bet_type == 1) {
                    if (! player.bet(game->ply, BET_pass, cash, 1) ) {
                        printf("%s made pass bet $%lu\n", player.name(game->ply), cash);
                    } else {
                        printf("%s, you're out of cash!\n", player.name(game->ply));
                        stop_bets = 1;
                    }
                    printf("%s made pass bet $%lu\n", player.name(game->ply), cash);
                } else {
                    if (! player.bet(game->ply, BET_dont_pass, cash, 0) ) {
                        printf("%s made dont pass bet $%lu\n", player.name(game->ply), cash);
                    } else {
                        printf("%s, you're out of cash!\n", player.name(game->ply));
                        stop_bets = 1;
                    }
                }
                fail = 0;
            } else if (r == 1 && bet_type == -1) {
                printf("%s has requested to stop betting until he loses his current bets\n",
                        player.name(game->ply));
                stop_bets = 1;
            }
        }

    } while(fail);
}

static void st_shooter(struct craps * state);          /* Find/select current "shooter" */
static void st_bets(struct craps * state);             /* Take bets */
static void st_roll(struct craps * state);             /* Roll the dice */
static void st_roll_point(struct craps * state);       /* Process a point roll (if in point state) */
static void st_roll_comeout(struct craps * state);     /* Process a comeout roll (if in comeout state) */



static void st_shooter(struct craps * state)
{
    /* Not implemented, just move on to taking bets */
    state->next = st_bets; 
    /* SElect the next shooter, for now the same one every time.. */
    state->game->shooter = state->game->ply;
}
static void st_bets(struct craps * state)
{
    /* Not implemented, just roll the dice */
    state->next = st_roll;
//    get_bets(state->game); /* get bets from the players */
}
static void st_roll(struct craps * state)
{
    state->game->dice.a = roll_dice();
    state->game->dice.b = roll_dice();
    state->game->dice.total = state->game->dice.a + state->game->dice.b;
    printf("roll %d, %d (%d)",
            state->game->dice.a, state->game->dice.b, state->game->dice.total
           );

    /* If we're in Point phase, or Comeout phase */
    if (player.pointno(state->game->shooter))
        state->next = st_roll_point;
    else
        state->next = st_roll_comeout;

    /* TODO We should place any general betting logic for bets
     * TODO that can be resolved on a single roll (hard ways, snakeyes, midnight, etc..)
     */
}
static void st_roll_point(struct craps * state)
{

    struct craps_data * game = state->game;
    /* Process player roll now */
    player.process(game->ply, player.pointno(game->ply), game->dice.total);
    player.print(game->ply);
    
    if (game->dice.total == 7) { /* back to comeout phase */
        printf("Shooter %s on %d Seven'd out! :(\n", player.name(game->shooter), player.pointno(game->shooter));
        player.set_point(state->game->shooter, 0);
        /* (state) new shooter */
        stop_bets = 0; /* new shooter, start taking bets again */
        state->next = st_shooter;
    } else {
        /* (state) take bets */
        state->next = st_bets;

        if (game->dice.total == player.pointno(game->ply)) {
            printf("Shooter %s on %d (pass bets) wins!\n", player.name(game->shooter), player.pointno(game->shooter)); 
        }else if (game->dice.total == 11)
            printf("--> yoleven (11) (no point, no field, shooter stays)\n");
        else if (is_craps(game->dice.total))
            printf("--> field win (craps) no point win, shooter stays\n");
        else
            printf("--> %d buy/lay wins\n",game->dice.total);
    }

}
static void st_roll_comeout(struct craps * state)
{
    struct craps_data * game = state->game;
    player.process(game->ply, player.pointno(game->ply), game->dice.total);
    player.print(game->ply);
    
    /* (state) unless CRAPS, keep taking bets */
    state->next = st_bets; 
    if (!is_craps(game->dice.total) && !is_sevlev(game->dice.total)) {
        printf("Shooter %s on %d\n", player.name(game->shooter), game->dice.total);
        player.set_point(state->game->shooter, state->game->dice.total);
    } else if (game->dice.total == 7) {
        printf("--> !! PASS WINS 7 !!\n");
    } else if (game->dice.total == 11) {
        printf("--> !! PASS WINS 11(yoleven) !!\n");
    } else {
        printf("Shooter %s (pass bets) crapped out :(\n", player.name(game->shooter)); /* (state) crapped out, new shooter */
        stop_bets = 0; /* new shooter, start taking bets again */
        state->next = st_shooter;
    }

    /* Process player roll now */
}

const char * state2str(void (*p)(struct craps * state))
{
    const char * s = "invalid";
    if (p == st_shooter) s = "shooter";
    else if (p == st_roll_point) s = "point";
    else if (p == st_roll_comeout) s = "comeout";
    else if (p == st_roll) s = "roll";
    else if (p == st_bets) s = "bets";

    return s;
}

int main(void)
{
    struct player * ply = player.new("Joel", 3000);
    struct craps_data game = {
        .ply = ply,
    };
    struct craps state = { 
        .next = st_shooter, 
        .game = &game 
    };

    while (state.next) {
        state.next(&state);
        sleep(1);
    }

    player.free(&ply);

    return 0;
}
