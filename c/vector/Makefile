SRC=vector.c
CFLAGS=-O2 --std=c99 -Wall -pedantic
SHLIB=libvector.so
SHLIBINC=-L. -lvector
EXAMPLES=vector_example
TEST=tests

all: $(SHLIB) 

test: $(SHLIB) $(TEST)
	LD_LIBRARY_PATH=. ./$(TEST)

examples: $(EXAMPLES)

$(SHLIB): $(SRC)
	$(CC) -o $@ -fPIC -shared $(CFLAGS) $+

$(EXAMPLES): example.c
	$(CC) -o $@ $(SHLIBINC) $(CFLAGS) $+

$(TEST): tests.c
	$(CC) -o $@ $(SHLIBINC) $(CFLAGS) $+

clean: $(SHLIB)
	$(RM) -f $(SHLIB) $(EXAMPLES) $(TEST)
