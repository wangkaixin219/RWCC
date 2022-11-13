//
// Created by Wang Kaixin on 2022/6/8.
//

#ifndef DESCOL_DEFS_H
#define DESCOL_DEFS_H

#include <cstdio>
#include <cstdlib>
#include <sys/resource.h>

#define N_EDGES 200000000
#define N_NODES 10000000

inline unsigned max3(unsigned a, unsigned b, unsigned c) {
    a = (a > b) ? a : b;
    return (a > c) ? a : c;
}

inline unsigned max2(unsigned a, unsigned b) {
   return (a > b) ? a : b;
}

inline void GetCurTime(struct rusage* curTime) {
    if (getrusage(RUSAGE_THREAD, curTime) != 0) {
        fprintf(stderr, "The running time info couldn't be collected successfully.\n");
        exit(0);
    }
}

inline double GetTime(struct rusage* start, struct rusage* end) {
    return ((float)(end->ru_utime.tv_sec - start->ru_utime.tv_sec)) * 1e3 +
    ((float)(end->ru_utime.tv_usec - start->ru_utime.tv_usec)) * 1e-3;
}   // unit: ms

#endif //DESCOL_DEFS_H
