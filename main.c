#include <stdio.h>
#include <sys/time.h>
#include <immintrin.h>
#include "ftable.h"

//#include "consistent.h"

#define COUNT 8000000


uint64_t mtime() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000LU + tv.tv_usec;
}

/* TODO: SIMD Modulus, as described in:
 * http://homepage.divms.uiowa.edu/~jones/bcd/mod.shtml#SIMDexmod3 */
int main() {
    ftable* ft = alloc_ftable();

    uint64_t start = mtime();
    for (uint64_t i = COUNT; i > 0; i--) {
        ft = insert(ft, i, i);
    }
    uint64_t end = mtime();

    uint64_t elapsed = end - start;
    printf("Inserts: %f per ms, %lu ms for %i\n", (((float) COUNT) / ((float) (elapsed / 1000))), elapsed / 1000, COUNT);
    printf("%f ns per insert.\n", (((float) (end - start) * 1000) / ((float) COUNT)));


    uint64_t t;
    start = mtime();
    for (uint64_t i = COUNT; i > 0; i--) {
        t = get(ft, i);
    }
    end = mtime();

    elapsed = end - start;
    printf("Reads: %f per ms, %lu ms for %i\n", (((float) COUNT) / ((float) (elapsed / 1000))), elapsed / 1000, COUNT);
    printf("%f ns per read.\n", (((float) (end - start) * 1000) / ((float) COUNT)));

    printf("Memory use: %lu bytes.\n", (sizeof(ftbucket) * ft->size + sizeof(ftable)));

    printf("%d %d\n", t, ft->lvl);

    // printf("%f %lu %lu\n", (((float) probelen) / ((float) probecount)), probelen, probecount);

    free_table(ft);

}
