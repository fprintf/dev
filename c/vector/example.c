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
    {"Jim", 15},
    {"John", 25},
    {"Jacop", 32},
    {"Fred", 59},
};

/* Used as a callback vector.foreach below 
 * NOTE how you can define the parameter as
 * any pointer, doesn't have to be void *
 */
int print_testdata(struct testdata * item) {
    printf("name: %s age: %d\n", item->name, item->age);
    return 1;
}

int main(int argc, char ** argv)
{
    struct vector * vec = vector.new(0);

    /* Resize vector */
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

    /* Print data using the vector.foreach method */
    vector.foreach(vec, print_testdata);

    /* Release our data we created during the resize
     * and loop through our vector using the raw
     * entries
     */
    void ** entries = vector.entries(vec);
    for (size_t i = 0; i < vector.size(vec); ++i) {
        struct testdata * data = entries[i];
        free(data);
    }

    /* Release the vector */
    vector.delete(vec);

	return 0;
}
