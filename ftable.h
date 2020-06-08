#ifndef FTABLE_FTABLE_H
#define FTABLE_FTABLE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LOAD 0.75

#define __x86

/* Set uin as uint32_t or uint64_t depending on system */
#ifdef __x86
typedef uint32_t uin;

/* Table of bitmasks to use */
const uin mask[] = {
        0x7,                0xF,
        0x1F,               0x3F,               0x7F,               0xFF,
        0x1FF,              0x3FF,              0x7FF,              0xFFF,
        0x1FFF,             0x3FFF,             0x7FFF,             0xFFFF,
        0x1FFFF,            0x3FFFF,            0x7FFFF,            0xFFFFF,
        0x1FFFFF,           0x3FFFFF,           0x7FFFFF,           0xFFFFFF,
        0x1FFFFFF,          0x3FFFFFF,          0x7FFFFFF,          0xFFFFFFF,
        0x1FFFFFFF,         0x3FFFFFFF,         0x7FFFFFFF,         0xFFFFFFFF,
};


/* Table of prime number sizes, each approx double the prev, that fits
 * into a uint32_t */
const uin tableSizes[] = {
        5, 11, 23, 47, 97, 197, 397, 797, 1597,
        3203, 6421, 12853, 25717, 51437, 102877,
        205759, 411527, 823117, 1646237, 3292489,
        6584983, 13169977, 26339969, 52679969,
        105359939, 210719881, 421439783, 842879579,
        1685759167, 3371518343 };

#elif __x86_64
typedef uint64_t uin;

/* Table of bitmasks to use */
const uin mask[] = {
        0x7,                0xF,
        0x1F,               0x3F,               0x7F,               0xFF,
        0x1FF,              0x3FF,              0x7FF,              0xFFF,
        0x1FFF,             0x3FFF,             0x7FFF,             0xFFFF,
        0x1FFFF,            0x3FFFF,            0x7FFFF,            0xFFFFF,
        0x1FFFFF,           0x3FFFFF,           0x7FFFFF,           0xFFFFFF,
        0x1FFFFFF,          0x3FFFFFF,          0x7FFFFFF,          0xFFFFFFF,
        0x1FFFFFFF,         0x3FFFFFFF,         0x7FFFFFFF,         0xFFFFFFFF,
        0x1FFFFFFFF,        0x3FFFFFFFF,        0x7FFFFFFFF,        0xFFFFFFFFF,
        0x1FFFFFFFFF,       0x3FFFFFFFFF,       0x7FFFFFFFFF,       0xFFFFFFFFFF,
        0x1FFFFFFFFFF,      0x3FFFFFFFFFF,      0x7FFFFFFFFFF,      0xFFFFFFFFFFF,
        0x1FFFFFFFFFFF,     0x3FFFFFFFFFFF,     0x7FFFFFFFFFFF,     0xFFFFFFFFFFFF,
        0x1FFFFFFFFFFFF,    0x3FFFFFFFFFFFF,    0x7FFFFFFFFFFFF,    0xFFFFFFFFFFFFF,
        0x1FFFFFFFFFFFFF,   0x3FFFFFFFFFFFFF,   0x7FFFFFFFFFFFFF,   0xFFFFFFFFFFFFFF,
        0x1FFFFFFFFFFFFFF,  0x3FFFFFFFFFFFFFF,  0x7FFFFFFFFFFFFFF,  0xFFFFFFFFFFFFFFF,
        0x1FFFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF,

};

/* Table of prime number sizes, each approx double the prev, that fits
 * into a uint64_t */
const uin tableSizes[] = {
        5, 11, 23, 47, 97, 197, 397, 797, 1597,
        3203, 6421, 12853, 25717, 51437, 102877,
        205759, 411527, 823117, 1646237, 3292489,
        6584983, 13169977, 26339969, 52679969,
        105359939, 210719881, 421439783, 842879579,
        1685759167, 3371518343, 6743036717, 13486073473,
        26972146961, 53944293929, 107888587883,
        215777175787, 431554351609, 863108703229,
        1726217406467, 3452434812973, 6904869625999,
        13809739252051, 27619478504183, 55238957008387,
        110477914016779, 220955828033581, 441911656067171,
        883823312134381, 1767646624268779, 3535293248537579,
        7070586497075177, 14141172994150357,
        28282345988300791, 56564691976601587,
        113129383953203213, 226258767906406483,
        452517535812813007, 905035071625626043,
        1810070143251252131, 3620140286502504283,
        7240280573005008577, 14480561146010017169,
        18446744073709551557};

#endif

#undef __x86


/* Linear probing max distance */
#define MAX_PROBES 10

/* Bucket States: Empty, Occupied, Tombstone */
#define EMPTY 0
#define OCCPD 1
#define TMBSTN 2

typedef struct sftbl_bckt ftbucket;

/* Hash table bucket: Key, value, distance from 'ideal' position,
 * and data field indicating the bucket state */
struct sftbl_bckt {
    uin key;
    uin val;
    uint8_t dist;
    uint8_t data;
};

typedef struct sftbl ftable;

struct sftbl {
    ftbucket* buckets;
    uin size;
    uin count;
    uint8_t lvl;
};

ftable* alloc_ftable() {
    ftable* out = malloc(sizeof(ftable));
    memset(out, 0, sizeof(ftable));
    return out;
}

ftable* insert(ftable* ft, uin key, uin val);

void free_table(ftable* ft);

ftable* resize(ftable* ft) {
    ftable* nt = malloc(sizeof(ftable));

    /* Increase the index in the prime table used for the size */
    nt->lvl = ft->lvl + 1;
    nt->size = tableSizes[nt->lvl];;
    nt->count = 0;

    // printf("%f Occupied, resizing from %lu to %lu.\n", (((float) ft->count) / ((float) ft->size)), ft->size, nt->size);

    nt->buckets = malloc(sizeof(ftbucket) * nt->size);

    memset(nt->buckets, 0, sizeof(ftbucket) * nt->size);

    /* Iterate through every valid entry and insert into new table */
    for (uin i = 0; i < ft->size; i++) {
        if (ft->buckets[i].data == OCCPD) {
            nt = insert(nt, ft->buckets[i].key, ft->buckets[i].val);
        }
    }

    /* Free old table and return new one */

    free_table(ft);

    return nt;
}

uint64_t probelen = 0;
uint64_t probecount = 0;

ftable* insert(ftable* ft, uin key, uin val) {
    if (((float) ft->count + 1) / ((float) ft->size) > MAX_LOAD) {
        ft = resize(ft);
    }

    binsert:;
    /* Prime modulus */
    uin index = key % ft->size;
    uint8_t dist = 0;

    uin tkey;
    uin tval;
    uint8_t tdist;

    ftbucket* tem;
    uin nind;

    while (1) {
        /* If more than MAX_PROBES away from ideal location
         * resize table and attempt to insert again (goto binsert) */
        if (dist > MAX_PROBES) {
            ft = resize(ft);
            // printf("Went over MAX_PROBES\n");
            goto binsert;
        }
        // uin nind = (index + dist) % ft->size;
        nind = (index + dist) & mask[ft->lvl];
        tem = ft->buckets + nind;
        if (tem->data == OCCPD) {
            if (tem->dist < dist) {
                /* Robin hood hashing: If a 'richer' node is found,
                 * steal from it: swap */
                tkey = tem->key;
                tval = tem->val;
                tdist = tem->dist;
                tem->key = key;
                tem->val = val;
                tem->dist = dist;
                key = tkey;
                val = tval;
                dist = tdist;
                index = key % ft->size;
            }
        }
        if (tem->data == EMPTY || tem->data == TMBSTN) {
            /* Occupy any empty or tombstone buckets */
            tem->data = OCCPD;
            tem->key = key;
            tem->val = val;
            tem->dist = dist;
            ft->count++;
            //probelen += dist;
            //probecount++;
            return ft;
        }

        ++dist;
    }
}

void delete(ftable* ft, uin key) {
    uin index = key % ft->size;
    uint8_t dist = 0;
    ftbucket* tem;
    uin nind;
    while (1) {
        if (dist > MAX_PROBES) {
            /* Object not present in table. Return. */
            return;
        }
        // uin nind = (index + dist) % ft->size;
        nind = (index + dist) & mask[ft->lvl];
        tem = ft->buckets + nind;
        if (tem->data == OCCPD) {
            if (tem->key == key) {
                /* Set bucket data to tombstone and
                 * clear key and value */
                tem->data = TMBSTN;
                tem->key = 0;
                tem->val = 0;
                ft->count--;
                return;
            }
        }

        ++dist;
    }
}

uin get(ftable* ft, uin key) {
    uin index = key % ft->size;
    uint8_t dist = 0;
    ftbucket* currBkt;
    uin nind;
    do {
        nind = (index + dist) & mask[ft->lvl];
        currBkt = ft->buckets + nind;
        if (currBkt->data == OCCPD) {
            if (currBkt->key == key) {
                return currBkt->val;
            }
        } else if (currBkt->data == EMPTY) {
            // * If empty, return early. Further elements
            // * would have been bridged by a tombstone or a
            // * occupied bucket.
            return -1;
        }
    } while (dist++ < MAX_PROBES);

    perror("Went over max probes!");
    return -1;

    /*
    while (1) {
        if (dist > MAX_PROBES) {
            // * Object not present in table. Return.
            perror("Went over max probes!");
            return -1;
        }
        // uin nind = (index + dist) % ft->size;
        nind = (index + dist) & mask[ft->lvl];
        currBkt = ft->buckets + nind;
        if (currBkt->data == OCCPD) {
            if (currBkt->key == key) {
                return currBkt->val;
            }
        } else if (currBkt->data == EMPTY) {
            // * If empty, return early. Further elements
             // * would have been bridged by a tombstone or a
             // * occupied bucket.
            return -1;
        }

        ++dist;
    }
    */
}

void free_table(ftable* ft) {
    free(ft->buckets);
    free(ft);
}

#endif
