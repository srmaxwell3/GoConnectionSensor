#include <bitset>

template<size_t S> struct square_bitset: public bitset<S * S> {
  square_bitset() {
    bitset<S * S>::reset();
  }

  static size_t at(int row, int col) {
    return (size_t(row) * S + size_t(col);
  }

  bool operator()(int row, int col) const {
      return (*this)[at(row, col)];
  }
  BitSet::reference operator()(int row, int col) {
    return (*this)[at(row, col)];
  }
};
