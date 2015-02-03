SRC=permute.c
PROGRAM=permute
CFLAGS=-Wall

PREFIX=/usr
BINDIR=$(PREFIX)/bin

.PHONY:	all clean install

all: $(PROGRAM)

clean: $(PROGRAM)
	$(RM) $(PROGRAM)

install: $(PROGRAM)
	cp $(PROGRAM) $(BINDIR)

$(PROGRAM): $(SRC)
	$(CC) -o $@ $(CFLAGS) $+


