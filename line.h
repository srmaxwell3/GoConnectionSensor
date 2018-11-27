#ifndef LINE_H
#define LINE_H

#include <cstdio>

#include <vector>
using std::vector;

#include "boardlocation.h"
#include "lineid.h"

template <size_t NRows, size_t NCols> struct Line: public vector<BoardLocation<NRows, NCols>> {
  typedef BoardLocation<NRows, NCols> LocationRC;
  typedef LineId<NRows, NCols> LineIdRC;
  typedef Line<NRows, NCols> LineRC;

  Line() : lineId (), reflections (0) { }

  // Line(Location<NRows, NCols> _src, int x1, int y1) :
  //   src (_src),
  //   dst (_src),
  //   reflections (0)
  // {
  //   // Setup Bresenham's line drawing algorithm.
  //
  //   int vx0 = src.row();
  //   int vy0 = src.col();
  //   int ax0 = vx0;
  //   int ay0 = vy0;
  //
  //   int dx = abs(x1 - vx0);
  //   int vsx = vx0 < x1 ? +1 : -1;
  //   int asx = vsx;
  //   int dy = abs(y1 - vy0);
  //   int vsy = vy0 < y1 ? +1 : -1;
  //   int asy = vsy;
  //   int err = dx - dy;
  //
  //   for (push_back(Location<NRows, NCols>(ax0, ay0));
  // 	 !(vx0 == x1 && vy0 == y1);
  // 	 push_back(Location<NRows, NCols>(ax0, ay0))
  // 	)
  //   {
  //     bool reflected = false;
  //     int e2 = 2 * err;
  //     if (e2 > -dy) {
  // 	err -= dy;
  // 	vx0 += vsx;
  // 	ax0 += asx;
  //
  // 	if (ax0 < 0) {
  // 	  reflected = true;
  // 	  ax0 = -ax0;
  // 	  asx = -asx;
  // 	} else if (NRows <= ax0) {
  // 	  reflected = true;
  // 	  ax0 = NRows - 2;
  // 	  asx = -asx;
  // 	}
  //     }
  //     if (e2 < dx) {
  // 	err += dx;
  // 	vy0 += vsy;
  // 	ay0 += asy;
  //
  // 	if (ay0 < 0) {
  // 	  reflected = true;
  // 	  ay0 = -ay0;
  // 	  asy = -asy;
  // 	} else if (NCols <= ay0) {
  // 	  reflected = true;
  // 	  ay0 = NCols - 2;
  // 	  asy = -asy;
  // 	}
  //     }
  //     if (reflected) {
  // 	reflections += 1;
  //     }
  //   }
  //   dst = this->back();
  // }
  Line(LineIdRC _lineId) :
    lineId(_lineId),
    reflections (0)
  {
    // Setup Bresenham's line drawing algorithm.

    int x0 = lineId.src.row();
    int y0 = lineId.src.col();
    int x1 = lineId.dst.row();
    int y1 = lineId.dst.col();

    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? +1 : -1;
    int dy = abs(y1 - y0);
    int sy = y0 < y1 ? +1 : -1;
    int err = dx - dy;

    for (push_back(LocationRC(x0, y0));
  	 !(x0 == x1 && y0 == y1);
  	 push_back(LocationRC(x0, y0))
  	)
    {
      int e2 = 2 * err;
      if (e2 > -dy) {
  	err -= dy;
  	x0 += sx;
      }
      if (e2 < dx) {
  	err += dx;
  	y0 += sy;
      }
    }
  }
  bool operator==(Line const &that) const {
    return lineId == that.lineId && reflections == that.reflections;
  }
  bool operator<(Line const &that) const {
    return lineId < that.lineId || (lineId == that.lineId && reflections < that.reflections);
  }

  void fprint(FILE *out) const {
    lineId.fprint(out);
    fprintf(out, ":%d:{", reflections);
    auto l = this->cbegin();
    l->fprint(out);
    while (++l != this->cend()) {
      fprintf(out, ",");
      l->fprint(out);
    }
    fprintf(out, "}");
  }

  LineIdRC lineId;
  size_t reflections;
};

#endif // LINE_H
