//------------------------------------------------------------------------------
// binsparse_matrix.h: a matrix data structure for the C binding of binsparse
//------------------------------------------------------------------------------

// draft by Tim Davis

// I propose that we create a single object, the binsparse_c_matrix, or
// bc_matrix for short, to hold any matrix for the C bindings of the binsparse
// package.

// Name space:  binsparse_c_*?  bc_*?  I'm open to suggestions.  For brevity
// I'm assuming bc_* for now.

#ifndef BINSPARSE_MATRIX_H
#define BINSPARSE_MATRIX_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

//------------------------------------------------------------------------------
// type codes
//------------------------------------------------------------------------------

// The bc_matrix can hold data types that correspond to the primary C built-in
// types, with a placeholder for future user-defined types.

typedef enum
{
    bc_type_none    = 0,        // no type; values array is NULL (maybe?)
    // unsigned types
    bc_type_bool    = 1,        // bool, assume sizeof(bool) == 1, as uint8_t
    bc_type_uint8   = 2,        // uint8_t
    bc_type_uint16  = 3,        // uint16_t
    bc_type_uint32  = 4,        // uint32_t
    bc_type_uint64  = 5,        // uint64_t
    // signed types
    bc_type_int8    = 6,        // int8_t
    bc_type_int16   = 7,        // int16_t
    bc_type_int32   = 8,        // int32_t
    bc_type_int64   = 9,        // int64_t
    bc_type_fp32    = 10,       // float
    bc_type_fp64    = 11,       // double
    bc_type_fc32    = 12,       // float complex
    bc_type_fc64    = 13,       // double complex
    // user-defined type
    bc_type_user    = 14,       // user-defined type
}
bc_type_code ;

//------------------------------------------------------------------------------
// the bc_matrix: a sparse matrix or vector of any type
//------------------------------------------------------------------------------

// Each dimension k of a given n-D matrix can be in one of four formats,
// listed in increasing order of sparsity:
//
// pointer[k]   index[k]    Name and description
// ----------   --------    --------------------
//
// NULL         non-NULL    "Index": some entries present.
//                          indices need not be in order, nor unique.
//                          size of index [k] array is nindex [k].
//                          in_order [k] can be true or false.
//
// non-NULL     non-NULL    "Hyper": some entries present.
//                          indices must be in order and unique.
//                          index [k] has size nindex [k]
//                          pointer [k] has size nindex [k]+1 and must be
//                          monotonically non-decreasing.
//                          in_order [k] must be true.
//
// non-NULL     NULL        "Sparse": all entries present.
//                          pointer [k] has size dimension [axis_order[k]]+1.
//                          nindex [k] not used (or can be set to
//                          dimension [axis_order[k]] for consistency).
//                          in_order [k] must be true.
//
// NULL         NULL        "Full": all entries present,
//                          nindex [k] not used (or can be set to
//                          dimension [axis_order[k]] for consistency).
//                          in_order [k] must be true.

// The matrix format is determined by the presence of pointer [0:rank-1]
// and index [0:rank-1] (NULL or non-NULL).  There need not be any format
// enum.

// Common formats:

// rank = 0:    a scalar, no arrays present.  nvals = 0 or 1

// rank = 1:    a 1-D vector of dimension n
//
//      axis_order = { 0 }
//      dimension  = { n }
//
//      sparse vector (COO-style):  Format is (Index)
//          pointer [0] = NULL
//          index   [0] = [list of nvals indices]
//          values  [0] = [list of nvals values], or size 1 if iso
//          in_order [0] = true if indices in ascending order, false otherwise
//
//      full vector: Format is (Full)
//          pointer [0] = NULL
//          index   [0] = NULL
//          values  [0] = size n, or size 1 if iso
//          in_order [0] = true

// rank = 2:    a 2-D matrix of dimension m-by-n
//
//      axis_order = { 0, 1 } if stored by-row
//      axis_order = { 1, 0 } if stored by-column
//      dimension  = { m, n }
//
//      COO: Format is (Index, Index)
//
//          pointer [0] = NULL
//          pointer [1] = NULL
//          index [0] = row indices if by-row, col indices if by-col, size nvals
//          index [1] = col indices if by-row, row indices if by-col, size nvals
//          values [0] = values, size nvals
//          in_order [0] = true if index [0] in ascending order, false otherwise
//          in_order [1] = true if index [1] in ascending order, false otherwise
//
//      CSR: Format is (Sparse, Index)
//
//          axis_order = { 0, 1 }, stored by-row
//          pointer [0] = non-NULL, of size m+1
//          pointer [1] = NULL
//          index [0] = NULL
//          index [1] = col indices, size nvals
//          in_order [0] = true
//          in_order [1] = true if index [1] in ascending order, false otherwise
//          values: size nvals
//
//      CSC: Format is (Sparse, Index)
//
//          axis_order = { 1, 0 }, stored by-row
//          pointer [0] = non-NULL, of size n+1
//          pointer [1] = NULL
//          index [0] = NULL
//          index [1] = col indices, size nvals
//          in_order [0] = true
//          in_order [1] = true if index [1] in ascending order, false otherwise
//          values: size nvals, or 1 if iso
//
//      DCSR (hypersparse by-row): Format is (Hyper, Index)
//
//          axis_order = { 0, 1 }, stored by-row
//          pointer [0] = non-NULL, of size nindex [0]+1
//          pointer [1] = NULL
//          index [0] = non-NULL, of size nindex [0]
//          index [1] = col indices, size nvals
//          in_order [0] = true
//          in_order [1] = true if index [1] in ascending order, false otherwise
//          values: size nvals, or 1 if iso
//
//      DCSC format (hypersparse by-col): Format is (Hyper, Index)
//
//          axis_order = { 1, 0 }, stored by-col
//          pointer [0] = non-NULL, of size nindex [0]+1
//          pointer [1] = NULL
//          index [0] = non-NULL, of size nindex [0]
//          index [1] = row indices, size nvals
//          in_order [0] = true
//          in_order [1] = true if index [1] in ascending order, false otherwise
//          values: size nvals, or 1 if iso
//
//      full format (held by row): Format is (Full, Full)
//
//          axis_order = { 0, 1 }, stored by-row
//          pointer [0] = NULL
//          pointer [1] = NULL
//          index [0] = NULL
//          index [1] = NULL
//          in_order [0] = true
//          in_order [1] = true
//          values: size nvals = m*n, or 1 if iso
//
//      full format (held by col): Format is (Full, Full)
//
//          axis_order = { 1, 0 }, stored by-col
//          pointer [0] = NULL
//          pointer [1] = NULL
//          index [0] = NULL
//          index [1] = NULL
//          in_order [0] = true
//          in_order [1] = true
//          values: size nvals = m*n, or 1 if iso
//
//      Index-Full format (held by row: each row is either full or all empty)
//          Format is (Index, Full)
//
//          axis_order = { 0, 1 }, stored by-row
//          pointer [0] = non-NULL, of size nindex [0]+1
//          pointer [1] = NULL
//          index [0] = non-NULL, of size nindex [0]
//          index [1] = NULL
//          in_order [0] = true
//          in_order [1] = true
//          values: size nvals = nindex [0]*n, or 1 if iso
//
//      Index-Full format (held by col: each col is either full or all empty)
//          Format is (Index, Full)
//
//          axis_order = { 1, 0 }, stored by-col
//          pointer [0] = non-NULL, of size nindex [0]+1
//          pointer [1] = NULL
//          index [0] = non-NULL, of size nindex [0]
//          index [1] = NULL
//          in_order [0] = true
//          in_order [1] = true
//          values: size nvals = m * nindex [0], or 1 if iso

//      Are all 16 formats possible?  NO.  I think there are only 5.

//          (Full, Sparse, Hyper, Index) x (Full, Sparse, Hyper, Index) ?
//          I think the last dimension must be Full or Index, which leads to
//          8 formats: (Full, Sparse, Hyper, Index) x (Full, Index).
//          5 Listed above are:
//
//              (Index, Index)      COO
//              (Sparse, Index)     CSR and CSC
//              (Hyper, Index)      hypersparse by row or col
//              (Full, Full)        full
//              (Index, Full)       can be defined, looks useful.  An unordered
//                                  set of full vectors.
//
//          not described above:  either not useful or not valid
//
//              (Hyper, Full)       not useful (rule 4)
//              (Sparse, Full)      can be defined but not useful?
//                                  see rule 5 below
//              (Full, Index)       huh?  See Rule (2) below.
//
//      bitmap format: held as two full bc_matrices with same dimension and
//          axis_order.  The first matrix ('bitmap' pattern) is always bool.
//          The second full matrix holds the values.
//          Format of both bc_matrices is (Full, Full)

// I think a few rules might work:
//
// (1) from left to right, if a "Full" format appears, the remaining
//      formats must all be Full.
//
// (2) the last format must be "Index" or "Full".
//
// (3) (... ,Sparse, Full, ...) can be defined but is not useful. It would be
//      the less storage cost and same representation as ( ... Full, Full, ...).
//      So do not support this format.
//
// (4) (... , Hyper, Full, ...) can be defined but is not useful.  The
//      same thing can be done with (... , Index, Full, ...) where the Index
//      dimension is sorted.  There's no need for the pointer for the Hyper
//      dimension, since all objects to the right have the same size.
//
// (5) Like rule 1, once "Index" appears, the remaining formats to the right
//      must be "Index" or "Full".  This is because "Index" has no pointer so
//      all formats to the right must have a known size, or be a list like
//      (Index, Index, Full) where the total size (nvals) is enough.

// rank = 3?
//
//      describe some for future extensions.  Possible formats, a subset of
//      (4 x 4 x 2) = 32 formats (rule 2) minus the 10 formats that break rule
//      1.  Some of these can be described but are not useful (rules 3 to 5),
//      This results in a total of 12 useful formats that could be supported.

//    * (Index , Index , Index)     all COO, nice
//      (Index , Hyper , Index)     unordered list of 2D hypersparse matrices?
//                                  but the first Index dimension has no pointer
//                                  so it can't be used to find each 2D slice.
//                                  Seems we need a rule 5.
//      (Index , Sparse, Index)     see rule 5
//      (Index , Full,   Index)     not possible (rule 1)

//    * (Hyper , Index , Index)     1D hyperlist of 2D COO matrices. Nice.
//    * (Hyper , Hyper , Index)     1D hyperlist of 2D hypersparse mtx. Nice
//    * (Hyper , Sparse, Index)     1D hyper list of 2D CSR/CSC matrices. Nice
//      (Hyper , Full  , Index)     not possible (rule 1), also rule 4

//    * (Sparse, Index , Index)     1D dense array of 2D COO matrices. Nice.
//    * (Sparse, Hyper , Index)     1D dense array of 2D hpyersparse. Nice.
//    * (Sparse, Sparse, Index)     1D dense array of 2D CSR/CSR mtx, Nice.
//      (Sparse, Full  , Index)     not possible (rule 1)

//      (Full  , Index , Index)     not possible (rule 1)
//      (Full  , Hyper , Index)     not possible (rule 1)
//      (Full  , Sparse, Index)     not possible (rule 1)
//      (Full  , Full  , Index)     not possible (rule 1)

//    * (Index , Index , Full )     like COO, but each "entry" is an entire
//                                  dense vector
//      (Index , Hyper , Full )     ok but not useful (rule 4)
//      (Index , Sparse, Full )     ok but not useful? (rule 3)
//    * (Index , Full  , Full )     Unordered list of 2D full matrices

//    * (Hyper , Index , Full )     2D hypersparse, each entry a full vec. Nice
//      (Hyper , Hyper , Full )     ok but not useful (rule 4)
//      (Hyper , Sparse, Full )     ok but not useful? (rule 3)
//      (Hyper , Full  , Full )     ok but not useful (rule 4)

//    * (Sparse, Index , Full )     1D dense array of 2D (Index,Full) Odd but ok
//      (Sparse, Hyper , Full )     ok but not useful (rule 4)
//      (Sparse, Sparse, Full )     ok but not useful? (rule 3)
//      (Sparse, Full  , Full )     ok but not useful? (rule 3)

//      (Full  , Index , Full )     not possible (rule 1)
//      (Full  , Hyper , Full )     not possible (rule 1), also rule 4
//      (Full  , Sparse, Full )     not possible (rule 1)
//    * (Full  , Full  , Full )



#define KMAX 32     // maximum rank allowed.  Is 32 too large?
    // should each of the KMAX-sized arrays be dynamically allocated?
    // With KMAX of 32, this struct is 1688 bytes in size.

typedef struct
{

    // basic information:  dimensions, type, and format

    int64_t magic ;             // for detecting uninitialized objects

    size_t header_size ;        // allocated size of this bc_matrix_struct,
                                // in bytes

    int rank ;                  // 0 to KMAX.  0: scalar, 1:vector, 2:matrix,
                                // 3: 3D tensor, etc.

    bc_type_code pointer_type ; // matrix 'pointer' type (any integer type)

    bc_type_code index_type ;   // matrix index type (any integer type)

    bc_type_code value_type ;   // matrix value type (bool, int8, ...).

    bool iso_valued ;           // if true, all entries have the same value,
                                // and thus only values [0] is used.

    size_t type_size ;          // sizeof (value type), that is, sizeof (bool),
                                // sizeof (int8), ...  Allows extension to
                                // user-defined types.

    uint8_t axis_order [KMAX] ; // axis ordering, only 0..rank-1 are used;
                                // a permutation of 0:rank-1

    uint64_t dimension [KMAX] ; // size of each dimension of the matrix

    bool in_order [KMAX] ;      // in_order [k] is true if the kth axis appears
                                // in strictly ascending order

    char *json_string ;         // metadata (may be NULL).  Allows future
                                // extension to user-defined types, with a json
                                // string.
    size_t json_string_size ;   // allocated size of json_string, in bytes

    // matrix content

    void *pointer [KMAX] ;      // set of pointers, of type pointer_type
    size_t pointer_size [KMAX] ;// allocated size of each pointer[k] array
                                // pointer [k] has nindex [k]+1 entries.

    void *index [KMAX] ;        // array of indices, of type index_type
    uint64_t nindex [KMAX] ;    // index [k] has nindex [k] entries
    size_t index_size [KMAX] ;  // allocated size of each index[k] array

    void *values ;              // array of values, of type value_type
                                // size 1 if iso, at least size nvals otherwise
    size_t values_size ;        // allocated size of values array, in bytes
    uint64_t nvals ;            // # of values present

}
bc_matrix_struct ;

// a bc_matrix is a pointer to the bc_matrix_struct shown above
typedef bc_matrix_struct *bc_matrix ;

#endif

