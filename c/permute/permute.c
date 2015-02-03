#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <unistd.h> /* getopt() */

#define PROGNAME "permute"
#define VERSION  1.0
#define AUTHOR   "fprintf"

#define usage()         do{                                     \
    fprintf(stderr, "usage %s [-h?]"                            \
                    "[-s startlength] [-e endlength]"           \
                    " <string> [[string] ...]\n",               \
                                   argv[0]);                    \
} while(0)

void print(ptrdiff_t * output, size_t size, const char * letters)
{
    size_t i;
    for (i = 0; i < size; ++i) 
        putchar(*(letters + output[i]));
    putchar('\n');
}

/*
 * Print all combinations of 'letters' from lengths
 * 'start' to lengths 'end'
 */
void permute(const char * letters, size_t start, size_t size)
{
    ptrdiff_t * output; size_t len = strlen(letters);

    if ( !(output = calloc(len, sizeof *output)) ) {
        fprintf(stderr, "[error] Out of memory!\n");
        return;
    }

    /* Setup defaults */
    if (start == -1) 
        start = size > 0 ? size - 1 : 0;
    size = size > 0 && size <= len ? size : len;

    /* List the permutations */
    while (start < size) {
        /* Output current permutation */
        print(output, start + 1, letters);

        /* Move to the next */
        ptrdiff_t back = start;
        char cont;
        do {
            cont = 0;
            if (++output[back] >= len) {
                output[back--] = 0;
                cont = 1;
                if (back < 0) /* Ok we're done for this length, increase length position */
                    ++start;
            }
        } while (back >= 0 && cont);
    }

    free(output);
}

int main(int argc, char ** argv)
{
    extern char * optarg;
    extern int optind;
    int opt;
    size_t start = -1, end = 0; /* Defaults */

    while ((opt = getopt(argc, argv, "h?s:e:")) != -1) {
        switch (opt) {
            case 'h': /* Show help */
                fprintf(stderr, 
                        "%s %.1f by %s\n"    // Program name, version, author
                        "SYNOPSIS\n"
                        "\t%s [-h?] [-s start] [-e end] <string> [[string] ...]\n"
                        "\n" // End section
                        "DESCRIPTION\n"
                        "permute the given string on the command line, printing each permutation\n"
                        "to stdout. start specifies the starting length to permute from and end\n"
                        "specifies where to end.\n"
                        "\n"
                        "OPTIONS\n"
                        "\t-h, -?  \t\tprint this help\n"
                        "\t-s start\t\tpermute the given string starting from this length - 1.\n"
                        "\t        \t\tdefault: 0\n"
                        "\t-e end  \t\tpermute the given string (from start) to this length.\n"
                        "\t        \t\tdefault: string length\n"
                        "\n",
                        PROGNAME, VERSION, AUTHOR,
                        PROGNAME
                       );
                /* FALL THROUGH */
            case '?':
            default: 
                usage();
                exit(EXIT_FAILURE);
                break;
            case 's':
                if (sscanf(optarg, "%zu", &start) != 1) { 
                    fprintf(stderr, "[error] invalid start length: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            case 'e':
                if (sscanf(optarg, "%zu", &end) != 1) { 
                    fprintf(stderr, "[error] invalid end length: %s\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "[error] please provide a string, try -h for help.\n");
        exit(EXIT_FAILURE);
    }

    /* Permute all the given strings */
    for (; optind < argc; ++optind) 
        permute(argv[optind], start, end);
	return 0;
}
