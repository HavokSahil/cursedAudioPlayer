/**
 * MIT License
 *
 * Copyright (c) 2025 Sahil
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file sparsematrix.h
 * @brief Sparse matrix module with configurable precision and vector
 * multiplication support.
 *  - This module provides a basic sparse matrix representation using coordinate
 * format (COO), along with vector multiplication (left and right), transpose,
 * and debugging support.
 */

#ifndef SPARSEMATRIX_H_
#define SPARSEMATRIX_H_

/**
 * @def Type
 * @brief Define the precision type used in the sparse matrix and vector
 * entries. Default is double. You can override it before including this header.
 */
#ifndef SMType
#define SMType double
#define SMProd(a, b) (a * b)
#define SMSum(a, b) (a + b)
#endif

/**
 * @def SMProd
 * @brief There should be a user defined product function for the type.
 * @usage Example:
 * Type product(Type a, Type b) { return a * b; }
 * #define SMProd(a, b) product(a, b);
 */
#ifndef SMProd
#error "product not defined."
#endif

/**
 * @def SMSum
 * @brief There should be a user defiend sum function for the type.
 * @usage Example:
 * Type sum(Type a, Type b) { return a + b; }
 * #define SMSum(a, b) sum(a, b);
 */
#ifndef SMSum
#error "sum not defined."
#endif

/**
 * @def SM_PRINT_FMT
 * @brief Print format for matrix entries in debug mode.
 * Adjusts automatically depending on the `Type` macro.
 */
#ifndef SM_PRINT_FMT
#define SM_PRINT_FMT "%.3f"
#endif // SM_PRINT_FMT

// Indices use int; larger FFTs must not silently truncate rows or kernel entries.
#ifndef SM_MAX_NZERO
#define SM_MAX_NZERO 4194304
#endif
#ifndef SM_MAX_ROWS
#define SM_MAX_ROWS 1048576
#endif
#ifndef SM_MAX_COLS
#define SM_MAX_COLS 32767
#endif

// Utility macros
#if !defined(MIN)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

#if !defined(MAX)
#define MAX(a, b) (((a) < (b)) ? (b) : (a))
#endif

#include <stdlib.h>
#include <string.h>

#ifdef SM_DEBUG
#include <assert.h>
#include <stdio.h>
#endif // SM_DEBUG

/**
 * @struct Vec
 * @brief Represents a vector with configurable precision.
 */
typedef struct Vec {
    SMType *entries; /**< Pointer to vector elements */
    int dim;         /**< Dimension of the vector */
} Vec;

/**
 * @struct SparseMatrix
 * @brief Represents a sparse matrix in coordinate format (COO).
 */
typedef struct SparseMatrix {
    SMType *nzentries; /**< Non-zero entries */
    int *prows;        /**< Row indices corresponding to each entry */
    int *pcols;        /**< Column indices corresponding to each entry */
    int nnz;           /**< Current number of non-zero entries */
    int capacity;      /**< Allocated entries, grown while constructing kernels */
    int nrows;         /**< Number of matrix rows */
    int ncols;         /**< Number of matrix columns */
} SparseMatrix;

/**
 * @brief Allocate and initialize a vector of given dimension.
 * @param dim Dimension of the vector
 * @return Pointer to the initialized Vec or NULL on failure
 */
Vec *vec_init(int dim);

/**
 * @brief Allocate and initialize a vector of given dimension and allocated
 * memory safely (turns the initial pointer to NULL, taking the ownership)
 * @param dim Dimension of the vector
 * @param pmem Pointer to the allocated memory
 * @return Pointer to the initialized Vec or NULL on failure
 */
Vec *vec_init_mem(int dim, SMType **pmem);

/**
 * @brief Deallocate a vector and its memory.
 * @param pv Pointer to the vector to be deallocated
 */
void vec_deinit(Vec *pv);

/**
 * @brief Initialize a sparse matrix with given dimensions.
 * @param nrows Number of rows
 * @param ncols Number of columns
 * @return Pointer to the initialized SparseMatrix or NULL on failure
 */
SparseMatrix *sparse_matrix_init(int nrows, int ncols);

/**
 * @brief Deallocate all memory associated with the sparse matrix.
 * @param psm Pointer to the sparse matrix to be deallocated
 */
void sparse_matrix_deinit(SparseMatrix *psm);

/**
 * @brief Get the queried entry at a row and column value in Sparse Matrix.
 * @param psm Pointer to the Sparse Matrix
 * @param irow Index of the row
 * @param icol Index of the col
 * @param entry Pointer to the memory where result is expected
 * @return 0 on success, -1 on error
 */
int sparse_matrix_get_entry(SparseMatrix *psm, int irow, int icol,
                            SMType *entry);

/**
 * @brief Transpose the given sparse matrix in-place.
 * Swaps row and column indices for each non-zero entry.
 * @param psm Pointer to the sparse matrix
 */
void sparse_matrix_transpose(SparseMatrix *psm);

/**
 * @brief Add a non-zero entry to the sparse matrix.
 * @param psm Pointer to the sparse matrix
 * @param entry Non-zero value
 * @param irow Row index
 * @param icol Column index
 * @return 0 on success, -1 on failure (invalid input or matrix full)
 */
int sparse_matrix_add_entry(SparseMatrix *psm, SMType entry, int irow,
                            int icol);

/**
 * @brief Multiply a vector from the left: result = vecᵗ × matrix.
 * @param pv Input vector (dimension = matrix rows)
 * @param psm Sparse matrix
 * @param res Result vector (dimension = matrix cols)
 * @return 0 on success, -1 on error (e.g., dimension mismatch)
 */
int sparse_matrix_mult_vec_left(Vec *pv, SparseMatrix *psm, Vec *res);

/**
 * @brief Multiply a array from the left: result = memᵗ × matrix.
 * @param pmem Input memory array (dimension = matrix rows)
 * @param size Dimension of the input array
 * @param psm Sparse matrix
 * @param res Result vector (dimension = matrix cols)
 * @return 0 on success, -1 on error (e.g., dimension mismatch)
 */
int sparse_matrix_mult_mem_left(SMType *pmem, int size, SparseMatrix *psm,
                                Vec *res);

/**
 * @brief Multiply a vector from the right: result = matrix × vec.
 * @param psm Sparse matrix
 * @param pv Input vector (dimension = matrix cols)
 * @param res Result vector (dimension = matrix rows)
 * @return 0 on success, -1 on error (e.g., dimension mismatch)
 */
int sparse_matrix_mult_vec_right(SparseMatrix *psm, Vec *pv, Vec *res);

/**
 * @brief Multiply a array from the right: result = matrix × vec.
 * @param psm Sparse matrix
 * @param pmem Input memory array (dimension = matrix cols)
 * @param size Dimension of the input array
 * @param res Result vector (dimension = matrix rows)
 * @return 0 on success, -1 on error (e.g., dimension mismatch)
 */
int sparse_matrix_mult_mem_right(SparseMatrix *psm, SMType *pmem, int size,
                                 Vec *res);

/*
** Implementations
*/
Vec *vec_init(int dim) {
    if (dim <= 0) return NULL;
    Vec *pv = (Vec *)malloc(sizeof(Vec));
    if (pv == NULL) {
#if defined(SM_DEBUG)
        printf("Memory allocation failed for Vec in vec_init.\n");
#endif
        return NULL;
    }
    pv->dim = dim;
    pv->entries = (SMType *)malloc(sizeof(SMType) * dim);
    if (pv->entries == NULL) {
        free(pv);
#if defined(SM_DEBUG)
        printf("Memory allocation failed for entries in vec_init.\n");
#endif
        return NULL;
    }
    // initialize the zero vector
    memset(pv->entries, 0, sizeof(SMType) * dim);
    return pv;
}

Vec *vec_init_mem(int dim, SMType **pmem) {
    if (pmem == NULL || *pmem == NULL) {
#if defined(SM_DEBUG)
        printf("invalid pointer provided in vec_init_mem.\n");
#endif
        return NULL;
    }
    Vec *pv = (Vec *)malloc(sizeof(Vec));
    if (pv == NULL) return NULL;
    pv->dim = dim;
    pv->entries = *pmem;
    // zero out the memory region
    memset(*pmem, 0, sizeof(SMType) * dim);
    *pmem = NULL; // take the ownership
    return pv;
}

void vec_deinit(Vec *pv) {
    if (pv) {
        if (pv->entries)
            free(pv->entries);
        free(pv);
    }
}

SparseMatrix *sparse_matrix_init(int nrows, int ncols) {
    if (nrows <= 0 || ncols <= 0 || nrows > SM_MAX_ROWS || ncols > SM_MAX_COLS)
        return NULL;
    SparseMatrix *psm = (SparseMatrix *)malloc(sizeof(SparseMatrix));
    if (psm == NULL) {
#if defined(SM_DEBUG)
        printf("Memory allocation failure for SparseMatrix in "
               "sparse_matrix_init.\n");
#endif
        return NULL;
    }
    psm->nrows = MIN(nrows, SM_MAX_ROWS);
    psm->ncols = MIN(ncols, SM_MAX_COLS);

    // initialize the pointers with null value
    psm->prows = NULL;
    psm->pcols = NULL;
    psm->nzentries = NULL;

    psm->nnz = 0; // no entries present initially;
    int mxnnz = MIN((size_t)nrows * ncols, SM_MAX_NZERO);
    psm->capacity = MIN(mxnnz, 256);
    psm->nzentries = (SMType *)malloc(sizeof(SMType) * psm->capacity);
    if (psm->nzentries == NULL) {
        sparse_matrix_deinit(psm);
#if defined(SM_DEBUG)
        printf("Memory allocation failed for non-zero entries in "
               "sparse_matrix_init.\n");
#endif
        return NULL;
    }
    psm->prows = (int *)malloc(sizeof(int) * psm->capacity);
    if (psm->prows == NULL) {
        sparse_matrix_deinit(psm);
#if defined(SM_DEBUG)
        printf("Memory allocation failed for rows in sparse_matrix_init.\n");
#endif
        return NULL;
    }
    psm->pcols = (int *)malloc(sizeof(int) * psm->capacity);
    if (psm->pcols == NULL) {
        sparse_matrix_deinit(psm);
#if defined(SM_DEBUG)
        printf("Memory allocation failed for columns in sparse_matrix_init.\n");
#endif
        return NULL;
    }

    return psm;
}

void sparse_matrix_deinit(SparseMatrix *psm) {
    if (psm != NULL) {
        if (psm->nzentries != NULL)
            free(psm->nzentries);
        if (psm->prows != NULL)
            free(psm->prows);
        if (psm->pcols != NULL)
            free(psm->pcols);
        free(psm);
        psm = NULL;
    }
}

int sparse_matrix_get_entry(SparseMatrix *psm, int irow, int icol,
                            SMType *entry) {
    if (psm == NULL) {
#if defined(SM_DEBUG)
        printf("Null pointer provided in sparse_matrix_get_entry.\n");
#endif
        return -1; // invalid pointer
    }

    if (psm->nrows > irow && irow >= 0 && psm->ncols > icol && icol >= 0) {
        memset(entry, 0, sizeof(SMType)); // start with zero value
        for (int i = 0; i < psm->nnz; i++) {
            int row = psm->prows[i];
            int col = psm->pcols[i];
            if (row == irow && col == icol) {
                *entry = psm->nzentries[i];
            }
        }
        return 0; // success
    } else {
#if defined(SM_DEBUG)
        printf("The queried index for row or column is out of range.\n");
#endif
        return -1; // out of bound query
    }
}

void sparse_matrix_transpose(SparseMatrix *psm) {
    if (psm && psm->nzentries && psm->prows && psm->pcols) {
        for (int i = 0; i < psm->nnz; i++) {
            int *row = psm->prows + i;
            int *col = psm->pcols + i;
            // swap the rows and columns
            int tmp = *row;
            *row = *col;
            *col = tmp;
        }
    } else {
#if defined(SM_DEBUG)
        printf("Invalid pointers provided to sparse_matrix_transpose.\n");
#endif
    }
}

int sparse_matrix_add_entry(SparseMatrix *psm, SMType entry, int irow,
                            int icol) {
    if (psm && psm->nzentries && psm->prows && psm->pcols) {
        if (irow >= 0 && irow < psm->nrows && icol >= 0 && icol < psm->ncols) {
            // For valid row and column index
            int mxnnz = MIN((size_t)psm->nrows * psm->ncols, SM_MAX_NZERO);
            if (psm->nnz < mxnnz) {
                if (psm->nnz == psm->capacity) {
                    int capacity = MIN(psm->capacity * 2, mxnnz);
                    SMType *entries = realloc(psm->nzentries, sizeof(SMType) * capacity);
                    if (!entries) return -1;
                    psm->nzentries = entries;
                    int *rows = realloc(psm->prows, sizeof(int) * capacity);
                    if (!rows) return -1;
                    psm->prows = rows;
                    int *cols = realloc(psm->pcols, sizeof(int) * capacity);
                    if (!cols) return -1;
                    psm->pcols = cols;
                    psm->capacity = capacity;
                }
                psm->nzentries[psm->nnz] = entry;
                psm->prows[psm->nnz] = irow;
                psm->pcols[psm->nnz] = icol;
                psm->nnz++;
                return 0;
            } else {
#if defined(SM_DEBUG)
                printf("The maximum non zero entries limit exceeded.\n");
#endif
                return -1;
            }
        } else {
#if defined(SM_DEBUG)
            printf("The index row or column is out of limits.\n");
#endif
            return -1;
        }
    }
#if defined(SM_DEBUG)
    printf("Invalid pointer provided in sparse_matrix_add_entry.\n");
#endif
    return -1;
}

int sparse_matrix_mult_vec_left(Vec *pv, SparseMatrix *psm, Vec *res) {
    if (pv && pv->entries && res && res->entries && psm && psm->prows &&
        psm->pcols && psm->nzentries) {
        if (pv->dim != psm->nrows) {
#if defined(SM_DEBUG)
            printf("pv->dim(%d) != psm->nrows(%d): dimension mismatch.\n",
                   pv->dim, psm->nrows);
#endif
            return -1; // dimension mismatch for input
        }
        if (res->dim != psm->ncols) {
#if defined(SM_DEBUG)
            printf("res->dim(%d) != psm->ncols(%d): dimension mismatch.\n",
                   res->dim, psm->ncols);
#endif
            return -1; // dimension mismatch for output
        }
        memset(res->entries, 0, res->dim * sizeof(SMType));
        for (int i = 0; i < psm->nnz; i++) {
            int irow = psm->prows[i];
            int icol = psm->pcols[i];
            res->entries[icol] =
                SMSum(SMProd(psm->nzentries[i], pv->entries[irow]),
                      res->entries[icol]);
        }
        return 0; // success
    }
#if defined(SM_DEBUG)
    printf("invalid pointer provided in sparse_matrix_mult_vec_left.\n");
#endif
    return -1; // invalid pointer
}

int sparse_matrix_mult_mem_left(SMType *pmem, int size, SparseMatrix *psm,
                                Vec *res) {
    if (pmem && psm && psm->prows && psm->pcols && psm->nzentries && res &&
        res->entries) {
        if (size != psm->nrows) {
#if defined(SM_DEBUG)
            printf("size(%d) != psm->nrows(%d): dimension mismatch.\n", size,
                   psm->ncols);
#endif
            return -1; // dimension mismatch for input
        }
        if (res->dim != psm->ncols) {
#if defined(SM_DEBUG)
            printf("res->dim(%d) != psm->ncols(%d): dimension mismatch.\n",
                   res->dim, psm->ncols);
#endif
            return -1; // dimemsion mismatch for output
        }
        memset(res->entries, 0, res->dim * sizeof(SMType));
        for (int i = 0; i < psm->nnz; i++) {
            int irow = psm->prows[i];
            int icol = psm->pcols[i];
            res->entries[icol] = SMSum(SMProd(psm->nzentries[i], pmem[irow]),
                                       res->entries[icol]);
        }
        return 0; // success
    }
#if defined(SM_DEBUG)
    printf("invalid pointer provided in sparse_matrix_mult_mem_left.\n");
#endif
    return -1; // invalid pointer
}

int sparse_matrix_mult_vec_right(SparseMatrix *psm, Vec *pv, Vec *res) {
    if (pv && pv->entries && res && res->entries && psm && psm->prows &&
        psm->pcols && psm->nzentries) {
        if (pv->dim != psm->ncols) {
#if defined(SM_DEBUG)
            printf("pv->dim(%d) != psm->ncols(%d): dimension mismatch.\n",
                   pv->dim, psm->ncols);
#endif
            return -1; // dimension mismatch for input
        }
        if (res->dim != psm->nrows) {
#if defined(SM_DEBUG)
            printf("res->dim(%d) != psm->nrows(%d): dimension mismatch.\n",
                   res->dim, psm->nrows);
#endif
            return -1; // dimension mismatch for output
        }
        memset(res->entries, 0, res->dim * sizeof(SMType));
        for (int i = 0; i < psm->nnz; i++) {
            int irow = psm->prows[i];
            int icol = psm->pcols[i];
            res->entries[irow] =
                SMSum(SMProd(psm->nzentries[i], pv->entries[icol]),
                      res->entries[irow]);
        }
        return 0; // success
    }
#if defined(SM_DEBUG)
    printf("invalid pointer provided in sparse_matrix_mult_vec_right.\n");
#endif
    return -1; // invalid pointer
}

int sparse_matrix_mult_mem_right(SparseMatrix *psm, SMType *pmem, int size,
                                 Vec *res) {
    if (pmem && res && res->entries && psm && psm->prows && psm->pcols &&
        psm->nzentries) {
        if (size != psm->ncols) {
#if defined(SM_DEBUG)
            printf("size(%d) != psm->ncols(%d): dimension mismatch.\n", size,
                   psm->ncols);
#endif
            return -1; // dimension mismatch for input
        }
        if (res->dim != psm->nrows) {
#if defined(SM_DEBUG)
            printf("res->dim(%d) != psm->nrows(%d): dimension mismatch.\n",
                   res->dim, psm->nrows);
#endif
            return -1; // dimension mismatch for output
        }
        memset(res->entries, 0, res->dim * sizeof(SMType));
        for (int i = 0; i < psm->nnz; i++) {
            int irow = psm->prows[i];
            int icol = psm->pcols[i];
            res->entries[irow] = SMSum(SMProd(psm->nzentries[i], pmem[icol]),
                                       res->entries[irow]);
        }
        return 0; // success
    }
#if defined(SM_DEBUG)
    printf("invalid pointer provided in sparse_matrix_mult_mem_right.\n");
#endif
    return -1; // invalid pointer
}

#if defined(SM_DEBUG)
/**
 * @brief Print the vector for debugging.
 * Requires `SM_DEBUG` to be defined.
 * @param pv Pointer to the vector
 */
void vec_debug_print(Vec *pv) {
    if (pv == NULL) {
        printf("Null pointer provided.\n");
        return;
    }
    if (pv->entries == NULL) {
        printf("pv->entries is null.\n");
        return;
    }

    assert(pv->dim >= 0);
    printf("[ ");
    for (int i = 0; i < pv->dim; i++) {
#ifdef SM_DBG_PRINT_FN
        SM_DBG_PRINT_FN(pv->entries[i]);
#else
        printf(SM_PRINT_FMT " ", pv->entries[i]);
#endif
    }
    printf("]\n");
}

/**
 * @brief Print the full matrix in dense format for debugging.
 * Requires `SM_DEBUG` to be defined.
 * @param psm Pointer to the sparse matrix
 */
void sparse_matrix_debug_print(const SparseMatrix *psm) {
    if (psm == NULL) {
        printf("Sparse Matrix: Null pointer.\n");
        return;
    }
    printf("INFO: SparseMatrix\n");
    printf("nrows: %d, ncols: %d\n", psm->nrows, psm->ncols);
    printf("Non zero entries: %d\n", psm->nnz);
    if (psm->nzentries == NULL) {
        printf("psm->nzentries is null.\n");
        return;
    }
    if (psm->prows == NULL) {
        printf("psm->prows is null.\n");
        return;
    }
    if (psm->pcols == NULL) {
        printf("psm->pcols is null.\n");
        return;
    }

    assert(psm->nrows >= 0);
    assert(psm->ncols >= 0);

    int nrows = psm->nrows, ncols = psm->ncols;
    SMType matrix[nrows][ncols];

    memset(matrix, 0, sizeof(SMType) * nrows * ncols);

    for (int i = 0; i < psm->nnz; i++) {
        matrix[psm->prows[i]][psm->pcols[i]] = psm->nzentries[i];
    }

    for (int i = 0; i < nrows; i++) {
        for (int ii = 0; ii < ncols; ii++) {
#ifdef SM_DBG_PRINT_FN
            SM_DBG_PRINT_FN(matrix[i][ii]);
#else
            printf(SM_PRINT_FMT " ", matrix[i][ii]);
#endif
        }
        printf("\n");
    }
    printf("END: SparseMatrix\n");
}

void sparse_matrix_debug_print_vec(Vec *pv) {
    if (pv == NULL) {
        printf("Vec is null.\n");
        return;
    }
    printf("INFO: Vec\n");
    printf("dim: %d\n", pv->dim);
    if (pv->entries == NULL) {
        printf("pv->entries is null.\n");
        return;
    }
    printf("[ ");
    for (int i = 0; i < pv->dim; i++) {
#if defined(SM_DBG_PRINT_FN)
        SM_DBG_PRINT_FN(pv->entries[i]);
#else
        printf(SM_PRINT_FMT " ", pv->entries[i]);
#endif
    }
    printf("]\n");
    printf("END: Vec\n");
}

#endif // SM_DEBUG
#endif // SPARSEMATRIX_H_
