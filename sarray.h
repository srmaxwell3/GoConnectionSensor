#ifndef SARRAY_H
#define SARRAY_H

#include "rarray.h"

template<typename T, size_t S> class sarray: public rarray<T, S, S> {
public:
  static size_t const Size = S;
};

#endif // SARRAY_H
