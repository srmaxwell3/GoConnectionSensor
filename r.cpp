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

#include <algorithm>
using std::find;

#include <map>
using std::map;

#include <vector>
using std::vector;

struct Point {
  Point(int _x, int _y) :
    x (_x),
    y (_y)
  {
  }
  bool operator==(Point const &that) const {
    return x == that.x && y == that.y;
  }
  bool operator<(Point const &that) const {
    return x < that.x || (x == that.x && y < that.y);
  }

  int x;
  int y;
  void fprint(FILE *out) const {
    fprintf(out, "{%+3d,%+3d}", x, y);
  }
};

struct PointTree: public Point {
  PointTree(Point const &that) :
    Point (that),
    parent (0)
  {
  }

  PointTree *hasChild(Point const &point) {
    for (auto c = children.begin(); c != children.end(); c++) {
      if (**c == point) {
	return *c;
      }
    }
    return 0;
  }

  void addChild(PointTree *c) {
    children.push_back(c);
    c->parent = this;
  }

  size_t numberOfForks() const {
    size_t nForks = 1 < children.size() ? children.size() - 1 : 0;

    for (auto c = children.cbegin(); c != children.cend(); c++) {
      nForks += (*c)->numberOfForks();
    }

    return nForks;
  }

  void fprint(FILE *out, size_t depth = 0) const {
    for (size_t d = 0; d < depth; d += 1) {
      fprintf(out, ".   ");
    }
    Point::fprint(out);
    fprintf(out, "\n");
    for (auto c = children.cbegin(); c != children.cend(); c++) {
      (*c)->fprint(out, depth + 1);
    }
  }

  // void fprintNested(FILE *out, size_t length = 0) const {
  //   Point::fprint(out);
  // 
  //   if (children.size()) {
  //     fprintf(stdout, ",");
  // 
  //     if (size_t nForks = numberOfForks()) {
  //     	fprintf(stdout, "{");
  //     
  //     	auto c = children.cbegin();
  //     	(*c)->fprintNested(out, 0);
  //     
  //     	for (; c != children.cend(); c++) {
  //     	  fprintf(stdout, ",");
  //     	  (*c)->fprintNested(out, 0);
  //     	}
  //     	fprintf(stdout, "}\n");
  //     } else if (!children.empty()) {
  //     	assert(children.size() == 1);
  //     
  //     	children[0]->fprintNested(out, length + 1);
  //     }
  //   } else {
  //     fprintf(stdout, "\n");
  //   }
  // }

  size_t fprintNested(FILE *out, size_t nFwForks = 0) const {
    size_t nBkForks = 0;
    switch (children.size()) {
    case 0:
      Point::fprint(out);
      return 0;
    case 1:
      nBkForks = children[0]->fprintNested(out, nFwForks);
      fprintf(stdout, ",");
      Point::fprint(out);
      return 0;
    default:
      fprintf(stdout, "{");
      auto c = children.cbegin();
      nBkForks = (*c)->fprintNested(out, nFwForks + 1);
      for (; c != children.end(); c++) {
	fprintf(stdout, ",\n");
	nBkForks += (*c)->fprintNested(out, nFwForks + 1);
      }
      Point::fprint(out);
      fprintf(stdout, "}\n");
      return children.size() - 1;
    }
  }

  PointTree *parent;
  vector<PointTree *> children;
};

void fprint(FILE *out, vector<Point> const &line)
{
  fprintf(stdout, "{");
  if (!line.empty()) {
    line[0].fprint(stdout);
    for (size_t p = 1; p < line.size(); p += 1) {
      fprintf(stdout, ",");
      line[p].fprint(stdout);
    }
  }
  fprintf(stdout, "}\n");
}

void drawLineTo(vector<Point> &line, int x1, int y1)
{
  // fprintf(stdout, "drawLineTo({0,0}..{%+3d,%+3d}): ", x1, y1);

  int x0 = 0;
  int y0 = 0;

  int dx = abs(x1 - x0);
  int sx = x0 < x1 ? +1 : -1;
  int dy = abs(y1 - y0);
  int sy = y0 < y1 ? +1 : -1;
  int err = dx - dy;

  for (line.push_back(Point(x0, y0)); !(x0 == x1 && y0 == y1); line.push_back(Point(x0, y0))) {
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

size_t adjustIndex(size_t N, int dN)
{
  int n = int(N) + dN;
  while (n < 0) {
    n += int(N);
  }
  return size_t(n % int(N));
}

int main(int argc, char const *argv[])
{
  vector<vector<Point>> lines;

  for (int i = -18; i < +18; i += 1) {
    lines.push_back(vector<Point>());
    drawLineTo(lines.back(), i, +18);
  }
  for (int j = +18; -18 < j; j -= 1) {
    lines.push_back(vector<Point>());
    drawLineTo(lines.back(), +18, j);
  }
  for (int i = +18; -18 < i; i -= 1) {
    lines.push_back(vector<Point>());
    drawLineTo(lines.back(), i, -18);
  }
  for (int j = -18; j < +18; j += 1) {
    lines.push_back(vector<Point>());
    drawLineTo(lines.back(), -18, j);
  }

  size_t const nLines = lines.size();

  PointTree *root = new PointTree(Point(0, 0));

  for (size_t l = 0; l < lines.size(); l += 1) {
    vector<Point> const &line = lines[l];

    PointTree *pt = root;

    for (size_t p = 1; p < line.size(); p += 1) {
      Point const &point = line[p];

      if (PointTree *ct = pt->hasChild(point)) {
	pt = ct;
	continue;
      }

      PointTree *ct = new PointTree(point);
      pt->addChild(ct);
      pt = ct;
    }
  }

  root->fprint(stdout);
  // root->fprintNested(stdout);

  // map<Point, size_t> pointCounts;

  // for (auto l = lines.cbegin(); l != lines.cend(); l++) {
  //   for (auto p = l->cbegin(); p != l->cend(); p++) {
  //     pointCounts[*p] += 1;
  //   }
  // }

  // for (size_t l = 0; l < lines.size(); l += 1) {
  //   vector<Point> const &line = lines[l];
  // 
  //   fprintf(stdout, "{");
  // 
  //   line[0].fprint(stdout);
  //   for (size_t p = 1; p < line.size(); p += 1) {
  //     Point const &point = line[p];
  // 
  //     fprintf(stdout, ",");
  //     point.fprint(stdout);
  //     fprintf(stdout, "(%2d)", pointCounts[point]);
  //   }
  // 
  //   fprintf(stdout, "}\n");
  // }

  // size_t l = 0;
  // while (l < lines.size()) {
  //   fprint(stdout, lines[l]);
  // 
  //   size_t ll = adjustIndex(nLines, l + 1);
  //   while (lines[l][1] == lines[ll][1]) {
  //     fprintf(stdout, "{         "); // lines[l][0]
  //     fprintf(stdout, ",         "); // lines[l][1]
  // 
  //     size_t p = 2;
  //     while (p < lines[ll].size() && lines[l][p] == lines[ll][p]) {
  // 	// fprintf(stdout, ",{-18,-18}");
  // 	fprintf(stdout, ",         ");
  // 	p += 1;
  //     }
  // 
  //     if (p < lines[ll].size()) {
  // 	fprintf(stdout, ",");
  // 	while (p < lines[ll].size()) {
  // 	  lines[ll][p].fprint(stdout);
  // 	  p += 1;
  // 	}
  //     }
  // 
  //     fprintf(stdout, "\n");
  //     ll += 1;
  //   }
  // 
  //   l += ll;
  // }
  return 0;
}
