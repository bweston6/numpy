/* powersort.cpp - Powersort implementation
 *
 * TODO - change funtion references to be powersort instead of timsort
 *
 * See https://www.wild-inter.net/posts/powersort-in-python-3.11 for more
 * information on the method.
 *
 * Powersort is a merge sort derivative that is p_rightovably near-optimal with
 * minimal overhead.
 */

#define NPY_NO_DEPRECATED_API NPY_API_VERSION

#include "npy_sort.h"
#include "npysort_common.h"
#include "numpy_tag.h"

#include <cstdlib>
#include <utility>

/*
 *****************************************************************************
 **                            COMMON STRUCTS                               **
 *****************************************************************************
 */

/*
 *****************************************************************************
 **                     GENERIC SORT IMPLEMENTATION                         **
 *****************************************************************************
 */

static char *
npy_extend_run(char *p_run_start, char *p_right, char *p_working_array,
               char *p_working_value, npy_intp elsize,
               PyArray_CompareFunc *cmp, PyArrayObject *array)
{
    // extend run in a list
    char *p_run_end = p_run_start + elsize;
    return p_run_end;
}

static void
npy_merge()
{
}

static void
npy_merge_top_two()
{
}

static void
npy_power()
{
}

static void
npy_powersort(char *p_left, char *p_right, char *p_working_array,
              char *p_working_value, npy_intp elsize, PyArray_CompareFunc *cmp,
              PyArrayObject *array)
{
    return;
}

/*
 *****************************************************************************
 **                          GENERIC SORT ENTRY                             **
 *****************************************************************************
 */

/* Returns 0 for success and -1 for error */
NPY_NO_EXPORT int
npy_timsort(void *start, npy_intp num, void *void_array)
{
    PyArrayObject *array = (PyArrayObject *)void_array;
    npy_intp elsize = PyArray_ITEMSIZE(array);
    PyArray_CompareFunc *cmp = PyArray_DESCR(array)->f->compare;
    char *p_left = (char *)start;
    char *p_right = p_left + num * elsize;
    char *p_working_array;
    char *p_working_value;
    int err = -NPY_ENOMEM;

    /* Items that have zero size don't make sense to sort */
    if (elsize == 0) {
        return 0;
    }

    p_working_array = (char *)malloc((num >> 1) * elsize);
    p_working_value = (char *)malloc(elsize);
    if (p_working_array != NULL && p_working_value != NULL) {
        npy_powersort(p_left, p_right, p_working_array, p_working_value,
                      elsize, cmp, array);
        err = 0;
    }

    free(p_working_value);
    free(p_working_array);

    return 0;
}

/* argsort - sort the subset of the array defined in tosort */
NPY_NO_EXPORT int
npy_atimsort(void *start, npy_intp *tosort, npy_intp num, void *void_array)
{
    return 0;
}
