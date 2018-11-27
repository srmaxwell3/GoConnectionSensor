#ifndef RARRAY_H
#define RARRAY_H

#include <array>
using std::array;

#include <stdexcept>
using std::out_of_range;

template<typename T, size_t N, size_t M> class rarray: public array<T, N * M> {
public:
  static size_t const NRows = N;
  static size_t const NCols = M;

  typedef array<T, N * M> array_type;
  typedef typename array_type::const_reference const_reference;
  typedef typename array_type::reference reference;
  typedef typename array_type::size_type size_type;

  reference at(size_type n, size_type m) {
    if (N <= n) {
      throw out_of_range("rarray<T, N, M>::at(n, m): N <= n");
    }
    if (M <= m) {
      throw out_of_range("rarray<T, N, M>::at(n, m): M <= m");
    }
    return this->at(indexFrom(n, m));
  }

  const_reference at(size_type n, size_type m) const {
    if (N <= n) {
      throw out_of_range("rarray<T, N, M>::at(n, m): N <= n");
    }
    if (M <= m) {
      throw out_of_range("rarray<T, N, M>::at(n, m): M <= m");
    }
    return this->at(indexFrom(n, m));
  }

  const_reference operator()(size_type row, size_type col) const {
    return (*this)[indexFrom(row, col)];
  }
  reference operator()(size_type row, size_type col) {
    return (*this)[indexFrom(row, col)];
  }

private:
  static size_type indexFrom(size_type row, size_type col) {
    return (size_type(row) * M) + size_type(col);
  }
};

#endif // RARRAY_H
