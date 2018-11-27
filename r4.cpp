#include <cassert>
#include <cstdlib>
#include <cstdio>

#include "rarray.h"

#include <map>
using std::map;

#include <set>
using std::set;

#include <vector>
using std::vector;

size_t const NRows = 9;
size_t const NCols = 9;

#include "line.h"

enum State {
  Empty,
  Black,
  White,

  EoState
};

template<size_t NRows, size_t NCols> class Intersection : public set<LineId<NRows, NCols>> {
public:
  typedef BoardLocation<NRows, NCols> LocationRC;
  typedef Line<NRows, NCols> LineRC;
  typedef LineId<NRows, NCols> LineIdRC;

  Intersection() :
    state (Empty)
  {
  }

  bool is(State s) const {
    return state == s;
  }

  void put(State s) {
    state = s;
  }

private:
  State state;
};

template<size_t NRows, size_t NCols> class Board : public rarray<Intersection<NRows, NCols>, NRows, NCols> {
public:
  typedef BoardLocation<NRows, NCols> LocationRC;
  typedef LineId<NRows, NCols> LineIdRC;
  typedef Line<NRows, NCols> LineRC;
  typedef Intersection<NRows, NCols> IntersectionRC;

  Board() {

    // Find all the possible connections between all intersections...

    for (size_t i = 0; i < NRows; i += 1) {
      for (size_t j = 0; j < NCols; j += 1) {
	LocationRC src(i, j);

	for (size_t ii = 0; ii < NRows; ii += 1) {
	  for (size_t jj = 0; jj < NCols; jj += 1) {
	    LocationRC dst(ii, jj);

	    if (src != dst) {
	      LineIdRC lineId(src, dst);
	      LineRC const *line = new LineRC(lineId);

	      // ... save them in allLines, mapped by their endpoints,...

	      allLines[lineId] = line;

	      // ... and remember, the lines in each touched intersection.

	      for (auto l = line->cbegin(); l != line->cend(); ++l) {
		(*this)[*l].insert(lineId);
	      }
	    }
	  }
	}
      }
    }
  }

  void put(LocationRC l, State s) {
    fprintf(stdout, "Board::put(l=");
    l.fprint(stdout);
    fprintf(stdout, ", s=%s) ", s == Black ? "Black" : (s == White) ? "White" : "Empty");
    fflush(stdout);

    char const *comma1 = "{";

    if ((*this)[l].is(Empty)) {
      IntersectionRC &p = (*this)[size_t(l)];

      p.put(s);

      for (auto lineId : p) {
	fprintf(stdout, "%s ", comma1);
	lineId.fprint(stdout);
	fflush(stdout);
	comma1 = ",";

	if (l != lineId.src && l != lineId.dst) {
	  auto &line = allLines[lineId];

	  char const *comma2 = " {";
	  for (auto const &l : *line) {
	    fprintf(stdout, "%s ", comma2);
	    l.fprint(stdout);
	    fflush(stdout);
	    comma2 = ",";

	    (*this)[l].erase(lineId);
	  }
	  fprintf(stdout, " }");
	  fflush(stdout);
	}
      }

      fprintf(stdout, " }");
      fflush(stdout);
    }
    fprintf(stdout, "\n");
    fflush(stdout);
  }

  ~Board() {
    // for (auto i = allLines.begin(); allLines.end(); i++) {
    //   delete i->second;
    // }
  }

  void fprint(FILE *out) const {
    fprintf(stdout, " ");
    for (size_t j = 0; j < NCols; j += 1) {
      fprintf(stdout, "       %c", char(j + 'a'));
    }
    fprintf(stdout, "\n");
    for (size_t i = 0; i < NRows; i += 1) {
      fprintf(stdout, "%c", char(i + 'a'));

      for (size_t j = 0; j < NCols; j += 1) {
	LocationRC l(i, j);
	IntersectionRC const &p = (*this)[size_t(l)];

	fprintf(stdout,
		" %5lu %c",
		p.size(),
		p.is(Black) ? '@' : (p.is(White) ? 'O' : '.')
	       );
      }
      fprintf(stdout, "\n");
    }
    for (size_t i = 0; i < NRows; i += 1) {
      for (size_t j = 0; j < NCols; j += 1) {
	LocationRC l(i, j);
	IntersectionRC const &p = (*this)[size_t(l)];

	fprintf(stdout, "Board[");
	l.fprint(stdout);
	fprintf(stdout, "] = { state=%s, { ", p.is(Black) ? "Black" : (p.is(White) ? "White" : "Empty"));
	auto lineId = p.cbegin();
	if (lineId != p.cend()) {
	  lineId->fprint(stdout);
	  for (++lineId; lineId != p.cend(); ++lineId) {
	    fprintf(stdout, ", ");
	    lineId->fprint(stdout);
	  }
	}
	fprintf(stdout, " }\n");
      }
      fprintf(stdout, "\n");
    }
  }

private:
  map<LineIdRC, LineRC const *> allLines;
};

typedef Board<NRows, NCols> BoardRC;
typedef BoardLocation<NRows, NCols> LocationRC;
typedef LineId<NRows, NCols> LineIdRC;
typedef Line<NRows, NCols> LineRC;
typedef Intersection<NRows, NCols> IntersectionRC;

int main(int argc, char const *argv[])
{
  BoardRC board;

  board.fprint(stdout);

  State who = Black;
  for (size_t i = 0; i < ((NRows * NCols) * 5) / 8; i += 1) {
    int r = rand();
    size_t m = r % NRows;
    size_t n = (r / NRows) % NCols;

    while (!board.at(m, n).is(Empty)) {
      r = rand();
      m = r % NRows;
      n = (r / NRows) % NCols;
    }

    LocationRC p(m, n);

    fprintf(stdout, "%lu: ", i);
    p.fprint(stdout);
    fprintf(stdout, " <- %s\n", who == Black ? "Black" : "White");

    board.put(p, who);

    board.fprint(stdout);
    fprintf(stdout, "\n");

    who = who == Black ? White : Black;
  }

  return 0;
}
