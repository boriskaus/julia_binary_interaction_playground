#ifndef BINARY_PLAYGROUND_H
#define BINARY_PLAYGROUND_H

#include <stddef.h>

typedef struct {
    int x;
    float y;
    double z;
} MyStruct;

/* Sum three scalar floats and return the result */
float sum_scalars(float a, float b, float c);

/* In-place sum of three vectors: result stored into v1. `len` is the length. */
void sum_vectors(float *v1, const float *v2, const float *v3, size_t len);

/* Sum fields of two structs and return the resulting struct */
MyStruct sum_structs(const MyStruct *s1, const MyStruct *s2);

#endif /* BINARY_PLAYGROUND_H */
