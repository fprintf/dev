SRC=vector.c
CFLAGS=-O2 --std=c99 -Wall -pedantic
SHLIB=libvector.so
SHLIBINC=-L. -lvector
TEST=testlibvec

all: $(SHLIB) $(TEST)

$(SHLIB): $(SRC)
	$(CC) -o $@ -fPIC -shared $(CFLAGS) $+

$(TEST): test.c
	$(CC) -o $@ $(SHLIBINC) $(CFLAGS) $+
	LD_LIBRARY_PATH=. ./$(TEST)

clean: $(SHLIB)
	$(RM) -f $(SHLIB) $(TEST)
