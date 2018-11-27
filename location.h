#ifndef LOCATION_H
#define LOCATION_H

#include <cstdio>

enum Direction {
  N, NE, E, SE, S, SW, W, NW,

  EoDirection
};

template<size_t NRows, size_t NCols> class Location {
public:
  Location() : offset(rcToOffset(NRows, NCols)) { }
  Location(size_t _offset) : offset(_offset < rcToOffset(NRows, NCols) ? _offset : rcToOffset(NRows, NCols)) { }
  Location(size_t r, size_t c) : offset(rcToOffset(r, c)) { }
  Location(Location const &that) : offset(that.offset) { }
  operator size_t() const { return offset; }
  size_t row() const { return offset / NCols; }
  size_t col() const { return offset % NCols; }
  bool isOnAnEdge() const {
    size_t r = row();
    size_t c = col();
    return !(0 < row && row < (NRows - 1) && 0 < col && col < (NCols - 1));
  }
  bool isOnAnEdgeToThe(Direction const &d) const {
    size_t r = row();
    size_t c = col();
    switch (d) {
    case N: return !(0 < r);
    case NE: return !(0 < r && c < (NCols - 1));
    case E: return !(c < (NCols - 1));
    case SE: return !(c < (NCols - 1) && r < (NRows - 1));
    case S: return !(r < (NRows - 1));
    case SW: return !(r < (NRows - 1) && 0 < c);
    case W: return !(0 < c);
    case NW: return !(0 < r && 0 < c);
    }
  }
  Location locationToThe(Direction const &d) const {
    return Location(offset + !isOnAnEdgeToThe(d) ? offsetToThe(d) : 0);
  }
  bool operator==(Location const &that) const { return offset == that.offset; }
  bool operator<(Location const &that) const { return offset < that.offset; }
  Location &operator+=(int by) {
    int result = int(offset) + by;
    if (result < 0) {
      result = 0;
    }
    if (size <= result) {
      result = size;
    }
    offset = result;
    return *this;
  }
  Location &operator-=(int by) {
    int result = int(offset) - by;
    if (result < 0) {
      result = 0;
    }
    if (size <= result) {
      result = size;
    }
    offset = result;
    return *this;
  }
  void fprint(FILE *out) const {
    fprintf(out, "%c%c", char(row() + 'a'), char(col() + 'a'));
  }

  static size_t rcToOffset(size_t r, size_t c) { return (r * NCols) + c; }

  static size_t const size = NRows * NCols;
  static size_t const nRows = NRows;
  static size_t const nCols = NCols;

private:
  Location(int o) : offset(size_t(o)) { }
  static int offsetToThe(Direction const &d) {
    static int offsets[EoDirection] = {
      -NCols,			// N
      -NCols + 1,		// NE
      +1,			// E
      +NCols + 1,		// SE
      +NCols,			// S
      +NCols - 1,		// SW
      -1,			// W
      -NCols - 1		// NW
    };
    return offsets[d];
  }

  size_t offset;
};

#endif // LOCATION_H
