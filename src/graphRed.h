#ifndef _GRAPHRED_H_
#define _GRAPHRED_H_

/* This function removes any sets in the universe that are smaller than the
   specified value of minSize.  To remove the sets the pixels in the set
   are merged with adjacent sets of adequate size. */
void graphReduce(const vector<Edge_t> &edgeVec, const int minSize,
                 DisjointSet<int> &universe) {
  for (unsigned i = 0; i < edgeVec.size(); i++) {
    int s0 = universe.find(edgeVec[i].p0);
    int s1 = universe.find(edgeVec[i].p1);

    if (s0 != s1 &&
        (universe.size(s0) < minSize || universe.size(s1) < minSize)) {
      universe.join(s0, s1);
    }
  }
}

#endif // _GRAPHRED_H_
