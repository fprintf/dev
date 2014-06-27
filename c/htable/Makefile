SRC=htable.c
OBJ=$(patsubst %.c,%.o,$(SRC))
MAIN=main.c
PROG=test_htable
CFLAGS=-O2 -Wall -g
SHLIB=libhtable.so

.PHONY:	clean 

all: $(OBJ) $(SHLIB) $(PROG)

clean:
	$(RM) $(OBJ) $(PROG) $(SHLIB)

$(SHLIB): $(OBJ)
	$(CC) -o $@ $+ -shared 

$(PROG): $(SHLIB) $(MAIN) 
	$(CC) -o $@ $+ $(CFLAGS) -I. -L. -lhtable

$(OBJ): %.o: %.c
	$(CC) -o $@ $+ -c $(CFLAGS) -fPIC -fomit-frame-pointer


