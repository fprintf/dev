/*
 * TODO: Examples should be more fleshed out
 */
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

int print_testdata(struct testdata * item) {
    printf("name: %s age: %d\n", item->name, item->age);
    return 1;
}

int main(int argc, char ** argv)
{
    struct vector * vec = vector.new(0);

    /* Test the resizing ability */
    for (size_t i = 0; i < 5000; ++i) {
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
    }

    /* Print data checking the vector.foreach method */
    vector.foreach(vec, print_testdata);

    /* Free data checking the vector.entries method */
    void ** entries = vector.entries(vec);
    for (size_t i = 0; i < vector.size(vec); ++i) {
        struct testdata * data = entries[i];
        free(data);
    }

    /* Test resize and then accessing an element in between */
    vector.sindex(vec, 5100, NULL);
    for (ptrdiff_t i = 4980; i < 5100; ++i)
        fprintf(stdout, "vec[%zd] = %p\n", i, vector.index(vec,i));

	return 0;
}
