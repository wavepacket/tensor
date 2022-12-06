#pragma once
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
#ifndef TENSOR_SPARSE_TYPES_H

#include <vector>
#include <tensor/tensor.h>

namespace tensor {

template <typename elt_t>
struct SparseTriplet {
  index_t row{0}, col{0};
  elt_t value{0};
  bool operator<(const SparseTriplet &other) const {
    if (row < other.row) return true;
    if (row == other.row) return col < other.col;
    return false;
  }
};

/**A sparse matrix. A sparse matrix is a compact representation of
     two-dimensional tensors that have a lot of zero elements. Our
     implementation behaves much like Matlab's sparse matrices, in the sense
     that one can build them up from 2D tensors, preallocate them, perform
     matrix multiplication with mmult(), etc, etc.

     \ingroup Tensors
  */
template <typename elt>
class CSRMatrix {
 public:
  using elt_t = elt;
  using tensor = Tensor<elt>;

  /**Build an empty matrix.*/
  CSRMatrix();
  /**Create a matrix with all elements set to zero.*/
  CSRMatrix(index_t rows, index_t cols, index_t nonzero = 0);
  /**Create a matrix from a collection of coordinates and values.*/
  CSRMatrix(std::vector<SparseTriplet<elt>> coordinates, index_t rows = 0,
            index_t columns = 0);
  /**Create a sparse matrix from the coordinates and values. */
  CSRMatrix(const Indices &row_indices, const Indices &column_indices,
            const Tensor<elt_t> &data, index_t rows = 0, index_t columns = 0);
  /* Create a sparse matrix from its internal representation. */
  CSRMatrix(Indices dims, Indices row_start, Indices column,
            Tensor<elt_t> data);
  /**Convert a tensor to sparse form.*/
  explicit CSRMatrix(const Tensor<elt_t> &tensor);
  /**Copy constructor.*/
  CSRMatrix(const CSRMatrix<elt_t> &s) = default;
  /**Move constructor.*/
  CSRMatrix(CSRMatrix<elt_t> &&s) = default;
  /**Assignment operator.*/
  CSRMatrix &operator=(const CSRMatrix<elt_t> &s) = default;
  /**Move assignment operator.*/
  CSRMatrix &operator=(CSRMatrix<elt_t> &&s) = default;
  /**Implicit conversion from other sparse types.*/
  // NOLINTNEXTLINE(*-explicit-constructor)
  template <typename e2>
  // cppcheck-suppress noExplicitConstructor
  CSRMatrix(const CSRMatrix<e2> &other)
      : dims_(other.priv_dims()),
        row_start_(other.priv_row_start()),
        column_(other.priv_column()),
        data_(other.priv_data()) {}
  ~CSRMatrix() = default;

  /**Return an element of the sparse matrix.*/
  elt_t operator()(index_t row, index_t col) const;

  /**Return CSRMatrix matrix dimensions.*/
  const Dimensions &dimensions() const { return dims_; }
  /**Length of a given CSRMatrix matrix index.*/
  index_t dimension(int which) const;
  /**Number of rows.*/
  index_t rows() const { return dims_[0]; }
  /**Number of columns*/
  index_t columns() const { return dims_[1]; }
  /**Number of nonzero elements.*/
  index_t length() const {
    index_t r = rows();
    return r ? row_start_[r] : 0;
  }

  /**Empty matrix?*/
  bool is_empty() const { return (rows() == 0) || (columns() == 0); }

  /**Identity matrix in sparse form.*/
  static CSRMatrix<elt_t> eye(index_t rows, index_t columns);
  /**Identity matrix in sparse form.*/
  static CSRMatrix<elt_t> eye(index_t rows) { return eye(rows, rows); }
  /**Return a random sparse matrix.*/
  static CSRMatrix<elt_t> random(index_t rows, index_t columns,
                                 double density = 0.2);

  template <typename t>
  friend const Tensor<t> full(const CSRMatrix<t> &s);

  const Dimensions &priv_dims() const { return dims_; }
  const Indices &priv_row_start() const { return row_start_; }
  const Indices &priv_column() const { return column_; }
  const Tensor<elt> &priv_data() const { return data_; }

 private:
  /** The dimensions (rows and columns) of the sparse matrix. */
  Dimensions dims_;
  /** Gives for each row of the matrix at which index the column_/data_ entries start. */
  Indices row_start_;
  /** Gives for each data_ entry the column in the matrix. */
  Indices column_;
  /** The single data entries. */
  Tensor<elt_t> data_;

  static std::vector<SparseTriplet<elt_t>> make_sparse_triplets(
      const Indices &rows, const Indices &cols, const Tensor<elt_t> &data);

  static CSRMatrix make_sparse(std::vector<SparseTriplet<elt_t>> sorted_data,
                               index nrows, index ncols);
};

template <typename elt_t>
using Sparse = CSRMatrix<elt_t>;

extern template class CSRMatrix<double>;
extern template class CSRMatrix<cdouble>;

using RSparse = Sparse<double>;
using CSparse = Sparse<cdouble>;

}  // namespace tensor

#endif  // TENSOR_SPARSE_TYPES
