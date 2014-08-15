SRC=player.c bet.c 
OBJ=$(patsubst %.c,%.o,$(SRC))
MAIN=game.c
PROG=craps
CFLAGS=-g -Wall

.PHONY:	clean 

all: $(OBJ) $(PROG)

clean:
	$(RM) $(OBJ) $(PROG)

$(PROG): $(OBJ) $(MAIN)
	$(CC) -o $(PROG) $+ $(CFLAGS)

$(OBJ): %.o: %.c
	$(CC) -o $@ $+ -c $(CFLAGS) -fPIC -fomit-frame-pointer


