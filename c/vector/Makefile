SRC=vector.c
CFLAGS=-O2 --std=c99 -Wall -pedantic
SHLIBHEADER=vector.h
SHLIB=libvector.so
SHLIBINC=-L. -lvector
TEST=testlibvec
INSTALLPREFIX=../share

.PHONY: all test clean install

all: $(SHLIB) 

test: $(TEST)

install: $(SHLIB) 
	cp $(SHLIB) $(INSTALLPREFIX)/lib/
	cp $(SHLIBHEADER) $(INSTALLPREFIX)/include/


$(SHLIB): $(SRC)
	$(CC) -o $@ -fPIC -shared $(CFLAGS) $+

$(TEST): test.c
	$(CC) -o $@ $(SHLIBINC) $(CFLAGS) $+
	LD_LIBRARY_PATH=. ./$(TEST)

clean: $(SHLIB)
	$(RM) -f $(SHLIB) $(TEST)
