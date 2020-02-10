#ifndef _EDGE_H_
#define _EDGE_H_

/* Abstraction of a graph edge. */
struct Edge_t {
  double w;   // edge weight
  int p0, p1; // vector indicies of of two pixels

  bool operator<(const Edge_t &rhs) const { return w < rhs.w; }
};

#endif // _EDGE_H_
