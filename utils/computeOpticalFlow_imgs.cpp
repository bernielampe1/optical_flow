#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#include "Exception.h"
#include "FileStreamDecoder.h"
#include "Image.h"
#include "gaussian.h"
#include "opticalFlow.h"

int main(int argc, char **argv) {
  Image<float> u, v;
  Image<float> pImg, cImg;
  Image<RGB_t> img1, img2;
  int winSize;
  string img1_str, img2_str;
  float *textimg;
  float *gaussKernel;
  int ksize, num;
  double sigma;

  if (argc != 6) {
    cerr << argv[0] << " <img1.ppm> <img2.ppm> <sigma> <winSize> <num>" << endl;
    return (1);
  }

  img1_str = argv[1];
  img2_str = argv[2];
  sigma = atof(argv[3]);
  winSize = atoi(argv[4]);
  num = atoi(argv[5]);

  try {
    // read images
    img1.readFromFile(img1_str);
    img2.readFromFile(img2_str);

    // create texture map
    textimg = new float[img1.width() * img1.height()];
    for (int i = 0; i < img1.width() * img1.height(); i++) {
      textimg[i] = rand() % 1000;
    }

    // populate the grayscale images
    cImg.init(img1.height(), img1.width());
    pImg.init(img1.height(), img1.width());
    for (int i = 0; i < img1.width() * img1.height(); i++) {
      float v1 =
          0.2989 * img1[i][0] + 0.5870 * img1[i][1] + 0.1140 * img1[i][2];
      float v2 =
          0.2989 * img2[i][0] + 0.5870 * img2[i][1] + 0.1140 * img2[i][2];

      cImg.setPixel(i, v1);
      pImg.setPixel(i, v2);
    }

    // compute 1-D gaussian convolution kernel
    makeGaussianKernel(sigma, &gaussKernel, ksize);

    // convolve images
    cImg.convolve(gaussKernel, ksize);
    pImg.convolve(gaussKernel, ksize);

    // info
    cerr << " * computing optical flow vectors" << endl;

    // compute the optical flow between these two frames
    computeOpticalFlow_HLK(&pImg, &cImg, winSize, &u, &v);

    // create vector field for output format
    Image<Vec2f_t> oflow(u.height(), u.width());
    Vec2f_t vp;
    for (int i = 0; i < u.height() * u.width(); i++) {
      vp[0] = u[i];
      vp[1] = v[i];
      oflow[i] = vp;
    }

    // output the optical flow vectors
    {
      ostringstream oss;
      oss << "oflow_" << num << ".ppm";
      oflow.writeToFile(oss.str(), textimg);
    }

    {
      ostringstream oss;
      oss << "ovecs_" << num << ".pgm";
      oflow.writeToFile(oss.str());
    }

    // release texture image
    delete[] textimg;
  } catch (Exception &e) {
    cerr << "Error: " << e.what() << endl;
    return (1);
  } catch (...) {
    cerr << "Error: caught unhandled exception" << endl;
    return (1);
  }

  return (0);
}
