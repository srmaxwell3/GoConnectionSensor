#include <cassert>
#include <cstdlib>
#include <cstdio>

#include <algorithm>

#include <array>
using std::array;

#include <bitset>
using std::bitset;

#include <set>
using std::set;

#include <utility>
using std::pair;

size_t const bSize = 19;

char const *ARGV0 = "g";

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
  }
  return '?';
}

template<typename T> struct PArray: public array<T, size_t(EoPoint)> {
};

template<typename T, size_t S> struct RArray: public array<T, S> {
};

template<typename T, size_t S> struct BArray: public array<RArray<T, S>, S> {
  T operator()(int row, int col) const {
    return (*this)[row][col];
  }
  T &operator()(int row, int col) {
    return (*this)[row][col];
  }
};

// constexpr size_t sToNByM(size_t s) { return (S + 2) * (S + 1); }
#define sToNByM(S) (((S) + 2) * ((S) + 1))

template<size_t S> struct BoardSet: public bitset<sToNByM(S)> {
  typedef bitset<sToNByM(S)> BitSet;

  BoardSet() {
    BitSet::reset();
  }

  static size_t toIndex(int row, int col) {
    return (size_t(row) + 1) * (S + 1) + size_t(col);
  }

  bool operator()(int row, int col) const {
    return (*this)[toIndex(row, col)];
  }
  typename BitSet::reference operator()(int row, int col) {
    return (*this)[toIndex(row, col)];
  }
};

template<size_t S> class BoardModel: public array<BoardSet<S>, size_t(EoPoint)> {
public:
  BoardModel() {
    reset();
  }

  static size_t toIndex(int row, int col) {
    return BoardSet<S>::toIndex(row, col);
  }

  Point pointAt(int i, int j) const {
    size_t k = toIndex(i, j);

    if ((*this)[Black](i, j)) {
      return Black;
    }
    if ((*this)[White](i, j)) {
      return White;
    }
    if ((*this)[Empty](i, j)) {
      return Empty;
    }
    return Illegal;
  }

  void reset() {
    for (auto p = this->begin(); p != this->end(); p++) {
      p->reset();
    }
    BoardSet<S> &illegals = (*this)[Illegal];
    for (int c = -1; c < S + 1; c += 1) {
      illegals(   -1, c) = 1;
      illegals(S + 1, c) = 1;
    }
    for (int r = 0; r < S + 1; r += 1) {
      illegals(r, -1) = 1;
      illegals(r,  S) = 1;
    }
  }

  void RandomFill(size_t nStones = ((S * S) * 8) / 10) {
    reset();

    int i = rand() % S;
    int j = rand() % S;
    size_t k = toIndex(i, j);

    BoardSet<S> filled;
    Point who = Black;

    fprintf(stdout, "(");

    for (size_t n = 0; n < nStones; n += 1) {
      while (filled(i, j)) {
	int iNew = i + ((rand() % 8) - 4);
	if (iNew < 0 || S <= iNew) {
	  continue;
	}

	int jNew = j + ((rand() % 8) - 4);
	if (jNew < 0 || S <= jNew) {
	  continue;
	}

	i = iNew;
	j = jNew;
      }

      fprintf(stdout,
	      "; %c[%c%c]",
	      who == Black ? 'B' : 'W',
	      'a' + i,
	      'a' + j
	     );

      (*this)[who](i, j) = 1;
      filled(i, j) = 1;

      who = (who == Black) ? White : Black;
    }

    fprintf(stdout, " )\n");

    (*this)[Empty] |= ~filled;
    (*this)[Empty] &= ~(*this)[Illegal];
  }

  void fprint(FILE *out) const {
    fprintf(out, " ");
    for (size_t j = 0; j < S; j += 1) {
      fprintf(out, " %c", char('a' + j));
    }
    fprintf(out, "\n");

    for (size_t i = 0; i < S; i += 1) {
      fprintf(out, "%c", char('a' + i));

      for (size_t j = 0; j < S; j += 1) {
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
    fprintf(out, "%lu%lu%lu%lu", (*this)[Illegal], (*this)[Empty], (*this)[Black], (*this)[White]);
  }
};

template<size_t S> struct BoardNeighborhoodCounts: public BArray<NeighborhoodCounts, S> {
  BoardNeighborhoodCounts() {
    NeighborhoodCounts empty;

    for (size_t i = 0; i < S; i += 1) {
      std::fill((*this)[i].begin(), (*this)[i].end(), empty);
    }
  }

  void Fill(BoardModel<S> const &board) {
    for (int i = 0; i < S; i += 1) {
      for (int j = 0; j < S; j += 1) {
	Point point = board.pointAt(i, j);

	if (0 < i) {
	  (*this)(i - 1, j)[point] += 1;
	}
	if (i < (S - 1)) {
	  (*this)(i + 1, j)[point] += 1;
	}
	if (0 < j) {
	  (*this)(i, j - 1)[point] += 1;
	}
	if (j < (S - 1)) {
	  (*this)(i, j + 1)[point] += 1;
	}

	// (*this)[i - 1][j][point] += 1;
	// (*this)[i + 1][j][point] += 1;
	// (*this)[i][j - 1][point] += 1;
	// (*this)[i][j + 1][point] += 1;
      }
    }

    for (size_t k = 0; k < S; k += 1) {
      (*this)(    0, k)[Illegal] += 1;
      (*this)(    1, k)[board.pointAt(    0, k)] += 1;      
      (*this)(S - 2, k)[board.pointAt(S - 1, k)] += 1;      
      (*this)(S - 1, k)[Illegal] += 1;
      (*this)(k,     0)[Illegal] += 1;
      (*this)(k,     1)[board.pointAt(k,     0)] += 1;      
      (*this)(k, S - 2)[board.pointAt(k, S - 1)] += 1;      
      (*this)(k, S - 1)[Illegal] += 1;
    }
  }

  void fprint(FILE *out) const {
    fprintf(out, " ");
    for (size_t j = 0; j < S; j += 1) {
      fprintf(out, "    %c", char('a' + j));
    }
    fprintf(out, "\n");

    for (size_t i = 0; i < S; i += 1) {
      fprintf(out, "%c", char('a' + i));

      for (size_t j = 0; j < S; j += 1) {
	fprintf(out, " ");
	(*this)[i][j].fprint(out);
      }

      fprintf(out, "\n");
    }

    fprintf(out, "\n");
  }
};

class Group: set<pair<size_t, size_t>> {
public:
  Group(Point const &point):
    groupOf (point)
  {
  }

  void Fill(BoardModel<bSize> const &board, size_t i, size_t j) {
    assert(groupOf == board.pointAt(i, j));

    insert({ i, j });
  }

  void fprint(FILE *out) const {
    fprintf(out, "{");
    auto p = cbegin();
    if (p != cend()) {
      fprintf(out, " %c%c", char('a' + p->first), char('a' + p->second));
      while (++p != cend()) {
	fprintf(out, ", %c%c", char('a' + p->first), char('a' + p->second));
      }
    }
    fprintf(out, " }");
  }

  Point point() const { return groupOf; }

private:
  Point groupOf;
};

class Groups: public PArray<set<Group *>> {
public:
  Groups() {
    for (auto i = pointGroups.begin(); i != pointGroups.end(); i++) {
      std::fill(i->begin(), i->end(), (Group *) 0);
    }
  }

  void Fill(BoardModel<bSize> const &board, Group *group, size_t i, size_t j) {
    assert(group);

    Point point = group->point();

    assert(point == board.pointAt(i, j));
    assert(!pointGroups[i][j]);

    group->Fill(board, i, j);
    pointGroups[i][j] = group;

    if (0 < i) {
      if (!pointGroups[i - 1][j] && point == board.pointAt(i - 1, j)) {
	Fill(board, group, i - 1, j);
      }
    }
    if (i < (bSize - 1)) {
      if (!pointGroups[i + 1][j] && point == board.pointAt(i + 1, j)) {
	Fill(board, group, i + 1, j);
      }
    }
    if (0 < j) {
      if (!pointGroups[i][j - 1] && point == board.pointAt(i, j - 1)) {
	Fill(board, group, i, j - 1);
      }
    }
    if (j < (bSize - 1)) {
      if (!pointGroups[i][j + 1] && point == board.pointAt(i, j + 1)) {
	Fill(board, group, i, j + 1);
      }
    }
  }

  void Fill(BoardModel<bSize> const &board) {
    for (size_t i = 0; i < bSize; i += 1) {
      for (size_t j = 0; j < bSize; j += 1) {
	if (!pointGroups[i][j]) {
	  Point point = board.pointAt(i, j);
	  Group *group = new Group(point);

	  (*this)[point].insert(group);
	  Fill(board, group, i, j);
	}
      }
    }
  }

  void fprint(FILE *out) const {
    fprintf(out, "Groups = {\n");
    for (Point p = Illegal; p < EoPoint; p = Point(size_t(p) + 1)) {
      fprintf(out, "  [%d] {\n", p);
      for (auto g = (*this)[p].begin(); g != (*this)[p].end(); g++) {
	fprintf(out, "    ");
	(*g)->fprint(out);
	fprintf(out, "\n");
      }
      fprintf(out, "  }\n");
    }
    fprintf(out, "}\n");
  }

  BArray<Group *, bSize> pointGroups;
};

int main(int argc, char const *argv[])
{
  ARGV0 = argv[0];

  // BoardModel board;
  BoardModel<19> board;

  board.RandomFill();
  board.fprint(stdout);

  BoardNeighborhoodCounts<19> counts;
  counts.Fill(board);
  counts.fprint(stdout);

  Groups groups;
  groups.Fill(board);
  groups.fprint(stdout);
  return 0;
}
