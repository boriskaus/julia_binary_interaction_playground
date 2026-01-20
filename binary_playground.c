#/*******************************************************************************
#  binary_playground.c
#
#  Small demonstration of scalar, vector and struct summation functions.
#
#  Provides three functions (declared in `binary_playground.h`):
#    - `sum_scalars(float a, float b, float c)` returns a+b+c
#    - `sum_vectors(float *v1, const float *v2, const float *v3, size_t len)`
#         computes element-wise v1[i] = v1[i] + v2[i] + v3[i]
#    - `sum_structs(const MyStruct *s1, const MyStruct *s2)`
#         returns a `MyStruct` with fields summed component-wise
#
#  The `main` implements a small CLI to exercise these functions. Example
#  usages:
#
#    Build binary:
#      make
#
#    Run scalar mode (specify three floats):
#      ./binary_playground --mode scalar --a 1.2 --b 3.4 --c 5.6
#
#    Run vector example (uses built-in example vectors):
#      ./binary_playground --mode vector
#
#    Run struct example (uses two example structs):
#      ./binary_playground --mode struct
#
#    Build a dynamic library and run the program loading it at runtime:
#      make lib
#      ./binary_playground --use-lib --mode scalar --a 1 --b 2 --c 3
#
#  The program will attempt to `dlopen` a local library named
#  `libbinary_playground.dylib` (macOS) or `libbinary_playground.so` (Linux)
#  when `--use-lib` is passed.
#
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <dlfcn.h>
#include "binary_playground.h"

/*
 * sum_scalars
 * ---------------
 * Compute the sum of three scalar floats and return the result.
 */
float sum_scalars(float a, float b, float c) {
    return a + b + c;
}

/*
 * sum_vectors
 * ---------------
 * In-place element-wise sum of three vectors. The result is written into
 * the first vector (`v1`). All arrays are assumed to have at least `len`
 * elements.
 */
void sum_vectors(float *v1, const float *v2, const float *v3, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        v1[i] = v1[i] + v2[i] + v3[i];
    }
}

/*
 * sum_structs
 * ---------------
 * Add corresponding numeric fields of two `MyStruct` instances and return
 * a new `MyStruct` with the aggregated values.
 */
MyStruct sum_structs(const MyStruct *s1, const MyStruct *s2) {
    MyStruct out;
    out.x = s1->x + s2->x;
    out.y = s1->y + s2->y;
    out.z = s1->z + s2->z;
    return out;
}

/*
 * print_usage
 * ---------------
 * Print a short summary of command-line options.
 */
static void print_usage(const char *prog) {
    printf("Usage: %s [--mode scalar|vector|struct] [--use-lib] [--a <float> --b <float> --c <float>]\n", prog);
}

int main(int argc, char **argv) {
    /* Default configuration */
    const char *mode = "scalar"; /* mode can be: scalar, vector, struct */
    int use_lib = 0;              /* when true, attempt to load functions from a shared library */
    /* Default scalar values now set to 1,2,3 as requested */
    float a = 1.0f, b = 2.0f, c = 3.0f; /* scalar inputs for scalar mode */

    /* Command-line options parsed with getopt_long */
    static struct option long_options[] = {
        {"mode", required_argument, 0, 'm'},
        {"use-lib", no_argument, 0, 'u'},
        {"a", required_argument, 0, 'a'},
        {"b", required_argument, 0, 'b'},
        {"c", required_argument, 0, 'c'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "m:ua:b:c:h", long_options, NULL)) != -1) {
        switch (opt) {
            case 'm': mode = optarg; break;
            case 'u': use_lib = 1; break;
            case 'a': a = strtof(optarg, NULL); break;
            case 'b': b = strtof(optarg, NULL); break;
            case 'c': c = strtof(optarg, NULL); break;
            case 'h': print_usage(argv[0]); return 0;
            default: print_usage(argv[0]); return 1;
        }
    }

    /* If requested, attempt to load the functions from a dynamic library */
    void *handle = NULL;
    float (*lib_sum_scalars)(float, float, float) = NULL;
    void (*lib_sum_vectors)(float *, const float *, const float *, size_t) = NULL;
    MyStruct (*lib_sum_structs)(const MyStruct *, const MyStruct *) = NULL;

    if (use_lib) {
        /* Try macOS .dylib first, then fallback to .so */
        const char *libname = "./libbinary_playground.dylib";
        handle = dlopen(libname, RTLD_LAZY);
        if (!handle) {
            libname = "./libbinary_playground.so";
            handle = dlopen(libname, RTLD_LAZY);
        }
        if (!handle) {
            fprintf(stderr, "Failed to open dynamic library: %s\n", dlerror());
            return 1;
        }

        /* Resolve symbols; if they are not found, the pointers remain NULL */
        lib_sum_scalars = (float (*)(float,float,float))dlsym(handle, "sum_scalars");
        lib_sum_vectors = (void (*)(float*,const float*,const float*,size_t))dlsym(handle, "sum_vectors");
        lib_sum_structs = (MyStruct (*)(const MyStruct*,const MyStruct*))dlsym(handle, "sum_structs");
    }

    /* Dispatch by mode */
    if (strcmp(mode, "scalar") == 0) {
        /* Use library function if available and requested, otherwise local */
        float res;
        if (use_lib && lib_sum_scalars) {
            res = lib_sum_scalars(a, b, c);
        } else {
            res = sum_scalars(a, b, c);
        }
        printf("sum_scalars(%f, %f, %f) = %f\n", a, b, c, res);

    } else if (strcmp(mode, "vector") == 0) {
        /* Example fixed-length vectors for demonstration */
        size_t len = 3;
        float v1[3] = {1.0f, 2.0f, 3.0f};
        float v2[3] = {0.5f, 0.5f, 0.5f};
        float v3[3] = {0.1f, 0.2f, 0.3f};

        if (use_lib && lib_sum_vectors) {
            lib_sum_vectors(v1, v2, v3, len);
        } else {
            sum_vectors(v1, v2, v3, len);
        }

        printf("sum_vectors result: [");
        for (size_t i = 0; i < len; ++i) {
            printf("%f%s", v1[i], (i+1 < len) ? ", " : "");
        }
        printf("]\n");

    } else if (strcmp(mode, "struct") == 0) {
        /* Example structs for demonstration */
        MyStruct s1 = { .x = 1, .y = 2.5f, .z = 3.25 };
        MyStruct s2 = { .x = 4, .y = 1.5f, .z = 0.75 };
        MyStruct out;

        if (use_lib && lib_sum_structs) {
            out = lib_sum_structs(&s1, &s2);
        } else {
            out = sum_structs(&s1, &s2);
        }

        printf("sum_structs: x=%d, y=%f, z=%f\n", out.x, out.y, out.z);

    } else {
        fprintf(stderr, "Unknown mode '%s'\n", mode);
        print_usage(argv[0]);
        if (handle) dlclose(handle);
        return 1;
    }

    if (handle) dlclose(handle);
    return 0;
}
