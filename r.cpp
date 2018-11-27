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

#include <cstdio>

#include <vector>
using std::vector;

struct Point {
  int x;
  int y;
  void fprint(FILE *out) const {
    fprintf(out, "{%+3d,%+3d}", i, j);
  }
};

void line(int x1, int y1)
{
  vector<Point> points;

  int x0 = 0;
  int y0 = 0;

  int dx = abs(x1 - x0);
  int sx = x0 < x1 ? +1 : -1;
  int dy = abs(y1 - y0);
  int sy = y0 < y1 ? +1 : -1;
  int err = dx - dy;

  for (points.push_back({ x0, y0 }); x0 != x1 && y0 != y1; points.push_back({ x0, y0 })) {
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

  fprintf(stdout, "{");
  if (!points.empty()) {
    points[0].fprint(stdout);
    for (size_t p = 1; p < points.size(); p += 1) {
      fprintf(stdout, ",");
      points[p].fprint(stdout);
    }
  }
  fprintf(stdout, "}\n");
}

int main(int argc, char const *argv[])
{
  for (int i = -18; i < +18; i += 1) {
    line(i, +18);
  }
  for (int j = +18; -18 < j; j -= 1) {
    line(+18, j);
  }
  for (int i = +18; -18 < i; i -= 1) {
    line(i, -18);
  }
  for (int j = -18; j < +18; j += 1) {
    line(-18, j);
  }
  return 0;
}
