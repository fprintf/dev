SRC=htable.c
OBJ=$(patsubst %.c,%.o,$(SRC))
MAIN=main.c
PROG=test_htable
CFLAGS=-O2 -Wall -g
SHLIB=libhtable.so
STATICLIB=libhtable.a

.PHONY:	clean 

all: $(OBJ) $(SHLIB) $(STATICLIB) $(PROG)

clean:
	$(RM) $(OBJ) $(PROG) $(SHLIB) $(STATICLIB)

$(SHLIB): $(OBJ)
	$(CC) -o $@ $+ -shared 

$(STATICLIB): $(OBJ)
	$(CC) -o $@ $+ -static 

$(PROG): $(SHLIB) $(MAIN) 
	$(CC) -o $@ $+ $(CFLAGS) -I. -L. -lhtable

$(OBJ): %.o: %.c
	$(CC) -o $@ $+ -c $(CFLAGS) -fPIC -fomit-frame-pointer


