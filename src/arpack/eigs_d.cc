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

#include "eigs_tools.h"
#include "gemv.cc"

namespace linalg {

using namespace tensor;

CTensor eigs_gen(const RTensor &A, EigType eig_type, size_t neig,
                 CTensor *eigenvectors, bool *converged) {
  auto n = blas::tensor_columns(A);
  if (n <= 4) {
    /* For small sizes, the ARPACK solver produces wrong results!
       * In any case, for these sizes it is more efficient to do the solving
       * using the full routine.
       */
    return eigs_gen_small(A, eig_type, neig, eigenvectors, converged);
  }
  return eigs_gen(
      [&](const RTensor &in, RTensor &out) {
        blas::gemv('N', n, n, 1.0, A.begin(), n, in.begin(), 1, 0.0,
                   out.begin(), 1);
      },
      static_cast<size_t>(n), eig_type, neig, eigenvectors, converged);
}

}  // namespace linalg
