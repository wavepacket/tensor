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

#ifndef TENSOR_PROFILE_PROFILE_H
#define TENSOR_PROFILE_PROFILE_H

#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <array>
#include <ctime>
#include <functional>
#include <memory>
#include <tensor/tools.h>
#include <tensor/config.h>

namespace benchmark {

static size_t __count_executions = 0;

template <typename T>
void force(const T &t) {
  __count_executions += t.size() != 0;
}

template <typename T>
void force_nonzero(const T &t) {
  __count_executions += (t != static_cast<T>(0));
}

struct BenchmarkSet;
struct BenchmarkGroup;
struct BenchmarkItem;

std::string tensor_acronym() {
  std::string compiler =
#if defined(_MSC_VER)
      "MSVC-";
#elif defined(__GNUC__)
      "GCC-";
#elif defined(__clang__)
      "Clang-";
#else
      "";
#endif
  std::string platform =
#if defined(_WIN64)
      "W64-";
#elif defined(_WIN32)
      "W32-";
#elif defined(__linux__)
      "Linux-";
#elif defined(__APPLE__)
      "Darwin-";
#else
          "";
#endif
  std::string blas_library =
#if defined(TENSOR_USE_ATLAS)
      "Atlas";
#elif defined(TENSOR_USE_OPENBLAS)
      "OpenBLAS";
#elif defined(TENSOR_USE_VECLIB)
      "Veclib";
#elif defined(TENSOR_USE_MKL)
      "MKL";
#elif defined(TENSOR_USE_ACML)
      "ACML";
#elif defined(TENSOR_USE_ESSL)
              "ESSL";
#elif defined(TENSOR_USE_CBLAPACK)
              "CBLAPACK";
#else
              "BLAS";
#endif
  return std::string("tensor ") + compiler + platform + blas_library;
}

std::string tensor_environment() {
  std::string compiler =
#if defined(_MSC_VER)
      "Microsoft C++";
#elif defined(__GNUC__)
      "Gnu C++";
#elif defined(__clang__)
      "Clang C++";
#else
      "Unknown C++ compiler";
#endif
  std::string platform =
#if defined(_WIN64)
      "Windows AMD64";
#elif defined(__linux__)
      "Linux";
#elif defined(__APPLE__)
      "Darwin";
#else
      "Unknown OS";
#endif
  std::string blas_library =
#if defined(TENSOR_USE_ATLAS)
      "Atlas";
#elif defined(TENSOR_USE_OPENBLAS)
      "OpenBLAS";
#elif defined(TENSOR_USE_VECLIB)
      "Apple Veclib";
#elif defined(TENSOR_USE_MKL)
      "Intel MKL";
#elif defined(TENSOR_USE_ACML)
              "ACML";
#elif defined(TENSOR_USE_ESSL)
              "IBM ESSL";
#elif defined(TENSOR_USE_CBLAPACK)
              "CBLAPACK";
#else
#error "Unknown BLAS library"
#endif
  return compiler + ", " + platform + ", " + blas_library;
}

std::vector<size_t> make_sizes(size_t start, size_t end, size_t factor = 2) {
  std::vector<size_t> output;
  while (start <= end) {
    output.push_back(start);
    start *= factor;
  }
  return output;
}

template <typename Functor>
inline double timeit(const Functor &f, size_t repeats) {
  tensor::tic();
  for (size_t j = 0; j < repeats; ++j) {
    f();
  }
  return tensor::toc();
}

template <typename Functor>
inline double autorange(const Functor &f, double limit = 0.2) {
  size_t repeats = 1;
  double time = 0.0;
  std::cerr.precision(17);
  for (int attempts = 4; attempts; --attempts) {
    time = timeit(f, repeats);
    if (time >= limit) {
      break;
    }
    repeats = static_cast<size_t>(1.5 * limit * static_cast<double>(repeats) /
                                  std::max(time, 1e-8));
  }
  return time / static_cast<double>(repeats);
}

struct BenchmarkItem {
  std::string name{};
  std::vector<size_t> sizes{};
  std::vector<double> times{};
  std::function<double(size_t)> benchmark;

  static std::vector<size_t> default_sizes() {
    return make_sizes(1, 4194304, 4);
  };

  template <class args_tuple>
  BenchmarkItem(std::string aname, void (*f)(args_tuple &),
                args_tuple (*s)(size_t), const std::vector<size_t> &asizes = {},
                bool run_now = true)
      : name(std::move(aname)),
        sizes(asizes.size() ? asizes : default_sizes()),
        times(sizes.size()),
        benchmark([s, f](size_t size) -> double {
          args_tuple args = s(size);
          return autorange([&]() { f(args); });
        }) {
    if (run_now) run();
  }

  void run() {
    times.clear();
    for (size_t i = 0; i < sizes.size(); i++) {
      auto size = sizes[i];
      auto time = times[i] = benchmark(size);
      std::cerr << "Executing item " << name << " at size " << size << " took "
                << time << " seconds per iteration\n";
    }
  }
};

struct BenchmarkGroup {
  using warmup_function_t = std::function<void()>;

  std::string name{};
  std::vector<BenchmarkItem> items{};
  bool warmup_run{false};
  warmup_function_t warmup_function{[] {}};

  BenchmarkGroup(std::string aname) : name(std::move(aname)), items{} {}

  BenchmarkGroup &operator<<(const BenchmarkItem &item) {
    items.push_back(item);
    return *this;
  }

  template <typename run, typename setup>
  BenchmarkGroup &add(const char *aname, run f, setup s,
                      const std::vector<size_t> &sizes = {},
                      bool run_now = false) {
    if (run_now) maybe_warmup();
    items.emplace_back(aname, f, s, sizes, run_now);
    return *this;
  }

  void set_warmup_function(warmup_function_t f) {
    warmup_function = std::move(f);
  }

  void run() {
    std::cerr << "------------------\nStarting group " << name << '\n';
    maybe_warmup();
    for (auto &item : items) {
      item.run();
    }
  }

  void maybe_warmup() {
    if (!warmup_run) {
      warmup_function();
      warmup_run = true;
    }
  }
};

struct BenchmarkSet {
  std::string name = tensor_acronym();
  std::string environment = tensor_environment();
  std::vector<BenchmarkGroup> groups{};

  BenchmarkSet(std::string aname) : name(std::move(aname)), groups{} {}

  BenchmarkSet &operator<<(const BenchmarkGroup &group) {
    groups.push_back(group);
    return *this;
  }

  void run() {
    std::cerr << "===================\nStarting set " << name << '\n'
              << "Environment: " << environment << '\n';
    for (auto &group : groups) {
      group.run();
    }
  }
};

template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &v) {
  const char *comma = "";
  out << '[';
  for (auto &item : v) {
    out << comma << item;
    comma = ",";
  }
  out << ']';
  return out;
}

std::ostream &operator<<(std::ostream &out, const BenchmarkSet &set) {
  out << "{\"name\": \"" << set.name << "\", \"environment\": \""
      << set.environment << "\", \"groups\": " << set.groups << "}";
  return out;
}

std::ostream &operator<<(std::ostream &out, const BenchmarkGroup &group) {
  out << "{\"name\": \"" << group.name << "\", \"items\": " << group.items
      << "}";
  return out;
}

std::ostream &operator<<(std::ostream &out, const BenchmarkItem &item) {
  out << "{\"name\": \"" << item.name << "\", \"sizes\": " << item.sizes
      << ", \"times\": " << item.times << "}";
  return out;
}

}  // namespace benchmark

#endif  // TENSOR_PROFILE_PROFILE_H
