// function line(x0, y0, x1, y1)
//    dx := abs(x1-x0)
//    dy := abs(y1-y0) 
//    if x0 < x1 then sx := 1 else sx := -1
//    if y0 < y1 then sy := 1 else sy := -1
//    err := dx-dy
//  
//    loop
//      setPixel(x0,y0)
//      if x0 = x1 and y0 = y1 exit loop
//      e2 := 2*err
//      if e2 > -dy then 
//        err := err - dy
//        x0 := x0 + sx
//      end if
//      if e2 <  dx then 
//        err := err + dx
//        y0 := y0 + sy 
//      end if
//    end loop

#include <cassert>
#include <cstdlib>
#include <cstdio>

#include <set>
using std::set;

#include <vector>
using std::vector;

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
    return !(0 < row() && row() < (NRows - 1) && 0 < col() && col() < (NCols - 1));
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
    case EoDirection:
      return false;
    }
  }
  BoardLocation locationToThe(Direction const &d) const {
    return BoardLocation(offset + (!isOnAnEdgeToThe(d) ? offsetToThe(d) : 0));
  }
  bool operator==(BoardLocation const &that) const { return offset == that.offset; }
  bool operator<(BoardLocation const &that) const { return offset < that.offset; }
  BoardLocation &operator+=(int by) {
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
  BoardLocation &operator-=(int by) {
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

template <size_t NRows, size_t NCols> struct Line: public vector<BoardLocation<NRows, NCols>> {
  typedef vector<BoardLocation<NRows, NCols>> VectorOfBoardLocations;

  // Line(BoardLocation<NRows, NCols> _src, int x1, int y1) :
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
  //   for (push_back(BoardLocation<NRows, NCols>(ax0, ay0));
  // 	 !(vx0 == x1 && vy0 == y1);
  // 	 push_back(BoardLocation<NRows, NCols>(ax0, ay0))
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
  Line(BoardLocation<NRows, NCols> _src, BoardLocation<NRows, NCols> _dst) :
    src (_src),
    dst (_dst),
    reflections (0)
  {
    // Setup Bresenham's line drawing algorithm.
  
    int x0 = src.row();
    int y0 = src.col();
    int x1 = dst.row();
    int y1 = dst.col();
  
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? +1 : -1;
    int dy = abs(y1 - y0);
    int sy = y0 < y1 ? +1 : -1;
    int err = dx - dy;
  
    for (VectorOfBoardLocations::push_back(BoardLocation<NRows, NCols>(x0, y0));
  	 !(x0 == x1 && y0 == y1);
  	 VectorOfBoardLocations::push_back(BoardLocation<NRows, NCols>(x0, y0))
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
    return
      src == that.src &&
      dst == that.dst &&
      reflections == that.reflections;
  }
  bool operator<(Line const &that) const {
    return
      src < that.src ||
      (src == that.src &&
       (dst < that.dst ||
	(dst == that.dst &&
	 reflections < that.reflections
	)
       )
      );
  }

  void fprint(FILE *out) const {
    src.fprint(out);
    fprintf(out, "..");
    dst.fprint(out);
    fprintf(out, ":%d:{", reflections);
    auto l = this->cbegin();
    l->fprint(out);
    while (++l != this->cend()) {
      fprintf(out, ",");
      l->fprint(out);
    }
    fprintf(out, "}");
  }

  BoardLocation<NRows, NCols> src;
  BoardLocation<NRows, NCols> dst;
  size_t reflections;
};

typedef BoardLocation<19, 19> BoardLocation19x19;
typedef Line<19, 19> Line19x19;
typedef set<Line19x19> SetOfLine19x19;
typedef vector<SetOfLine19x19> VectorOfSetOfLine19x19;

int main(int argc, char const *argv[])
{
  SetOfLine19x19 allLines;

  for (size_t i = 0; i < 19; i += 1) {
    for (size_t j = 0; j < 19; j += 1) {
      BoardLocation<19, 19> src(i, j);

      // for (int di = -18; di <= 18; di += 1) {	
      // 	for (int dj = -18; dj <= 18; dj += 1) {	
      // 	  allLines.insert(Line<19, 19>(src, di, dj));
      // 	}
      // }

      for (size_t ii = 0; ii < 19; ii += 1) {
      	for (size_t jj = 0; jj < 19; jj += 1) {
      	  BoardLocation<19, 19> dst(ii, jj);
      
      	  if (src != dst) {
      	    allLines.insert(Line<19, 19>(src, dst));
      	  }
      	}
      }
    }
  }

  VectorOfSetOfLine19x19 board(BoardLocation19x19::size);

  for (auto line = allLines.cbegin(); line != allLines.cend(); line++) {
    for (auto location = line->cbegin(); location != line->cend(); location += 1) {
      board[*location].insert(*line);
    }
  }

  fprintf(stdout, " ");
  for (size_t j = 0; j < 19; j += 1) {
    fprintf(stdout, "     %c", char(j + 'a'));
  }
  fprintf(stdout, "\n");
  for (size_t i = 0; i < 19; i += 1) {
    fprintf(stdout, "%c", char(i + 'a'));

    for (size_t j = 0; j < 19; j += 1) {
      BoardLocation19x19 location(i, j);

      fprintf(stdout, " %5lu", board[size_t(location)].size());
    }
    fprintf(stdout, "\n");
  }

  return 0;
}
