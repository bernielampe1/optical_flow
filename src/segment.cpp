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
  unsigned minSize;
  double sigma;
  double threshold;
  string vidFname;
  unsigned tsteps;
  unsigned winSize;

  if (argc != 7) {
    cerr << argv[0] << " <video stream file> <sigma> <winSize> <tsteps>"
         << " <threshold> <minSize>" << endl;
    return (1);
  }

  vidFname = argv[1];
  sigma = atof(argv[2]);
  winSize = atoi(argv[3]);
  tsteps = atoi(argv[4]);
  threshold = atof(argv[5]);
  minSize = atoi(argv[6]);

  try {
    // intialize the video stream
    cerr << " * initializing video stream decoder" << endl;
    FileStreamDecoder streamObj(vidFname);

    // get all the frames
    cerr << " * decoding video stream" << endl;
    Image<RGB_t> *cFrame;
    vector<Image<RGB_t> *> frames;
    do {
      cFrame = streamObj.getFrame();
      if (cFrame) {
        frames.push_back(cFrame);
      }
    } while (cFrame);

    // info
    cerr << " * decoded " << frames.size() << " video frames" << endl;

    // compute 1-D gaussian convolution kernel
    int gaussSize;
    float *gaussKernel;
    makeGaussianKernel(sigma, &gaussKernel, gaussSize);

    // vectors to store optical flow estimates
    vector<Image<float> > dus;
    vector<Image<float> > dvs;

    // info
    cerr << " * computing optical flow vectors" << endl;

    // initialize the brightness image
    Image<float> *cImg = computeBrightness(frames[0]);
    cImg->convolve(gaussKernel, gaussSize);

    // get reference dimensions
    int height = cImg->height();
    int width = cImg->width();

    // loop over frames two at a time
    unsigned frameNum = 1;
    do {
      // info
      cerr << "   -- frame " << frameNum << endl;

      // update the previous brightness pointer
      Image<float> *pImg = cImg;

      // update the current brightness pointer
      cImg = computeBrightness(frames[frameNum]);
      cImg->convolve(gaussKernel, gaussSize);

      // compute the optical flow between these two frames
      Image<float> u, v;
      computeOpticalFlow_HLK(pImg, cImg, winSize, &u, &v);

      // save optical flow estimates
      dus.push_back(u);
      dvs.push_back(v);

      frameNum++;  // go to next frame
      delete pImg; // release prior brightness image
    } while (frameNum < frames.size());

    // release final image
    delete cImg;

    // release the guassian filter
    delete[] gaussKernel;

    // release frames
    for (unsigned i = 0; i < frames.size(); i++)
      delete frames[i];

    // info
    cerr << " * integrating frames in window size " << tsteps << endl;

    // coherenetly integrate up of the optical flow estimates over window
    // and compute squared magnitude
    vector<Image<float> > sqmags;
    for (unsigned i = 0; i < dus.size() - tsteps; i++) {
      Image<float> usum(height, width), vsum(height, width);
      for (unsigned j = 0; j < tsteps; j++) {
        usum = usum + dus[i + j];
        vsum = vsum + dvs[i + j];
      }

      usum = usum * (1.0 / tsteps);
      vsum = vsum * (1.0 / tsteps);

      sqmags.push_back(usum * usum + vsum * vsum);
    }

    // initialize the edges for graph segmentation
    vector<Edge_t> edgeVec(height * width * 4);

    // initialize memory for segmented image
    Image<RGB_t> segImg(height, width);

    // info
    cerr << " * segmenting integrated images" << endl;

    // loop over integrated images
    for (unsigned i = 0; i < sqmags.size(); i++) {
      // create the complete graph
      createGraph(&sqmags[i], edgeVec);

      // segment graph
      DisjointSet<int> universe;
      graphSegment(height * width, threshold, edgeVec, universe);

      // remove small sets
      graphReduce(edgeVec, minSize, universe);

      // color graph
      colorSegments(universe, segImg);

      // write segmented image to file
      {
        ostringstream oss;
        oss << "seg_" << i << ".ppm";
        segImg.writeToFile(oss.str());
      }
    }
  } catch (Exception &e) {
    cerr << "Error: " << e.what() << endl;
    return (1);
  } catch (...) {
    cerr << "Error: caught unhandled exception" << endl;
    return (1);
  }

  return (0);
}
