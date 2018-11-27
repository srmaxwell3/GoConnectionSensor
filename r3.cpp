#include <cassert>
#include <cstdlib>
#include <cstdio>

#include <map>
using std::map;

#include <set>
using std::set;

#include <vector>
using std::vector;

size_t const NRows = 9;
size_t const NCols = 9;

#include "boardlocation.h"
#include "lineid.h"
#include "line.h"

typedef BoardLocation<NRows, NCols> BoardLocationRxC;
typedef Line<NRows, NCols> LineRxC;
typedef LineId<NRows, NCols> LineIdRxC;
typedef map<LineIdRxC, LineRxC> MapOfIdToLineRxC;
typedef set<LineIdRxC> SetOfLineIdRxC;
typedef vector<SetOfLineIdRxC> VectorOfSetOfLineIdRxC;

int main(int argc, char const *argv[])
{
  MapOfIdToLineRxC allLines;

  for (size_t i = 0; i < NRows; i += 1) {
    for (size_t j = 0; j < NCols; j += 1) {
      BoardLocationRxC src(i, j);

      // for (int di = -18; di <= 18; di += 1) {
      // 	for (int dj = -18; dj <= 18; dj += 1) {
      // 	  allLines.insert(Line<NRows, NCols>(src, di, dj));
      // 	}
      // }

      for (size_t ii = 0; ii < NRows; ii += 1) {
      	for (size_t jj = 0; jj < NCols; jj += 1) {
      	  BoardLocationRxC dst(ii, jj);

      	  if (src != dst) {
	    LineIdRxC lineId(src, dst);
      	    allLines[lineId] = LineRxC(lineId);
      	  }
      	}
      }
    }
  }

  VectorOfSetOfLineIdRxC board(BoardLocationRxC::size);

  for (auto idAndLine = allLines.cbegin(); idAndLine != allLines.cend(); idAndLine++) {
    LineIdRxC const &id = idAndLine->first;
    LineRxC const &line = idAndLine->second;
    for (auto location = line.cbegin(); location != line.cend(); location += 1) {
      board[*location].insert(id);
    }
  }

  fprintf(stdout, " ");
  for (size_t j = 0; j < NCols; j += 1) {
    fprintf(stdout, "     %c", char(j + 'a'));
  }
  fprintf(stdout, "\n");
  for (size_t i = 0; i < NRows; i += 1) {
    fprintf(stdout, "%c", char(i + 'a'));

    for (size_t j = 0; j < NCols; j += 1) {
      BoardLocationRxC location(i, j);

      fprintf(stdout, " %5lu", board[size_t(location)].size());
    }
    fprintf(stdout, "\n");
  }

  for (size_t i = 0; i < ((NRows * NCols) * 5) / 8; i += 1) {
    BoardLocationRxC p(size_t(rand() % (NRows * NCols)));
    SetOfLineIdRxC pLineIds = board[size_t(p)];

    for (auto lId = pLineIds.begin(); lId != pLineIds.end(); lId++) {
      if (p != lId->src && p != lId->dst) {
	LineRxC const &line = allLines[*lId];

	for (auto l = line.cbegin(); l != line.cend(); l += 1) {
	  board[size_t(*l)].erase(*lId);
	}
      }
    }

    fprintf(stdout, " ");
    for (size_t j = 0; j < NCols; j += 1) {
      fprintf(stdout, "     %c", char(j + 'a'));
    }
    fprintf(stdout, "\n");
    for (size_t i = 0; i < NRows; i += 1) {
      fprintf(stdout, "%c", char(i + 'a'));

      for (size_t j = 0; j < NCols; j += 1) {
	BoardLocationRxC location(i, j);

	fprintf(stdout, " %5lu", board[size_t(location)].size());
      }
      fprintf(stdout, "\n");
    }

  }

  return 0;
}
