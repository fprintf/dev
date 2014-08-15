#ifndef BET_HEADER__H_
#define BET_HEADER__H_

struct bet;
struct bets;

struct bets * bets_newtable(void);
void bets_freetable(struct bets ** bets);

struct bet * bets_add(struct bets * bets, unsigned int bet_type, uint64_t cash, uint64_t odds_cash);
struct bet * bets_get(struct bets * bets, unsigned int bet_type);
/* Set the total cash in the bet */
uint64_t bets_get_total_cash(struct bets * bets);
uint64_t bets_total_cash(struct bets * bets, uint64_t total_cash);
/* Checks all bets against 'roll' and adjusts the cash value */
int bets_checkbets(struct bets * bets, int pointno, int roll);

uint64_t bet_cash(struct bet * bet);
uint64_t bet_odds_cash(struct bet * bet);

#endif
