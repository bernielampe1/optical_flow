#ifndef _GAUSSIAN_H_
#define _GAUSSIAN_H_

const float SQRT_2PI = sqrt(2.0 * M_PI);

/* This function creates a 1-D Gaussian function with variance of sigma.  The
   size of the array is allocated to be large enough to contain 3 standard
   deviations given the variance.

   Note: The function is normalized by the integral of the function so that
         the sum of the values returned is 1.  Thereby allowing it to be
         used as a convolution kernel. */
void makeGaussianKernel(const float &sigma, float **k, int &size) {
  int winSize = 1 + 2 * (int)ceil(2.5 * sigma);
  int center = winSize >> 1;
  float sum = 0.0;

  // alloc memory for kernel
  *k = new float[winSize];
  size = winSize;

  // compute the gaussian value for each cell
  for (int i = 0; i < winSize; i++) {
    float x = i - center;
    float fx =
        pow(2.71828, -0.5 * x * x / (sigma * sigma)) / (sigma * SQRT_2PI);
    (*k)[i] = fx;
    sum += fx;
  }

  // normalize the filter so the integral is 1.0
  for (int i = 0; i < winSize; i++) {
    (*k)[i] /= sum;
  }
}

void makeGaussianKernel(const int winSize, float **k) {
  float sigma = winSize / 5.0;
  int center = winSize >> 1;
  float sum = 0.0;

  // alloc memory for kernel
  *k = new float[winSize];

  // compute the gaussian value for each cell
  for (int i = 0; i < winSize; i++) {
    float x = i - center;
    float fx =
        pow(2.71828, -0.5 * x * x / (sigma * sigma)) / (sigma * SQRT_2PI);
    (*k)[i] = fx;
    sum += fx;
  }

  // normalize the filter so the integral is 1.0
  for (int i = 0; i < winSize; i++) {
    (*k)[i] /= sum;
  }
}

#endif // _GAUSSIAN_H_
