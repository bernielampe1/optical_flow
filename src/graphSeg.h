#ifndef _GRAPHSEG_H_
#define _GRAPHSEG_H_

/* Function that segments the complete, image space graph by merging sets in
   the universe.  Sets are merged based on a comparison of the max of the
   internal set distance and the min of the between set distance.

   Internal set distance is the maximum distance between all pixels in that set.

   The between set distance is the minimum distance between any two pixels
   between the sets.

   Note: As sets increase in size, the internal distance usually grows and
         the final sets represent the partioning of the graph such that the
         internal set distances are minimized relative to the between set
         distances. */
void graphSegment(const int numSets, const double &threshold,
                  vector<Edge_t> &edgeVec, DisjointSet<int> &universe) {
  // sort the edges on weight
  sort(edgeVec.begin(), edgeVec.end());

  // create disjoint sets (one per pixel)
  universe.clear();
  for (int i = 0; i < numSets; i++) {
    universe.make_set(i);
  }

  // initialize all thresholds
  double *threshs = new double[numSets];
  for (int i = 0; i < numSets; i++)
    threshs[i] = threshold;

  // loop over edges in increasing order
  for (unsigned i = 0; i < edgeVec.size(); i++) {
    int s0 = universe.find(edgeVec[i].p0);
    int s1 = universe.find(edgeVec[i].p1);

    if (s0 != s1 && edgeVec[i].w <= threshs[s0] &&
        edgeVec[i].w <= threshs[s1]) {
      universe.join(s0, s1);
      s0 = universe.find(s0);
      threshs[s0] = edgeVec[i].w + threshold / universe.size(s0);
    }
  }

  delete[] threshs;
}

#endif // _GRAPHSEG_H_
