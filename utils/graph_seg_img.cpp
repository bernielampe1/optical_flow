#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#include "DisjointSet.h"
#include "Edge.h"
#include "Exception.h"
#include "FileStreamDecoder.h"
#include "Image.h"
#include "gaussian.h"
#include "graphCol.h"
#include "graphGen.h"
#include "graphRed.h"
#include "graphSeg.h"
#include "opticalFlow.h"

int main(int argc, char **argv) {
  vector<Edge_t> edgeVec;
  float *gaussKernel;
  int minSize;
  Image<RGB_t> img1;
  double sigma;
  double threshold;
  DisjointSet<int> universe;
  string imgFname;
  int ksize;

  if (argc != 5) {
    cerr << argv[0] << " <img> <sigma> <threshold> <minSize>" << endl;
    return (1);
  }

  imgFname = argv[1];
  sigma = atof(argv[2]);
  threshold = atof(argv[3]);
  minSize = atoi(argv[4]);

  try {
    img1.readFromFile(imgFname);

    // compute 1-D gaussian convolution kernel
    makeGaussianKernel(sigma, &gaussKernel, ksize);

    // allocate space for edges
    edgeVec.resize(img1.width() * img1.height() * 4);

    // create complete graph from color channels and flow vectors
    Image<float> *mag1 = computeBrightness(&img1);
    mag1->convolve(gaussKernel, ksize);
    createGraph(mag1, edgeVec);

    // segment the graph
    graphSegment(img1.width() * img1.height(), threshold, edgeVec, universe);

    // remove small sets
    graphReduce(edgeVec, minSize, universe);

    // assign unique color to each superpixel
    Image<RGB_t> segImg(img1.height(), img1.width());
    colorSegments(universe, segImg);

    // output the color-segmented frame
    segImg.writeToFile("segImg.ppm");
  } catch (Exception &e) {
    cerr << "Error: " << e.what() << endl;
    return (1);
  } catch (...) {
    cerr << "Error: caught unhandled exception" << endl;
    return (1);
  }

  return (0);
}
