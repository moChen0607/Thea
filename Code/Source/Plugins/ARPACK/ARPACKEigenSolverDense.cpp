//============================================================================
//
// This file is part of the Thea project.
//
// This software is covered by the following BSD license, except for portions
// derived from other works which are covered by their respective licenses.
// For full licensing information including reproduction of these external
// licenses, see the file LICENSE.txt provided in the documentation.
//
// Copyright (C) 2009, Siddhartha Chaudhuri/Stanford University
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holders nor the names of contributors
// to this software may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
//============================================================================

#include "ARPACKEigenSolver.hpp"
#include <ardsnsym.h>

namespace Thea {
namespace Algorithms {

int64
ARPACKEigenSolver::solveDense(AbstractDenseMatrix<float64> const & m, int32 nev, int8 shift_invert, float64 sigma, char * which,
                              int32 ncv, float64 tol, int32 maxit, float64 * resid, int8 auto_shift)
{
  try
  {
    // Create the matrix
    ARdsNonSymMatrix<float64, float64> arm(m.rows(), const_cast<float64 *>(m.data()));

    // Setup the problem
    std::shared_ptr< ARluNonSymStdEig<float64> > eig =
        shift_invert ? std::shared_ptr< ARluNonSymStdEig<float64> >(new ARluNonSymStdEig<float64>(
                                                                            nev, arm, sigma, which, ncv,
                                                                            tol, maxit, resid, auto_shift))
                     : std::shared_ptr< ARluNonSymStdEig<float64> >(new ARluNonSymStdEig<float64>(
                                                                            nev, arm, which, ncv, tol,
                                                                            maxit, resid, auto_shift));
    eig->Trace();

    // Find eigenpairs
    size_t nconv = (size_t)eig->FindEigenvectors();

    eigenvalues [0].resize(nconv); eigenvalues [1].resize(nconv);
    eigenvectors[0].resize(nconv); eigenvectors[1].resize(nconv);

    for (size_t i = 0; i < nconv; ++i)
    {
      eigenvalues[0][i] = eig->EigenvalueReal((int)i);
      eigenvalues[1][i] = eig->EigenvalueImag((int)i);

      eigenvectors[0][i].resize(ndims); eigenvectors[1][i].resize(ndims);
      for (intx j = 0; j < ndims; ++j)
      {
        eigenvectors[0][i][j] = eig->EigenvectorReal((int)i, (int)j);
        eigenvectors[1][i][j] = eig->EigenvectorImag((int)i, (int)j);
      }
    }

    return (int64)nconv;
  }
  THEA_STANDARD_CATCH_BLOCKS(return -1;, ERROR, "%s: Error solving dense eigensystem", getName())
}

} // namespace Algorithms
} // namespace Thea
