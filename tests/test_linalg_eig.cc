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

#include "loops.h"
#include <gtest/gtest.h>
#include <tensor/tensor.h>
#include <tensor/linalg.h>

namespace tensor_test {

using namespace tensor;

//////////////////////////////////////////////////////////////////////
// EIGENVALUE DECOMPOSITIONS
//

template <typename elt_t>
void test_eye_eig(int n) {
  if (n == 0) {
#ifdef TENSOR_DEBUG
    ASSERT_THROW_DEBUG(linalg::eig(Tensor<elt_t>::eye(n, n)),
                       ::tensor::invalid_assertion);
#endif
    return;
  }
  Tensor<elt_t> Inn = Tensor<elt_t>::eye(n, n);
  CTensor U, V, s = linalg::eig(Inn, &U, &V);
  EXPECT_TRUE(all_equal(CTensor::eye(n), U));
  EXPECT_TRUE(all_equal(CTensor::eye(n), V));
  EXPECT_TRUE(all_equal(s, CTensor::ones(n)));
}

template <typename elt_t>
void test_random_eig(int n) {
  if (n == 0) {
#ifdef TENSOR_DEBUG
    ASSERT_THROW_DEBUG(linalg::eig(Tensor<elt_t>::eye(n, n)),
                       ::tensor::invalid_assertion);
#endif
    return;
  }
  for (int times = 10; times; --times) {
    auto A = Tensor<elt_t>::random(n, n);
    CTensor L, R, s = linalg::eig(A, &R, &L);
    CTensor dS = diag(s);
    constexpr double epsilon = 1e-12;
    EXPECT_CEQ3(mmult(A, R), mmult(R, dS), epsilon);
    EXPECT_CEQ3(mmult(adjoint(L), A), mmult(dS, adjoint(L)), epsilon);

    A = mmult(A, adjoint(A)) / norm0(A);
    s = linalg::eig(A, &R, &L);
    dS = diag(s);
    EXPECT_TRUE(norm0(abs(s) - s) < 1e-13);
    EXPECT_TRUE(unitaryp(L, 1e-10));
    EXPECT_TRUE(unitaryp(R, 1e-10));
    EXPECT_CEQ3(mmult(A, R), mmult(R, dS), epsilon);
    EXPECT_CEQ3(mmult(adjoint(L), A), mmult(dS, adjoint(L)), epsilon);
    EXPECT_CEQ3(to_complex(A), mmult(L, mmult(diag(s), adjoint(R))), epsilon);
  }
}

//////////////////////////////////////////////////////////////////////
// REAL SPECIALIZATIONS
//

TEST(RMatrixTest, EyeEigTest) {
  test_over_integers(0, 32, test_eye_eig<double>);
}

TEST(RMatrixTest, RandomEigTest) {
  test_over_integers(0, 32, test_random_eig<double>);
}

//////////////////////////////////////////////////////////////////////
// COMPLEX SPECIALIZATIONS
//

TEST(CMatrixTest, EyeEigTest) {
  test_over_integers(0, 32, test_eye_eig<cdouble>);
}

TEST(CMatrixTest, RandomEigTest) {
  test_over_integers(0, 32, test_random_eig<cdouble>);
}

}  // namespace tensor_test
