#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <unistd.h>

#include <htable.h>

unsigned int hash_time(const time_t * t)
{
    unsigned char * p = (unsigned char *)t, * end = p + sizeof *t - 1;
    unsigned int h = 0;

    while ( p < end )
        h ^= ( h << 5 ) + ( h >> 2 ) + *p++;

    return h;
}

int printval(const char * key, void * data)
{
    char * word = data;
    printf("%s: %s\n", key, word);
    return 0;
}

void freestring(const char * key, void * data)
{
    char * s = data;
    free(s);
}

int main(void)
{
    time_t now = time(NULL);
    struct htable * htab = htable.new(500000);
    char buf[BUFSIZ];


    srand(hash_time(&now));

    while ( fgets(buf, sizeof buf, stdin) ) {
        char * chop = strchr(buf, '\n');
        if (chop) *chop = 0;

        htable.store(htab, buf, strdup(buf));
    }

    printf("direct: %lu\nlinked: %lu\ntotal: %lu htable size: %lu\nmaxdepth: %d loadfactor: %lg\n",
            htable.direct(htab), htable.linked(htab), htable.total(htab), htable.size(htab), htable.load(htab),
            (double)htable.total(htab)/(double)htable.size(htab)
          );
//    htable.foreach(htab, printval);

    /* Destroy htable */
    /* Free all the strings we created as well (above) */
    htable.free_cb(htab, freestring);

    return 0;
}
