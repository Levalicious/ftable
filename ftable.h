#ifndef FTABLE_FTABLE_H
#define FTABLE_FTABLE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LOAD 0.5

#ifdef __x86
typedef uint32_t uin;

const uin tableSizes[] = {
        5, 11, 23, 47, 97, 197, 397, 797, 1597,
        3203, 6421, 12853, 25717, 51437, 102877,
        205759, 411527, 823117, 1646237, 3292489,
        6584983, 13169977, 26339969, 52679969,
        105359939, 210719881, 421439783, 842879579,
        1685759167, 3371518343 };

#elif __x86_64
typedef uint64_t uin;

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

#define MAX_PROBES 10

#define EMPTY 0
#define OCCPD 1
#define TMBSTN 2

typedef struct sftbl_bckt ftbucket;

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

    nt->lvl = ft->lvl + 1;
    nt->size = tableSizes[nt->lvl];;
    nt->count = 0;

    nt->buckets = malloc(sizeof(ftbucket) * nt->size);

    memset(nt->buckets, 0, sizeof(ftbucket) * nt->size);

    for (uin i = 0; i < ft->size; i++) {
        if (ft->buckets[i].data == OCCPD) {
            nt = insert(nt, ft->buckets[i].key, ft->buckets[i].val);
        }
    }

    free_table(ft);

    return nt;
}

ftable* insert(ftable* ft, uin key, uin val) {
    if (((float) ft->count + 1) / ((float) ft->size) > MAX_LOAD) {
        ft = resize(ft);
    }

    binsert:;
    uin index = key % ft->size;
    uint8_t dist = 0;
    while (1) {
        if (dist > MAX_PROBES) {
            ft = resize(ft);
            goto binsert;
        }
        uin nind = (index + dist) % ft->size;
        if (ft->buckets[nind].data == OCCPD) {
            if (ft->buckets[nind].dist < dist) {
                uin tkey = ft->buckets[nind].key;
                uin tval = ft->buckets[nind].val;
                uint8_t tdist = ft->buckets[nind].dist;
                ft->buckets[nind].key = key;
                ft->buckets[nind].val = val;
                ft->buckets[nind].dist = dist;
                key = tkey;
                val = tval;
                dist = tdist;
            }
        }
        if (ft->buckets[nind].data == EMPTY || ft->buckets[index + dist].data == TMBSTN) {
            ft->buckets[nind].data = OCCPD;
            ft->buckets[nind].key = key;
            ft->buckets[nind].val = val;
            ft->buckets[nind].dist = dist;
            ft->count++;
            return ft;
        }

        dist++;
    }
}

void delete(ftable* ft, uin key) {
    uin index = key % ft->size;
    uint8_t dist = 0;
    while (1) {
        if (dist > MAX_PROBES) {
            /* Object not present in table. Return. */
            return;
        }
        uin nind = (index + dist) % ft->size;
        if (ft->buckets[nind].data == OCCPD) {
            if (ft->buckets[nind].key == key) {
                ft->buckets[nind].data = TMBSTN;
                ft->buckets[nind].key = 0;
                ft->buckets[nind].val = 0;
                ft->count--;
                return;
            }
        }

        dist++;
    }
}

uin get(ftable* ft, uin key) {
    uin index = key % ft->size;
    uint8_t dist = 0;
    while (1) {
        if (dist > MAX_PROBES) {
            /* Object not present in table. Return. */
            return -1;
        }
        uin nind = (index + dist) % ft->size;
        if (ft->buckets[nind].data == OCCPD) {
            if (ft->buckets[nind].key == key) {
                return ft->buckets[nind].val;
            }
        }

        dist++;
    }
}

void free_table(ftable* ft) {
    free(ft->buckets);
    free(ft);
}

#endif
