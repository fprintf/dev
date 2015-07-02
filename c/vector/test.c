#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include "vector.h"

struct testdata {
    const char * name;
    int age;
};

static const struct testdata fake_info[] = {
    {"Joel", 15},
    {"Jackup", 25},
    {"Nooster", 32},
};

static size_t global_test_count, global_test_failures;
#define OK(test, label) do {                                                    \
    ++global_test_count;                                                        \
    if (!(test)) {                                                              \
        ++global_test_failures;                                                 \
        fprintf(stdout, "test #%zu '%s' failed\n", global_test_count, label);   \
    } else {                                                                    \
        fprintf(stdout, "test #%zu '%s' ok\n", global_test_count, label);       \
    }                                                                           \
} while(0)

#define TEST_REPORT(...) do {  \
    if (global_test_failures)         \
        fprintf(stderr, "%zu/%zu test(s) failed -- see above for more information\n",  \
            global_test_failures, global_test_count);      \
    else                                  \
        fprintf(stdout, "All %zu tests passed! OK\n", global_test_count); \
} while(0)

int main(int argc, char ** argv)
{
    struct vector * vec = vector.new(0);

    OK(vec != NULL, "struct vector * created");

    /* Test the resizing ability */
    for (size_t i = 0; i < 10; ++i) {
        struct testdata * data = malloc(sizeof *data);
        if (!data) {
            perror("malloc");
            return 0;
        }

        if (i < sizeof fake_info/sizeof *fake_info) 
            *data = fake_info[i];
        else {
            data->name = "Test Data";
            data->age = i;
        }

        vector.sindex(vec, i, data);
        OK(vector.top(vec) == vector.index(vec, vector.size(vec) - 1), "does vector.top == vector.index(vec, size - 1)");
        struct testdata * element = vector.top(vec);
        fprintf(stderr, "name: %s age: %i\n", element->name, element->age);
        OK(vector.top(vec) != vector.index(vec, vector.size(vec) - 2), "does vector.top != vector.index(vec, size - 2)");
    }

    TEST_REPORT();

	return 0;
}
