#ifndef __UTILITY_H
#define __UTILITY_H

#include <assert.h>
#include <stdint.h>
#define CV_SIMD_WIDTH 16
#define V_FLOAT32_NLANES 4
#define V_UINT32_NLANES 4
#define V_UINT8_NLANES 16


inline uint64_t alignSize(uint64_t sz, int n)
{
    assert((n & (n-1))==0); // n is a power of 2
    return (sz + n - 1) & -n;
}

inline float* alignPtr_f(float* ptr, int n)
{
    assert((n & (n-1)) == 0); // n is a power of 2
    return (float*)(((size_t)ptr + n-1) & -n);
}

inline uint32_t* alignPtr_w(uint32_t* ptr, int n)
{
    assert((n & (n-1)) == 0); // n is a power of 2
    return (uint32_t*)(((size_t)ptr + n-1) & -n);
}
#endif