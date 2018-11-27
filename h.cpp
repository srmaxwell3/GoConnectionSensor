#include <cassert>
#include <cstdlib>
#include <cstdio>

#include <algorithm>

#include <array>
using std::array;

#include <bitset>
using std::bitset;

#include <map>
using std::map;

#include <set>
using std::set;

#include <utility>
using std::pair;

// #include "sarray.h"

size_t const bSize = 19;

char const *ARGV0 = "g";

enum Direction {
  N, NE, E, SE, S, SW, W, NW,

  EoDirection
};

template<size_t NRows, size_t NCols> class BoardLocation {
public:
  BoardLocation(size_t r, size_t c) : offset(rcToOffset(r, c)) { }
  BoardLocation(BoardLocation const &that) : offset(that.offset) { }
  operator size_t() const { return offset; }
  size_t row() const { return offset / NCols; }
  size_t col() const { return offset % NCols; }
  bool isOnAnEdge() const {
    size_t r = row();
    size_t c = col();
    return !(0 < r && r < (NRows - 1) && 0 < c && c < (NCols - 1));
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
    case EoDirection: assert(d != EoDirection);
    }
  }
  BoardLocation locationToThe(Direction const &d) const {
    return BoardLocation(offset + (!isOnAnEdgeToThe(d) ? offsetToThe(d) : 0));
  }
  bool operator==(BoardLocation const &that) const { return offset == that.offset; }
  bool operator<(BoardLocation const &that) const { return offset < that.offset; }
  BoardLocation &operator++() {
    offset += 1;
    return *this;
  }
  BoardLocation &operator--() {
    offset -= 1;
    return *this;
  }

  static BoardLocation begin() { return BoardLocation(0, 0); }
  static BoardLocation end() { return BoardLocation(NRows, NCols); }

  static size_t rcToOffset(size_t r, size_t c) { return (r * NCols) + c; }

  static size_t const size = NRows * NCols;
  static size_t const nRows = NRows;
  static size_t const nCols = NCols;

private:
  BoardLocation(int o) : offset(size_t(o)) { }
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

enum Point {
  Illegal,
  Empty,
  Black,
  White,

  EoPoint
};

char const toChar(Point const &i) {
  switch (i) {
  case Illegal: return '#';
  case Empty: return '.';
  case Black: return '@';
  case White: return 'O';
  case EoPoint: assert(i != EoPoint);
  }
  return '?';
}

template<typename T> struct PArray: public array<T, size_t(EoPoint)> {
};

template<size_t NRows, size_t NCols> struct BoardSet: public bitset<NRows * NCols> {
  typedef bitset<NRows * NCols> BitSet;

  BoardSet() {
    BitSet::reset();
  }

  bool operator()(size_t row, size_t col) const {
    return (*this)[toIndex(row, col)];
  }
  typename BitSet::reference operator()(size_t row, size_t col) {
    return (*this)[toIndex(row, col)];
  }

private:
  static size_t toIndex(size_t row, size_t col) {
    return (row * NCols) + col;
  }
};

template<size_t NRows, size_t NCols> class BoardModel: public array<BoardSet<NRows, NCols>, size_t(EoPoint)> {
public:
  BoardModel() {
    reset();
  }

  Point pointAt(int i, int j) const {
    if (i < 0 || NRows <= i || j < 0 || NCols <= j) {
      return Illegal;
    }

    if ((*this)[Black](i, j)) {
      return Black;
    }
    if ((*this)[White](i, j)) {
      return White;
    }
    if ((*this)[Empty](i, j)) {
      return Empty;
    }

    assert((*this)[Black](i, j) || (*this)[White](i, j) || (*this)[Empty](i, j));
  }

  void reset() {
    (*this)[Illegal].reset();
    (*this)[Black].reset();
    (*this)[White].reset();
    (*this)[Empty].set();
  }

  bool isEmpty(size_t i, size_t j) const {
    return (*this)[Empty](i, j) == 1;
  }
  bool isBlack(size_t i, size_t j) const {
    return (*this)[Black](i, j) == 1;
  }
  bool isWhite(size_t i, size_t j) const {
    return (*this)[White](i, j) == 1;
  }
  bool isFriend(size_t i, size_t j, Point who) const {
    switch (who) {
    case Black: return (*this)[Black](i, j) == 1;
    case White: return (*this)[White](i, j) == 1;
    }
    return false;
  }
  bool isEnemy(size_t i, size_t j, Point who) const {
    switch (who) {
    case Black: return (*this)[White](i, j) == 1;
    case White: return (*this)[Black](i, j) == 1;
    }
    return false;
  }

  void put(size_t i, size_t j, Point who) {
    switch (who) {
    case Black:
      (*this)[Black](i, j) = 1;
      (*this)[White](i, j) = 0;
      (*this)[Empty](i, j) = 0;
      break;
    case White:
      (*this)[Black](i, j) = 0;
      (*this)[White](i, j) = 1;
      (*this)[Empty](i, j) = 0;
      break;
    case Empty:
      (*this)[Black](i, j) = 0;
      (*this)[White](i, j) = 0;
      (*this)[Empty](i, j) = 1;
      break;
    }
  }

  void fprint(FILE *out) const {
    fprintf(out, " ");
    for (size_t j = 0; j < NCols; j += 1) {
      fprintf(out, " %c", char('a' + j));
    }
    fprintf(out, "\n");

    for (size_t i = 0; i < NRows; i += 1) {
      fprintf(out, "%c", char('a' + i));

      for (size_t j = 0; j < NCols; j += 1) {
	fprintf(out, " %c", toChar(pointAt(i, j)));
      }

      fprintf(out, "\n");
    }

    fprintf(out, "\n");
  }
};

struct NeighborhoodCounts: PArray<size_t> {
  NeighborhoodCounts() {
    std::fill(begin(), end(), 0);
  }
  void fprint(FILE *out) const {
    fprintf(out, "%u%u%u%u", (*this)[Illegal], (*this)[Empty], (*this)[Black], (*this)[White]);
  }
};

template<size_t NRows, size_t NCols> struct BoardNeighborhoodCounts: public rarray<NeighborhoodCounts, NRows, NCols> {
  BoardNeighborhoodCounts() {
    NeighborhoodCounts empty;

    std::fill(this->begin(), this->end(), empty);
  }

  void Fill(BoardModel<NRows, NCols> const &board) {
    for (int i = 0; i < NRows; i += 1) {
      for (int j = 0; j < NCols; j += 1) {
	Point point = board.pointAt(i, j);

	if (0 < i) {
	  (*this)(i - 1, j)[point] += 1;
	}
	if (i < (NRows - 1)) {
	  (*this)(i + 1, j)[point] += 1;
	}
	if (0 < j) {
	  (*this)(i, j - 1)[point] += 1;
	}
	if (j < (NCols - 1)) {
	  (*this)(i, j + 1)[point] += 1;
	}

	// (*this)[i - 1][j][point] += 1;
	// (*this)[i + 1][j][point] += 1;
	// (*this)[i][j - 1][point] += 1;
	// (*this)[i][j + 1][point] += 1;
      }
    }

    for (size_t c = 0; c < NCols; c += 1) {
      (*this)(        0, c)[Illegal] += 1;
      (*this)(        1, c)[board.pointAt(        0, c)] += 1;      
      (*this)(NCols - 2, c)[board.pointAt(NCols - 1, c)] += 1;      
      (*this)(NCols - 1, c)[Illegal] += 1;
    }
    for (size_t r = 0; r < NRows; r += 1) {
      (*this)(r,         0)[Illegal] += 1;
      (*this)(r,         1)[board.pointAt(r,         0)] += 1;      
      (*this)(r, NRows - 2)[board.pointAt(r, NRows - 1)] += 1;      
      (*this)(r, NRows - 1)[Illegal] += 1;
    }
  }

  void fprint(FILE *out) const {
    fprintf(out, " ");
    for (size_t j = 0; j < NCols; j += 1) {
      fprintf(out, "    %c", char('a' + j));
    }
    fprintf(out, "\n");

    for (size_t i = 0; i < NRows; i += 1) {
      fprintf(out, "%c", char('a' + i));

      for (size_t j = 0; j < NCols; j += 1) {
	fprintf(out, " ");
	(*this)(i, j).fprint(out);
      }

      fprintf(out, "\n");
    }

    fprintf(out, "\n");
  }
};

pair<size_t, size_t> rcMap19x19[19][19] = {
  //          0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16    17    18
  /*  0 */ {{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0},{8,0},{9,0},{8,0},{7,0},{6,0},{5,0},{4,0},{3,0},{2,0},{1,0},{0,0}},
  /*  1 */ {{1,0},{1,1},{2,1},{3,1},{4,1},{5,1},{6,1},{7,1},{8,1},{9,1},{8,1},{7,1},{6,1},{5,1},{4,1},{3,1},{2,1},{1,1},{1,0}},
  /*  2 */ {{2,0},{2,1},{2,2},{3,2},{4,2},{5,2},{6,2},{7,2},{8,2},{9,2},{8,2},{7,2},{6,2},{5,2},{4,2},{3,2},{2,2},{2,1},{2,0}},
  /*  3 */ {{3,0},{3,1},{3,2},{3,3},{4,3},{5,3},{6,3},{7,3},{8,3},{9,3},{8,3},{7,3},{6,3},{5,3},{4,3},{3,3},{3,2},{3,1},{3,0}},
  /*  4 */ {{4,0},{4,1},{4,2},{4,3},{4,4},{5,4},{6,4},{7,4},{8,4},{9,4},{8,4},{7,4},{6,4},{5,4},{4,4},{4,3},{4,2},{4,1},{4,0}},
  /*  5 */ {{5,0},{5,1},{5,2},{5,3},{5,4},{5,5},{6,5},{7,5},{8,5},{9,5},{8,5},{7,5},{6,5},{5,5},{5,4},{5,3},{5,2},{5,1},{5,0}},
  /*  6 */ {{6,0},{6,1},{6,2},{6,3},{6,4},{6,5},{6,6},{7,6},{8,6},{9,6},{8,6},{7,6},{6,6},{6,5},{6,4},{6,3},{6,2},{6,1},{6,0}},
  /*  7 */ {{7,0},{7,1},{7,2},{7,3},{7,4},{7,5},{7,6},{7,7},{8,7},{9,7},{8,7},{7,7},{7,6},{7,5},{7,4},{7,3},{7,2},{7,1},{7,0}},
  /*  8 */ {{8,0},{8,1},{8,2},{8,3},{8,4},{8,5},{8,6},{8,7},{8,8},{9,8},{8,8},{8,7},{8,6},{8,5},{8,4},{8,3},{8,2},{8,1},{8,0}},
  /*  9 */ {{9,0},{9,1},{9,2},{9,3},{9,4},{9,5},{9,6},{9,7},{9,8},{9,9},{9,8},{9,7},{9,6},{9,5},{9,4},{9,3},{9,2},{9,1},{9,0}},
  /* 10 */ {{8,0},{8,1},{8,2},{8,3},{8,4},{8,5},{8,6},{8,7},{8,8},{9,8},{8,8},{8,7},{8,6},{8,5},{8,4},{8,3},{8,2},{8,1},{8,0}},
  /* 11 */ {{7,0},{7,1},{7,2},{7,3},{7,4},{7,5},{7,6},{7,7},{8,7},{9,7},{8,7},{7,7},{7,6},{7,5},{7,4},{7,3},{7,2},{7,1},{7,0}},
  /* 12 */ {{6,0},{6,1},{6,2},{6,3},{6,4},{6,5},{6,6},{7,6},{8,6},{9,6},{8,6},{7,6},{6,6},{6,5},{6,4},{6,3},{6,2},{6,1},{6,0}},
  /* 13 */ {{5,0},{5,1},{5,2},{5,3},{5,4},{5,5},{6,5},{7,5},{8,5},{9,5},{8,5},{7,5},{6,5},{5,5},{5,4},{5,3},{5,2},{5,1},{5,0}},
  /* 14 */ {{4,0},{4,1},{4,2},{4,3},{4,4},{5,4},{6,4},{7,4},{8,4},{9,4},{8,4},{7,4},{6,4},{5,4},{4,4},{4,3},{4,2},{4,1},{4,0}},
  /* 15 */ {{3,0},{3,1},{3,2},{3,3},{4,3},{5,3},{6,3},{7,3},{8,3},{9,3},{8,3},{7,3},{6,3},{5,3},{4,3},{3,3},{3,2},{3,1},{3,0}},
  /* 16 */ {{2,0},{2,1},{2,2},{3,2},{4,2},{5,2},{6,2},{7,2},{8,2},{9,2},{8,2},{7,2},{6,2},{5,2},{4,2},{3,2},{2,2},{2,1},{2,0}},
  /* 17 */ {{1,0},{1,1},{2,1},{3,1},{4,1},{5,1},{6,1},{7,1},{8,1},{9,1},{8,1},{7,1},{6,1},{5,1},{4,1},{3,1},{2,1},{1,1},{1,0}},
  /* 18 */ {{0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0},{8,0},{9,0},{8,0},{7,0},{6,0},{5,0},{4,0},{3,0},{2,0},{1,0},{0,0}}
};

template<size_t NRows, size_t NCols> struct Pattern {
  Pattern(BoardModel<NRows, NCols> const &board, size_t r, size_t c) :
    value (0)
  {
    //-------+-------+-------+-------+
    // 0 1 2 | 6 3 0 | 8 7 6 | 2 5 8 |
    // 3 4 5 | 7 4 1 | 5 4 3 | 1 4 7 |
    // 6 7 8 | 8 5 2 | 2 1 0 | 0 3 6 |
    //-------+-------+-------+-------+
    // 2 1 0 | 0 3 6 | 6 7 8 | 8 5 2 |
    // 5 4 3 | 1 4 7 | 3 4 5 | 7 4 1 |
    // 8 7 6 | 2 5 8 | 0 1 2 | 6 3 0 |
    //-------+-------+-------+-------+

    static size_t rotate3x3[9][8] = {
      { 0, 6, 8, 2, 2, 0, 6, 8 }, // 0 (0,0)
      { 1, 3, 7, 5, 1, 3, 7, 5 }, // 1 (0,1)
      { 2, 0, 6, 8, 0, 6, 8, 2 }, // 2 (0,2)
      { 3, 7, 5, 1, 5, 1, 3, 7 }, // 3 (1,0)
      { 4, 4, 4, 4, 4, 4, 4, 4 }, // 4 (1,1)
      { 5, 1, 3, 7, 3, 7, 5, 1 }, // 5 (1,2)
      { 6, 8, 2, 0, 8, 2, 0, 6 }, // 6 (2,0)
      { 7, 5, 1, 3, 7, 5, 1, 3 }, // 7 (2,1)
      { 8, 2, 0, 6, 6, 8, 2, 0 }  // 9 (2,2)
    };

    assert(board.pointAt(r, c) == Empty);

    unsigned row = rcMap19x19[r][c].first;
    unsigned col = rcMap19x19[r][c].second;

    // fprintf(stdout, "Pattern(board[%c%c -> %c%c]) -> ", char('a' + r), char('a' + c), char('a' + row), char('a' + col));

    value = (row * 32) + col;

    array<int, 9> points;
    size_t k = 0;
    for (int i = 0; i < 3; i += 1) {
      for (int j = 0; j < 3; j += 1) {
	points[k] = unsigned(board.pointAt(r + i - 1, c + j - 1)) & 0x3;
	// fprintf(stdout, "%c", toChar(Point(points[k])));
	k += 1;
      }
    }

    array<unsigned, 8> values;
    values.fill(value);

    for (k = 0; k < 9; k += 1) {
      for (size_t r = 0; r < 8; r += 1) {
	values[r] *= 4;
	values[r] += points[rotate3x3[k][r]];
      }
    }

    // fprintf(stdout, "{0x%08x!", values[0]);
    value = values[0];
    for (k = 1; k < 8; k += 1) {
      // fprintf(stdout, ",0x%08x", values[k]);
      if (values[k] < value) {
	// fprintf(stdout, "!", values[k]);
	value = values[k];
      }
    }
    // fprintf(stdout, "} (0x%08x) ", value);
    // fprint(stdout);
    // fprintf(stdout, "\n");
  }

  Pattern() : value (-1) { }
  Pattern(Pattern const &that) : value (that.value) { }
  bool operator==(Pattern const &that) const { return value == that.value; }
  bool operator<(Pattern const &that) const { return value < that.value; }

  void fprint(FILE *out) const {
    union Value {
      struct {
	unsigned lr : 2;
	unsigned lc : 2;
	unsigned ll : 2;
	unsigned cr : 2;
	unsigned cc : 2;
	unsigned cl : 2;
	unsigned ur : 2;
	unsigned uc : 2;
	unsigned ul : 2;
	unsigned c  : 5;
	unsigned r  : 5;
      } fs;
      unsigned bs;
    } v;

    v.bs = value;

    fprintf(out,
	    "%c%c_%c%c%c%c%c%c%c%c%c",
	    char('a' + v.fs.r), char('a' + v.fs.c),
	    toChar(Point(v.fs.ul)),
	    toChar(Point(v.fs.uc)),
	    toChar(Point(v.fs.ur)),
	    toChar(Point(v.fs.cl)),
	    toChar(Point(v.fs.cc)),
	    toChar(Point(v.fs.cr)),
	    toChar(Point(v.fs.ll)),
	    toChar(Point(v.fs.lc)),
	    toChar(Point(v.fs.lr))
	   );
  }

  unsigned value;
};

template<size_t NRows, size_t NCols> struct BoardPatterns: public map<Pattern<NRows, NCols>, size_t> {
  BoardPatterns() { }

  void Fill(BoardModel<NRows, NCols> const &board) {
    for (int i = 0; i < NRows; i += 1) {
      for (int j = 0; j < NCols; j += 1) {
	if (board.pointAt(i, j) == Empty) {
	  Pattern<NRows, NCols> pattern(board, i, j);
	  (*this)[pattern] += 1;
	}
      }
    }
  }

  void fprint(FILE *out) const {
    for (auto p = this->cbegin(); p != this->cend(); p++) {
      p->first.fprint(out);
      fprintf(out, "(%d)\n", p->second);
    }

    fprintf(out, "\n");
  }
};

template<size_t NRows, size_t NCols> class Group: set<pair<size_t, size_t>> {
public:
  Group(Point const &point):
    groupOf (point)
  {
  }

  void Fill(BoardModel<NRows, NCols> const &board, size_t i, size_t j) {
    assert(groupOf == board.pointAt(i, j));

    insert({ i, j });
  }

  void fprint(FILE *out) const {
    fprintf(out, "{");
    auto p = cbegin();
    if (p != cend()) {
      fprintf(out, " %c%c", 'a' + p->first, 'a' + p->second);
      while (++p != cend()) {
	fprintf(out, ", %c%c", 'a' + p->first, 'a' + p->second);
      }
    }
    fprintf(out, " }");
  }

  Point point() const { return groupOf; }

private:
  Point groupOf;
};

template<size_t NRows, size_t NCols> class Groups: public PArray<set<Group<NRows, NCols> *>> {
public:
  Groups() {
    std::fill(pointGroups.begin(), pointGroups.end(), (Group<NRows, NCols> *) 0);
  }

  void Fill(BoardModel<NRows, NCols> const &board, Group<NRows, NCols> *group, size_t i, size_t j) {
    assert(group);

    Point point = group->point();

    assert(point == board.pointAt(i, j));
    assert(!pointGroups(i, j));

    group->Fill(board, i, j);
    pointGroups(i, j) = group;

    if (0 < i) {
      if (!pointGroups(i - 1, j) && point == board.pointAt(i - 1, j)) {
	Fill(board, group, i - 1, j);
      }
    }
    if (i < (bSize - 1)) {
      if (!pointGroups(i + 1, j) && point == board.pointAt(i + 1, j)) {
	Fill(board, group, i + 1, j);
      }
    }
    if (0 < j) {
      if (!pointGroups(i, j - 1) && point == board.pointAt(i, j - 1)) {
	Fill(board, group, i, j - 1);
      }
    }
    if (j < (bSize - 1)) {
      if (!pointGroups(i, j + 1) && point == board.pointAt(i, j + 1)) {
	Fill(board, group, i, j + 1);
      }
    }
  }

  void Fill(BoardModel<NRows, NCols> const &board) {
    for (size_t i = 0; i < bSize; i += 1) {
      for (size_t j = 0; j < bSize; j += 1) {
	if (!pointGroups(i, j)) {
	  Point point = board.pointAt(i, j);
	  Group<NRows, NCols> *group = new Group<NRows, NCols>(point);

	  (*this)[point].insert(group);
	  Fill(board, group, i, j);
	}
      }
    }
  }

  void fprint(FILE *out) const {
    fprintf(out, "Groups = {\n");
    for (Point p = Illegal; p < EoPoint; p = Point(size_t(p) + 1)) {
      fprintf(out, "  [%u] {\n", p);
      for (auto g = (*this)[p].begin(); g != (*this)[p].end(); g++) {
	fprintf(out, "    ");
	(*g)->fprint(out);
	fprintf(out, "\n");
      }
      fprintf(out, "  }\n");
    }
    fprintf(out, "}\n");
  }

  rarray<Group<NRows, NCols> *, NRows, NCols> pointGroups;
};

int main(int argc, char const *argv[])
{
  size_t const NRows = 19;
  size_t const NCols = 19;

  ARGV0 = argv[0];

  // BoardModel board;
  BoardModel<19, 19> board;

  size_t nStones = ((NRows * NCols) * 8) / 10;
  int i = rand() % NRows;
  int j = rand() % NCols;

  BoardSet<NRows, NCols> filled;
  Point who = Black;

  for (size_t n = 0; n < nStones; n += 1) {
    while (!board.isEmpty(i, j)) {
      int iNew = i + ((rand() % 8) - 4);
      if (iNew < 0 || NRows <= iNew) {
	continue;
      }

      int jNew = j + ((rand() % 8) - 4);
      if (jNew < 0 || NCols <= jNew) {
	continue;
      }

      i = iNew;
      j = jNew;
    }

    fprintf(stdout,
	    "%c[%c%c]\n",
	    who == Black ? 'B' : 'W',
	    'a' + i,
	    'a' + j
	   );

    board.put(i, j, who);

    board.fprint(stdout);

    BoardNeighborhoodCounts<19, 19> counts;
    counts.Fill(board);
    counts.fprint(stdout);

    Groups<19, 19> groups;
    groups.Fill(board);
    groups.fprint(stdout);

    BoardPatterns<19, 19> patterns;
    patterns.Fill(board);
    patterns.fprint(stdout);

    who = (who == Black) ? White : Black;
  }

  return 0;
}
