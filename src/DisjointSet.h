#ifndef _DISJOINTSET_H_
#define _DISJOINTSET_H_

#include <ext/hash_map>
#include <math.h>

using namespace std;

/* Objects of this class represent a universe of disjoint sets. The element
   type is a template parameter but is restricted by the types that a
   GNU hashmap can store. */

/* Note: Extended template types need to define a hash function for hash_map. */

template <typename T> class DisjointSet {
private:
  /* struct to hold a single set */
  class elem {
  public:
    mutable T parent; // value representative of a set
    int rank;         // heuristic value to compare set sizes
    int size;         // number of elements in a set

    elem() {}
    elem(const T &p, const int r, const int s) : parent(p), rank(r), size(s) {}
  };

  __gnu_cxx::hash_map<T, elem> elts; // map to hold all sets
  int num;                           // number of sets

public:
  DisjointSet() : num(0) {}
  ~DisjointSet() {}

  // clear all sets in universe
  void clear() {
    elts.clear();
    num = 0;
  }

  // get number of sets in universe
  int numSets() const { return (num); }

  // make a new set with value x
  void make_set(const T &x);

  // join two sets in universe with parents x and y
  void join(const T &x, const T &y);

  // find set parent which x belongs
  T &find(const T &x) const;

  // get size of set with parent x
  int size(const T &x) const;
};

#include "DisjointSet.inl"

#endif /* _DISJOINTSET_H_ */
