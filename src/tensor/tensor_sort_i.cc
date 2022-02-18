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

#include <algorithm>
#include <functional>
#include <tensor/tensor.h>

namespace tensor {

Indices sort(const Indices &v, bool reverse) {
  Indices output(v);
  if (reverse) {
    std::sort(output.begin(), output.end(), std::greater<Indices::elt_t>());
  } else {
    std::sort(output.begin(), output.end(), std::less<Indices::elt_t>());
  }
  return output;
}

template <typename elt_t>
struct Compare {
  const elt_t *p;

  Compare(const elt_t *newp) : p(newp){};
  int operator()(index i1, index i2) { return p[i1] < p[i2]; }
};

template <typename elt_t>
struct CompareInv {
  const elt_t *p;

  CompareInv(const elt_t *newp) : p(newp){};
  int operator()(index i1, index i2) { return p[i1] > p[i2]; }
};

Indices sort_indices(const Indices &v, bool reverse) {
  if (v.size()) {
    Indices output = iota(0, v.ssize() - 1);
    if (reverse) {
      CompareInv<index> c(v.begin());
      std::sort(output.begin(), output.end(), c);
    } else {
      Compare<index> c(v.begin());
      std::sort(output.begin(), output.end(), c);
    }
    return output;
  } else {
    return Indices();
  }
}

}  // namespace tensor
