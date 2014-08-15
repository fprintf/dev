#ifndef PLAYER_HEADER__H_
#define PLAYER_HEADER__H_

extern const struct player_api player;

struct player_api {
    struct player * (*new)(const char * name, uint64_t total_cash);
    void (*free)(struct player ** ply);

    char * (*name)(struct player * ply);
    uint64_t (*cash)(struct player * ply);
    int (*pointno)(struct player * ply);
    int (*set_point)(struct player * ply, int point);

    void (*print)(struct player * ply); /* prints information about player */
    /* Make a bet */
    int (*bet)(struct player * ply, unsigned int bet_type, uint64_t cash, uint64_t odds_cash);
    /* Process all bets player has, after a roll */
    int (*process)(struct player * ply, int pointno, int roll);
};

#endif
