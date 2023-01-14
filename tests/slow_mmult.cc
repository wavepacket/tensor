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

#include <tensor/exceptions.h>
#include <tensor/tensor.h>

namespace tensor_test {

using namespace tensor;

template <typename n1, typename n2>
Tensor<typename std::common_type<n1, n2>::type> fold_22_12(
    const Tensor<n1> &A, const Tensor<n2> &B) {
  typedef typename std::common_type<n1, n2>::type n3;
  index_t a1, a2, b1, b2;
  A.get_dimensions(&a1, &a2);
  B.get_dimensions(&b1, &b2);
  tensor_assert(a2 == b1);

  auto output = Tensor<n3>::empty(a1, b2);

  for (index_t i = 0; i < a1; i++) {
    for (index_t k = 0; k < b2; k++) {
      n3 x = number_zero<n3>();
      for (index_t j = 0; j < a2; j++) {
        x += A(i, j) * B(j, k);
      }
      output.at(i, k) = x;
    }
  }
  return output;
}

}  // namespace tensor_test
