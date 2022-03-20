/*
    Copyright (c) 2010 Juan Jose Garcia Ripoll

    Tensor is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published
    by the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Library General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <memory>
#include <tensor/tensor.h>
#include <tensor/tensor_lapack.h>
#include <tensor/linalg.h>

namespace linalg {

using tensor::SimpleVector;
using namespace lapack;

static RTensor economic_row_svd(const CTensor &A, CTensor *U, CTensor *VT) {
  RTensor s = RTensor::ones(1);
  if (U != nullptr) {
    *U = CTensor::ones(1, 1);
  }
  double n = s.at(0) = norm2(flatten(A));
  if (VT != nullptr) {
    *VT = A / n;
  }
  return s;
}

static RTensor economic_column_svd(const CTensor &A, CTensor *U, CTensor *VT) {
  RTensor s = RTensor::ones(1);
  if (VT != nullptr) {
    *VT = CTensor::ones(1, 1);
  }
  double n = s.at(0) = norm2(flatten(A));
  if (U != nullptr) {
    *U = A / n;
  }
  return s;
}

/**Singular value decomposition of a complex matrix.

     The singular value decomposition of a matrix A, consists in finding two
     unitary matrices U and V, and diagonal one S with nonnegative elements, such
     that \f$A = U S V\f$. The svd() routine computes the diagonal elements of
     the matrix S and puts them in a 1D tensor, which is the output of the
     routine.  Optionally, the matrices U and V are also computed, and stored in
     the variables pointed to by U and VT.

     Unless otherwise specified, if the matrix A has \c MxN elements, then U is
     \c MxM, V is \c NxN and the vector S will have \c min(M,N) elements. However
     if flag \c economic is different from zero, then we get smaller matrices,
     U being \c MxR, V being \c RxN and S will have \c R=min(M,N) elements.

     \ingroup Linalg
  */
RTensor svd(CTensor A, CTensor *U, CTensor *VT, bool economic) {
  /*
    if (accurate_svd) {
      return block_svd(A, U, VT, economic);
    }
    */
  tensor_assert(A.rows() > 0);
  tensor_assert(A.columns() > 0);
  tensor_assert(A.rank() == 2);

  blas::integer m = blas::tensor_rows(A);
  blas::integer n = blas::tensor_columns(A);

  if (m == 1) {
    if (n == 1 || economic) {
      return economic_row_svd(A, U, VT);
    }
  } else if (n == 1 && economic) {
    return economic_column_svd(A, U, VT);
  }

  blas::integer k = std::min(m, n);
  RTensor output = RTensor::empty(k);
  double *s = tensor_pointer(output);
  cdouble *a = tensor_pointer(A);

  char jobu{'N'};
  cdouble *u = nullptr;
  blas::integer ldu = 1;
  if (U) {
    *U = CTensor::empty(m, economic ? k : m);
    u = tensor_pointer(*U);
    jobu = economic ? 'S' : 'A';
    ldu = m;
  }

  char jobv{'N'};
  cdouble *v = nullptr;
  blas::integer ldv = 1;
  if (VT) {
    (*VT) = CTensor::empty(economic ? k : n, n);
    v = tensor_pointer(*VT);
    jobv = economic ? 'S' : 'A';
    ldv = economic ? k : n;
  }

  blas::integer info{};
#ifdef TENSOR_USE_ACML
  zgesvd(jobu, jobv, m, n, a, m, s, u, ldu, v, ldv, &info);
#else
  blas::integer lwork = -1;
  {
    cdouble work0{};
    double rwork0{};
    F77NAME(zgesvd)
    (&jobu, &jobv, &m, &n, a, &m, s, u, &ldu, v, &ldv, &work0, &lwork, &rwork0,
     &info);
    // work[0] contains the optimal amount of space required
    lwork = static_cast<blas::integer>(lapack::real(work0));
  }
  /* TODO: Guard against negative lwork using safe_size_t() */
  {
    SimpleVector<cdouble> work(tensor::safe_size_t(lwork));
    SimpleVector<double> rwork(5 * static_cast<size_t>(k));
    F77NAME(zgesvd)
    (&jobu, &jobv, &m, &n, a, &m, s, u, &ldu, v, &ldv, work.begin(), &lwork,
     rwork.begin(), &info);
  }
#endif
  return output;
}

}  // namespace linalg
