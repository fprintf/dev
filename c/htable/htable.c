#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "htable.h"

static const struct hentry {
    char * key; 
    size_t key_sz; 
    struct hentry * next;
    void * data;
} hentry_initializer;

static const struct htable {
    struct hentry ** tbl; /* this allows us to allocate each entry individually, we just store a table of pointers */
    size_t tbl_size;

    /* Stats */
    size_t tbl_direct, tbl_linked, tbl_total;
    int load;
} htable_initializer;

/*
 * CURRENT HASH FUNCTION WE'RE USING (THERE ARE MORE AT THE BOTTOM OF THIS FILE)
 */

/* This hash performs just as good as jenkins hash, for string types
 * which is the type required for keys.. :) */
static size_t htable_shift_add_xor_hash(const char * key)
{
    size_t h = 0;

    while ( *key ) 
        h ^= ( h << 5 ) + ( h >> 2 ) + *key++;

    return h;
}


/* Create a table of 2*'size'...will resize as necessary but
 * resizing is very expensive so start out with the size you
 * think you'll need  in a worst case scenario
 *
 * The actual size used will be the nearest prime number
 * larger then 2*'size'
 */
struct htable * htable_new(size_t size)
{
    struct htable * tbl;

    do {
        size_t nsize = 1;
        /* Initialize table */
        if ( !(tbl = malloc(sizeof *tbl)) ) 
            break;
        *tbl = htable_initializer;
        /* Find next power of 2 larger then 'size' */
        do nsize *= 2; while (size >>= 1); /* there is a faster method but even with 1000bit int this wouldn't take very long */
        tbl->tbl_size = nsize;
//        fprintf(stderr,"Requested size %lu.. using next larger power of 2 %lu\n", size, tbl->tbl_size);
        /* Allocate array[tbl_size] of void * (pointer to void) */
        tbl->tbl = calloc(tbl->tbl_size, sizeof *tbl->tbl);
        if (!tbl->tbl) {
            free(tbl);
            tbl = NULL;
            break;
        }
    } while(0);

    return tbl;
}

void htable_free_cb(struct htable * htab, void (*cb)(const char * key, void * data))
{
    size_t i;
    assert(htab != NULL);

    /* Free each entry..(expensive!!) :( */
    for (i = 0; i < htab->tbl_size; ++i) 
        if (htab->tbl[i]) {
            /* Free linked list in each entry, and key for each entry */
            struct hentry * p = htab->tbl[i], * tmp;

            while (p) {
                tmp = p->next;
                /* Call to our user cleanup function, if any */
                if (cb)
                    cb(p->key, p->data);
                free(p->key);
                free(p);
                p = tmp;
            }
        }
    free(htab->tbl);
    free(htab);
}

void htable_free(struct htable * htab)
{
    htable_free_cb(htab, NULL);
}




/*
 * Lookup key in our table and return a pointer
 * to the entry (if any). The pointer returned may in fact
 * be the insertion point (->next pointer in the entry) and not
 * the key itself.
 *
 * If 'found' is true then the returned value is in fact the
 * key you were looking for. If found is 0 then the returned value is
 * one of the following:
 *
 * NULL -> item not found and insertion point would be the table index at 'idx' (below)
 * valid pointer -> item not found pointer is the insertion point in the linked list we should store this key
 *
 * 'last' will be the previous pointer to this pointer (if it was a linked list entry..otherwise itw ill be NULL)
 * this is used for deletion and can be NULL then will be ignored
 */
/* This is our macro alias to our current hash function we're using */
#define get_key_index(htab,key) (htable_shift_add_xor_hash(key) & ((htab)->tbl_size-1))
struct hentry * get_key(struct htable * htab, const char * key, struct hentry ** last, size_t * index, int * found)
{
    struct hentry * he = NULL;
    int depth = 0;
    *index = get_key_index(htab,key);
    *last = NULL;
    *found = 0;

    /* Return value from the amount of bits that are in tbl_size */
    he = htab->tbl[*index];
    if (he) {
        while ( (*found = strncmp(key,he->key,he->key_sz)) && he->next ){
            ++depth;
//            printf("%s -> ", he->key);
            *last = he, he = he->next;
        }

        /* Above it's actually "not" found, so inverse the result */
        *found = !*found;
        if (htab->load < depth)
            htab->load = depth;
    }

    return he;
}
/*
 * Search 'htab' for 'key' returning it if found. If item is not NULL
 * Do one of the following:
 * if 'key' exists, leave it untouched as well as it's value
 * if 'key' doesn't exist, add it with value 'item' (return NEW item)
 * If 'item' is NULL then just return the existing item or NULL of none
 */
static void * htable_store(struct htable * htab, const char * key, void * data)
{
    size_t idx;
    int found;
    struct hentry * ne;
    struct hentry * he = get_key(htab, key, &ne, &idx, &found);
    void * rdata = NULL;

    do {
        if (!data)
            break;
        ne = malloc(sizeof *ne);
        if (!ne)
            break;

        /* Init entry with our key and data */
        *ne = hentry_initializer;
        ne->key_sz = strlen(key);

        ne->key = malloc(ne->key_sz+1);
        if (!ne->key) {
            free(ne);
            break;
        }
        /* assign values */
        strcpy(ne->key,key);
        rdata = ne->data = data;

        /* add or update item to hashtable */
        if (!he) {
            htab->tbl[idx] = ne;
            ++htab->tbl_direct;
            ++htab->tbl_total;
            //printf("storing '%s' at '%lu'\n", key, idx);
        } else if (!found) {
            //printf("'%s' -> linked '%s'\n", he->key, key);
            ne->next = he->next;
            he->next = ne;
            ++htab->tbl_linked;
            ++htab->tbl_total;
        } else { /* key already exists, free new item but update data object */
            free(ne->key);
            free(ne);
            /* Update entry and return old data */
            rdata = he->data;
            he->data = data;
        }
    } while (0);

    return rdata;
}

static void * htable_lookup(struct htable * htab, const char * key)
{
    size_t idx;
    int found;
    struct hentry * ne = NULL;
    struct hentry * he = get_key(htab, key, &ne, &idx, &found);
    void * rdata = NULL;

    if (he && found) 
        rdata = he->data;

    return rdata;
}

static void * htable_delete(struct htable * htab, const char * key)
{
    size_t idx; 
    int found;
    struct hentry * le;
    struct hentry * he = get_key(htab, key, &le, &idx, &found);
    void * rdata = NULL;

    if ( he && found ) {
        rdata = he->data;
        /* Destroy entry */
            --htab->tbl_total;
        if (le) --htab->tbl_linked, le->next = he->next; /* we might be deleting in center of list */
        else --htab->tbl_direct, htab->tbl[idx] = NULL;
        free(he->key);
        free(he);
    }

    return rdata;
}

/* Note, this can be an expensive call if we have a very large hash table */
static void htable_foreach(struct htable * htab, int (*cb)(const char * key, void * data))
{
    size_t i;
    struct hentry * he;

    for (i = 0; i < htab->tbl_size; ++i) {
        he = htab->tbl[i];
        /* Empty, continue on */
        if (!he) continue;

        /* Execute our callback for each entry, including linked list entries.. */
        while (he) {
            if (cb(he->key, he->data)) /* non zero return indicates we should end here */
                break;
            he = he->next;
        }
    }
}


int htable_load(struct htable * htab) { return htab->load; }
size_t htable_direct(struct htable * htab) { return htab->tbl_direct; }
size_t htable_linked(struct htable * htab) { return htab->tbl_linked; }
size_t htable_total(struct htable * htab) { return htab->tbl_total; }
size_t htable_size(struct htable * htab) { return htab->tbl_size; }

/*
 * Install API handlers
 */
const struct htable_api htable = {
    .new = htable_new,
    .free = htable_free,
    .free_cb = htable_free_cb,

    .lookup = htable_lookup,
    .delete = htable_delete,
    .store = htable_store, 

    .load = htable_load,
    .direct = htable_direct,
    .linked = htable_linked,
    .size = htable_size,
    .total = htable_total,

    .foreach = htable_foreach,
};

















/*
 * TEST HARNESS
 */
#if 0
int main(void)
{
    struct htable * htab;
    int i = 5;
    char line[BUFSIZ];

    htab = htable_new(25);
    while ( fgets(line, sizeof line, stdin) ) {
        char * p = strchr(line, '\n');
        if (p) *p = 0;
        htable_store(htab, line, &i);
    }
    htable_free(htab);

    return 0;
}
#endif











/*************************************************************************************
 * HASH RELATED FUNCTIONS THAT ARE GOOD TO HAVE BUT NOT CURRENTLY USED
 *************************************************************************************/

#if 0

/*
 *
 * Best possible deterministic test (aside from using lookup tables)
 * for primality
 *
 * Probablistic methods are faster but test
 * for "most likely a prime"..this PROVES its a prime
 *
 */
static int is_prime(size_t num)
{
    register size_t cur = 0, sqt;

    if (num % 2 && num % 3) {
        /* If number is indivisible by all numbers from 
         * 6k+or-1 to sqrt(num) it is prime 
         * Where k = 1,2,3,4,5 etc..
         */
        sqt = sqrt(num);
        for (cur = 5; cur <= sqt; cur+=6) {
            if ( !(num % cur) || !(num % (cur+2)) )
                break;
        }
        /* Was prime? set return value to 1 */
        cur = (cur > sqt);
    }
    /* 1 is prime, anything else is nonprime */
    return cur == 1;
}

/*
 * Miscellaneous other hash functions we might use later..
 */
static size_t htable_fnv_hash(const unsigned char *key)
{
    size_t h = 2166136261;

    while ( *key )
        h = ( h * 16777619 ) ^ *key++;

    return h;
}

static size_t htable_elf_hash(const unsigned char * key)
{
   size_t h = 0, g;
 
    while ( *key ) {
     h = ( h << 4 ) + *key++;
     g = h & 0xf0000000L;
 
     if ( g != 0 )
       h ^= g >> 24;
 
     h &= ~g;
   }
 
   return h;
}


#define mix(a,b,c) \
 { \
   a -= b; a -= c; a ^= ( c >> 13 ); \
   b -= c; b -= a; b ^= ( a << 8 ); \
   c -= a; c -= b; c ^= ( b >> 13 ); \
   a -= b; a -= c; a ^= ( c >> 12 ); \
   b -= c; b -= a; b ^= ( a << 16 ); \
   c -= a; c -= b; c ^= ( b >> 5 ); \
   a -= b; a -= c; a ^= ( c >> 3 ); \
   b -= c; b -= a; b ^= ( a << 10 ); \
   c -= a; c -= b; c ^= ( b >> 15 ); \
 }
 
static size_t htable_jen_hash( const unsigned char *k, size_t length, size_t initval )
{
   unsigned a, b;
   size_t c = initval;
   size_t len = length;

   a = b = 0x9e3779b9;

   while ( len >= 12 ) {
      a += ( k[0] + ( (unsigned)k[1] << 8 ) 
        + ( (unsigned)k[2] << 16 )
        + ( (unsigned)k[3] << 24 ) );
      b += ( k[4] + ( (unsigned)k[5] << 8 ) 
        + ( (unsigned)k[6] << 16 )
        + ( (unsigned)k[7] << 24 ) );
      c += ( k[8] + ( (unsigned)k[9] << 8 ) 
        + ( (unsigned)k[10] << 16 )
        + ( (unsigned)k[11] << 24 ) );

      mix ( a, b, c );

      k += 12;
      len -= 12;
   }

   c += length;

   switch ( len ) {
   case 11: c += ( (unsigned)k[10] << 24 );
   case 10: c += ( (unsigned)k[9] << 16 );
   case 9 : c += ( (unsigned)k[8] << 8 );
   /* First byte of c reserved for length */
   case 8 : b += ( (unsigned)k[7] << 24 );
   case 7 : b += ( (unsigned)k[6] << 16 );
   case 6 : b += ( (unsigned)k[5] << 8 );
   case 5 : b += k[4];
   case 4 : a += ( (unsigned)k[3] << 24 );
   case 3 : a += ( (unsigned)k[2] << 16 );
   case 2 : a += ( (unsigned)k[1] << 8 );
   case 1 : a += k[0];
   }

   mix ( a, b, c );

   return c;
}

static size_t htable_one_at_a_time_hash(const char * key)
{
   size_t h = 0;
 
   while ( *key ) {
     h += *key++;
     h += ( h << 10 );
     h ^= ( h >> 6 );
   }
 
   h += ( h << 3 );
   h ^= ( h >> 11 );
   h += ( h << 15 );
 
   return h;
}


#endif
