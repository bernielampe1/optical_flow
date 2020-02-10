#ifndef _GRAPHGEN_H_
#define _GRAPHGEN_H_

#include "Edge.h"
#include "Image.h"

/* Function for computing the distance between two pixels. */
inline double euclidDiff(const Image<float> *im, const int p0, const int p1) {
  float d = im->getPixel(p0) - im->getPixel(p1);
  return (sqrt(d * d));
}

/* Function for creating the complete set of edges which represents the
   complete graph consisting of all pixels in an image. */
void createGraph(const Image<float> *im, vector<Edge_t> &edgeVec) {
  int height = im->height();
  int height_1 = im->height() - 1;

  int width = im->width();
  int width_1 = im->width() - 1;

  int edgeInd = 0;

  for (int h = 0; h < height; h++) {
    for (int w = 0; w < width; w++) {
      int p0 = h * width + w;

      if (w < width_1) {
        edgeVec[edgeInd].p0 = p0;
        edgeVec[edgeInd].p1 = h * width + (w + 1);
        edgeVec[edgeInd].w = euclidDiff(im, p0, edgeVec[edgeInd].p1);
        edgeInd++;
      }

      if (h < height_1) {
        edgeVec[edgeInd].p0 = p0;
        edgeVec[edgeInd].p1 = (h + 1) * width + w;
        edgeVec[edgeInd].w = euclidDiff(im, p0, edgeVec[edgeInd].p1);
        edgeInd++;
      }

      if (w < width_1 && h < height_1) {
        edgeVec[edgeInd].p0 = p0;
        edgeVec[edgeInd].p1 = (h + 1) * width + (w + 1);
        edgeVec[edgeInd].w = euclidDiff(im, p0, edgeVec[edgeInd].p1);
        edgeInd++;
      }

      if (w < width_1 && h > 0) {
        edgeVec[edgeInd].p0 = p0;
        edgeVec[edgeInd].p1 = (h - 1) * width + (w + 1);
        edgeVec[edgeInd].w = euclidDiff(im, p0, edgeVec[edgeInd].p1);
        edgeInd++;
      }
    }
  }
}

#endif // _GRAPHGEN_H_
