#ifndef LINEID_H
#define LINEID_H

#include "boardlocation.h"

template <size_t NRows, size_t NCols> struct LineId {
  typedef BoardLocation<NRows, NCols> LocationRC;
  typedef LineId<NRows, NCols> LineIdRC;

  LineId() : src (), dst () { }
  LineId(LocationRC _src, LocationRC _dst) :
    src (_src),
    dst (_dst)
  {
  }
  bool operator==(LineId const &that) const {
    return src == that.src && dst == that.dst;
  }
  bool operator<(LineId const &that) const {
    return src < that.src || (src == that.src && dst < that.dst);
  }

  void fprint(FILE *out) const {
    src.fprint(out);
    fprintf(out, "..");
    dst.fprint(out);
  }

  LocationRC src;
  LocationRC dst;
};

#endif
