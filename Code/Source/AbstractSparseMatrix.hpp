//============================================================================
//
// This file is part of the Thea toolkit.
//
// This software is distributed under the BSD license, as detailed in the
// accompanying LICENSE.txt file. Portions are derived from other works:
// their respective licenses and copyright information are reproduced in
// LICENSE.txt and/or in the relevant source files.
//
// Author: Siddhartha Chaudhuri
// First version: 2019
//
//============================================================================

#ifndef __Thea_AbstractSparseMatrix_hpp__
#define __Thea_AbstractSparseMatrix_hpp__

#include "AbstractMatrix.hpp"

namespace Thea {

// Forward declarations
template <typename T> class AbstractCompressedSparseMatrix;

/** Abstract base interface for a 2D sparse matrix. Useful for passing matrices across shared library boundaries. */
template <typename T>
class /* THEA_API */ AbstractSparseMatrix : public virtual AbstractMatrix<T>
{
  public:
    THEA_DECL_SMART_POINTERS(AbstractSparseMatrix)

    /**
     * Get the number of entries actually stored in the matrix. These are often called "non-zeros", though they may actually
     * have the numeric value 0.
     */
    virtual int64 numStoredElements() const = 0;

    /**
     * If the matrix is stored in compressed column or row format, get a pointer to a derived interface supporting access
     * specific to that format. Else, return null.
     *
     * @note <code>dynamic_cast</code> does not work reliably across shared library boundaries, and relying on users to avoid it
     *   and only use <code>static_cast</code> is dangerous.
     */
    virtual AbstractCompressedSparseMatrix<T> const * asCompressed() const = 0;

    /**
     * If the matrix is stored in compressed column or row format, get a pointer to a derived interface supporting access
     * specific to that format. Else, return null.
     *
     * @note <code>dynamic_cast</code> does not work reliably across shared library boundaries, and relying on users to avoid it
     *   and only use <code>static_cast</code> is dangerous.
     */
    virtual AbstractCompressedSparseMatrix<T> * asCompressed() = 0;

}; // class AbstractSparseMatrix

} // namespace Thea

#endif
