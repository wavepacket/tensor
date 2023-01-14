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
#ifndef TENSOR_TENSOR_OPERATORS_H
#define TENSOR_TENSOR_OPERATORS_H

#include <type_traits>
#include <tensor/tensor/types.h>
#include <tensor/traits.h>

/*!\addtogroup Tensors*/
/* @{ */

namespace tensor {

//
// Unary operations
//
template <typename t>
Tensor<t> operator-(const Tensor<t> &a) {
  Tensor<t> output(a.dimensions());
  std::transform(a.cbegin(), a.cend(), output.unsafe_begin_not_shared(),
                 [](const t &x) { return -x; });
  return output;
}

//
// TENSOR <OP> TENSOR
//
template <
    typename t1, typename t2,
    typename = std::enable_if_t<is_tensor<t1>::value && is_tensor<t2>::value>>
tensor_common_t<t1, t2> operator+(const t1 &a, const t2 &b) {
  // This should be: tensor_assert(a.dimensions() == b.dimensions());
  tensor_assert(a.size() == b.size());
  tensor_common_t<t1, t2> output(a.dimensions());
  std::transform(
      a.cbegin(), a.cend(), b.cbegin(), output.unsafe_begin_not_shared(),
      [](tensor_scalar_t<t1> x, tensor_scalar_t<t2> y) { return x + y; });
  return output;
}

template <
    typename t1, typename t2,
    typename = std::enable_if_t<is_tensor<t1>::value && is_tensor<t2>::value>>
tensor_common_t<t1, t2> operator-(const t1 &a, const t2 &b) {
  // This should be: tensor_assert(a.dimensions() == b.dimensions());
  tensor_assert(a.size() == b.size());
  tensor_common_t<t1, t2> output(a.dimensions());
  std::transform(
      a.cbegin(), a.cend(), b.cbegin(), output.unsafe_begin_not_shared(),
      [](tensor_scalar_t<t1> x, tensor_scalar_t<t2> y) { return x - y; });
  return output;
}
template <
    typename t1, typename t2,
    typename = std::enable_if_t<is_tensor<t1>::value && is_tensor<t2>::value>>
tensor_common_t<t1, t2> operator*(const t1 &a, const t2 &b) {
  // This should be: tensor_assert(a.dimensions() == b.dimensions());
  tensor_assert(a.size() == b.size());
  tensor_common_t<t1, t2> output(a.dimensions());
  std::transform(
      a.cbegin(), a.cend(), b.cbegin(), output.unsafe_begin_not_shared(),
      [](tensor_scalar_t<t1> x, tensor_scalar_t<t2> y) { return x * y; });
  return output;
}
template <
    typename t1, typename t2,
    typename = std::enable_if_t<is_tensor<t1>::value && is_tensor<t2>::value>>
tensor_common_t<t1, t2> operator/(const t1 &a, const t2 &b) {
  // This should be: tensor_assert(a.dimensions() == b.dimensions());
  tensor_assert(a.size() == b.size());
  tensor_common_t<t1, t2> output(a.dimensions());
  std::transform(
      a.cbegin(), a.cend(), b.cbegin(), output.unsafe_begin_not_shared(),
      [](tensor_scalar_t<t1> x, tensor_scalar_t<t2> y) { return x / y; });
  return output;
}

//
// TENSOR <OP> NUMBER
//
template <
    typename t1, typename t2,
    typename = std::enable_if_t<is_scalar<t2>::value && is_tensor<t1>::value>>
tensor_common_t<t1, t2> operator+(const t1 &a, t2 b) {
  tensor_common_t<t1, t2> output(a.dimensions());
  std::transform(a.cbegin(), a.cend(), output.unsafe_begin_not_shared(),
                 [=](tensor_scalar_t<t1> x) { return x + b; });
  return output;
}

template <
    typename t1, typename t2,
    typename = std::enable_if_t<is_scalar<t2>::value && is_tensor<t1>::value>>
tensor_common_t<t1, t2> operator-(const t1 &a, t2 b) {
  tensor_common_t<t1, t2> output(a.dimensions());
  std::transform(a.cbegin(), a.cend(), output.unsafe_begin_not_shared(),
                 [=](tensor_scalar_t<t1> x) { return x - b; });
  return output;
}

template <
    typename t1, typename t2,
    typename = std::enable_if_t<is_scalar<t2>::value && is_tensor<t1>::value>>
tensor_common_t<t1, t2> operator*(const t1 &a, t2 b) {
  tensor_common_t<t1, t2> output(a.dimensions());
  std::transform(a.cbegin(), a.cend(), output.unsafe_begin_not_shared(),
                 [=](tensor_scalar_t<t1> x) { return x * b; });
  return output;
}

template <
    typename t1, typename t2,
    typename = std::enable_if_t<is_scalar<t2>::value && is_tensor<t1>::value>>
tensor_common_t<t1, t2> operator/(const t1 &a, t2 b) {
  tensor_common_t<t1, t2> output(a.dimensions());
  std::transform(a.cbegin(), a.cend(), output.unsafe_begin_not_shared(),
                 [=](tensor_scalar_t<t1> x) { return x / b; });
  return output;
}

//
// NUMBER <OP> TENSOR
//
template <
    typename t1, typename t2,
    typename = std::enable_if_t<is_scalar<t1>::value && is_tensor<t2>::value>>
tensor_common_t<t1, t2> operator+(t1 a, const t2 &b) {
  tensor_common_t<t1, t2> output(b.dimensions());
  std::transform(b.cbegin(), b.end(), output.unsafe_begin_not_shared(),
                 [=](tensor_scalar_t<t2> x) { return a + x; });
  return output;
}

template <
    typename t1, typename t2,
    typename = std::enable_if_t<is_scalar<t1>::value && is_tensor<t2>::value>>
tensor_common_t<t1, t2> operator-(t1 a, const t2 &b) {
  tensor_common_t<t1, t2> output(b.dimensions());
  std::transform(b.cbegin(), b.end(), output.unsafe_begin_not_shared(),
                 [=](tensor_scalar_t<t2> x) { return a - x; });
  return output;
}

template <
    typename t1, typename t2,
    typename = std::enable_if_t<is_scalar<t1>::value && is_tensor<t2>::value>>
tensor_common_t<t1, t2> operator*(t1 a, const t2 &b) {
  tensor_common_t<t1, t2> output(b.dimensions());
  std::transform(b.cbegin(), b.end(), output.unsafe_begin_not_shared(),
                 [=](tensor_scalar_t<t2> x) { return a * x; });
  return output;
}

template <
    typename t1, typename t2,
    typename = std::enable_if_t<is_scalar<t1>::value && is_tensor<t2>::value>>
tensor_common_t<t1, t2> operator/(t1 a, const t2 &b) {
  tensor_common_t<t1, t2> output(b.dimensions());
  std::transform(b.cbegin(), b.end(), output.unsafe_begin_not_shared(),
                 [=](tensor_scalar_t<t2> x) { return a / x; });
  return output;
}

//
// TENSOR <OP=> TENSOR
//
template <typename t1, typename t2>
Tensor<t1> &operator+=(Tensor<t1> &a, const Tensor<t2> &b) {
  // This should be: tensor_assert(a.dimensions() == b.dimensions());
  tensor_assert(a.size() == b.size());
  auto start = a.begin();
  auto end = a.unsafe_end_not_shared();
  std::transform(start, end, b.cbegin(), start,
                 [](const t1 &x, const t2 &y) { return x + y; });
  return a;
}

template <typename t1, typename t2>
Tensor<t1> &operator-=(Tensor<t1> &a, const Tensor<t2> &b) {
  // This should be: tensor_assert(a.dimensions() == b.dimensions());
  tensor_assert(a.size() == b.size());
  auto start = a.begin();
  auto end = a.unsafe_end_not_shared();
  std::transform(start, end, b.cbegin(), start,
                 [](const t1 &x, const t2 &y) { return x - y; });
  return a;
}

template <typename t1, typename t2>
Tensor<t1> &operator*=(Tensor<t1> &a, const Tensor<t2> &b) {
  // This should be: tensor_assert(a.dimensions() == b.dimensions());
  tensor_assert(a.size() == b.size());
  auto start = a.begin();
  auto end = a.unsafe_end_not_shared();
  std::transform(start, end, b.cbegin(), start,
                 [](const t1 &x, const t2 &y) { return x * y; });
  return a;
}

template <typename t1, typename t2>
Tensor<t1> &operator/=(Tensor<t1> &a, const Tensor<t2> &b) {
  // This should be: tensor_assert(a.dimensions() == b.dimensions());
  tensor_assert(a.size() == b.size());
  auto start = a.begin();
  auto end = a.unsafe_end_not_shared();
  std::transform(start, end, b.cbegin(), start,
                 [](const t1 &x, const t2 y) { return x / y; });
  return a;
}

//
// TENSOR <OP=> NUMBER
//
template <typename t1, typename t2>
Tensor<t1> &operator+=(Tensor<t1> &a, t2 b) {
  auto start = a.begin();
  auto end = a.unsafe_end_not_shared();
  std::transform(start, end, start,
                 [=](const auto &value) { return value + b; });
  return a;
}
template <typename t1, typename t2>
Tensor<t1> &operator-=(Tensor<t1> &a, t2 b) {
  auto start = a.begin();
  auto end = a.unsafe_end_not_shared();
  std::transform(start, end, start,
                 [=](const auto &value) { return value - b; });
  return a;
}
template <typename t1, typename t2>
Tensor<t1> &operator*=(Tensor<t1> &a, t2 b) {
  auto start = a.begin();
  auto end = a.unsafe_end_not_shared();
  std::transform(start, end, start,
                 [=](const auto &value) { return value * b; });
  return a;
}
template <typename t1, typename t2>
Tensor<t1> &operator/=(Tensor<t1> &a, t2 b) {
  auto start = a.begin();
  auto end = a.unsafe_end_not_shared();
  std::transform(start, end, start,
                 [=](const auto &value) { return value / b; });
  return a;
}

//
// TENSOR <=> TENSOR
//
namespace detail {

template <typename t1, typename t2, class comparison>
Booleans compare_tensors(const Tensor<t1> &a, const Tensor<t2> &b,
                         comparison fn) {
  // This should be: tensor_assert(a.dimensions() == b.dimensions());
  tensor_assert(a.size() == b.size());
  Booleans output(a.size());
  std::transform(a.cbegin(), a.cend(), b.cbegin(),
                 output.unsafe_begin_not_shared(), fn);
  return output;
}

template <typename t1, class test>
Booleans test_tensor(const Tensor<t1> &a, test fn) {
  Booleans output(a.size());
  std::transform(a.cbegin(), a.cend(), output.unsafe_begin_not_shared(), fn);
  return output;
}

}  // namespace detail

template <typename t1, typename t2,
          typename = std::enable_if_t<std::is_floating_point<t1>::value &&
                                      std::is_floating_point<t2>::value>>
Booleans operator<(const Tensor<t1> &a, const Tensor<t2> &b) {
  return detail::compare_tensors(a, b, std::less<>());
}

template <typename t1, typename t2,
          typename = std::enable_if_t<std::is_floating_point<t1>::value &&
                                      std::is_floating_point<t2>::value>>
Booleans operator>(const Tensor<t1> &a, const Tensor<t2> &b) {
  return detail::compare_tensors(a, b, std::greater<>());
}

template <typename t1, typename t2>
Booleans operator==(const Tensor<t1> &a, const Tensor<t2> &b) {
  return detail::compare_tensors(a, b, std::equal_to<>());
}

template <typename t1, typename t2>
Booleans operator!=(const Tensor<t1> &a, const Tensor<t2> &b) {
  return detail::compare_tensors(a, b, std::not_equal_to<>());
}

template <typename t1, typename t2,
          typename = std::enable_if_t<std::is_floating_point<t1>::value &&
                                      std::is_floating_point<t2>::value>>
Booleans operator<=(const Tensor<t1> &a, const Tensor<t2> &b) {
  return detail::compare_tensors(a, b, std::less_equal<>());
}

template <typename t1, typename t2,
          typename = std::enable_if_t<std::is_floating_point<t1>::value &&
                                      std::is_floating_point<t2>::value>>
Booleans operator>=(const Tensor<t1> &a, const Tensor<t2> &b) {
  return detail::compare_tensors(a, b, std::greater_equal<>());
}

template <typename t1, typename t2>
bool all_equal(const Tensor<t1> &a, const Tensor<t2> &b) {
  return (a.ssize() == b.ssize()) &&
         std::equal(a.cbegin(), a.cend(), b.cbegin());
}

//
// TENSOR <=> NUMBER
//

template <typename t1,
          typename = std::enable_if_t<std::is_floating_point<t1>::value>>
Booleans operator<(const Tensor<t1> &a, t1 b) {
  return detail::test_tensor(a, [=](const t1 &a_value) { return a_value < b; });
}

template <typename t1,
          typename = std::enable_if_t<std::is_floating_point<t1>::value>>
Booleans operator<=(const Tensor<t1> &a, t1 b) {
  return detail::test_tensor(a,
                             [=](const t1 &a_value) { return a_value <= b; });
}

template <typename t1,
          typename = std::enable_if_t<std::is_floating_point<t1>::value>>
Booleans operator>(const Tensor<t1> &a, t1 b) {
  return detail::test_tensor(a, [=](const t1 &a_value) { return a_value > b; });
}

template <typename t1,
          typename = std::enable_if_t<std::is_floating_point<t1>::value>>
Booleans operator>=(const Tensor<t1> &a, t1 b) {
  return detail::test_tensor(a,
                             [=](const t1 &a_value) { return a_value >= b; });
}

template <typename t1>
Booleans operator==(const Tensor<t1> &a, t1 b) {
  return detail::test_tensor(a,
                             [=](const t1 &a_value) { return a_value == b; });
}

template <typename t1>
Booleans operator!=(const Tensor<t1> &a, t1 b) {
  return detail::test_tensor(a,
                             [=](const t1 &a_value) { return a_value != b; });
}

template <typename t1>
bool all_equal(const Tensor<t1> &a, t1 b) {
  return std::all_of(a.cbegin(), a.cend(),
                     [=](const t1 &a_value) { return a_value == b; });
}

//
// NUMBER <=> TENSOR
//

template <typename t1>
Booleans operator<(t1 a, const Tensor<t1> &b) {
  return b > a;
}

template <typename t1>
Booleans operator<=(t1 a, const Tensor<t1> &b) {
  return b >= a;
}

template <typename t1>
Booleans operator>(t1 a, const Tensor<t1> &b) {
  return b < a;
}

template <typename t1>
Booleans operator>=(t1 a, const Tensor<t1> &b) {
  return b <= a;
}

template <typename t1>
Booleans operator==(t1 a, const Tensor<t1> &b) {
  return b == a;
}

template <typename t1>
Booleans operator!=(t1 a, const Tensor<t1> &b) {
  return b != a;
}

template <typename t1>
bool all_equal(t1 a, const Tensor<t1> &b) {
  return std::all_of(b.cbegin(), b.cend(),
                     [=](const t1 &b_value) { return b_value == a; });
}

template <typename e1, typename e2>
auto pow(const Tensor<e1> &a, const Tensor<e2> &b) {
  // This should be: tensor_assert(a.dimensions() == b.dimensions());
  using e3 = typename std::common_type<e1, e2>::type;
  tensor_assert(a.size() == b.size());
  Tensor<e3> output(a.dimensions());
  std::transform(a.cbegin(), a.cend(), b.cbegin(),
                 output.unsafe_begin_not_shared(),
                 [](e1 x, e2 y) { return std::pow(x, y); });
  return output;
}

}  // namespace tensor

/* @} */

#endif  // TENSOR_TENSOR_OPERATORS_H
