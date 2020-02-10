#ifndef _OPTICAL_FLOW_H_
#define _OPTICAL_FLOW_H_

#include "Image.h"

// number of iterations for Horn and Schunk algorithm
const int NUM_ITERS = 20;

// number of levels for gaussian pyramid
const int NUM_LEVELS = 3;

// kernels for computer derviatives in x,y,t dims.
const float kx_22[4] = {-0.25, 0.25, -0.25, 0.25};

const float ky_22[4] = {-0.25, -0.25, 0.25, 0.25};

const float kt_22[4] = {0.25, 0.25, 0.25, 0.25};

const float nkt_22[4] = {-0.25, -0.25, -0.25, -0.25};

// kernel for computing laplacian as weighted average of neighbors
const float lap_33[9] = {1.0 / 12.0, 1.0 / 6.0,  1.0 / 12.0, 1.0 / 6.0, 0.0,
                         1.0 / 6.0,  1.0 / 12.0, 1.0 / 6.0,  1.0 / 12.0};

/* Horn and Schunck iterative optical flow.  This algorithm assumes the
   vector field is differentiable. */
void computeOpticalFlow_HS(const Image<float> *pImg, const Image<float> *cImg,
                           const double &alpha, Image<Vec2f_t> *pflow,
                           Image<Vec2f_t> *oflow) {
  float avgu, avgv, ex, ey, et, d;
  Image<float> t0, t1, *du, *dv;
  int height = pImg->height();
  int width = pImg->width();
  Vec2f_t vp;

  // compute derivatives for this frame
  t0 = *pImg;
  t1 = *cImg;               // copy images
  t0.convolve(kx_22, 2, 2); // compute dx
  t1.convolve(kx_22, 2, 2); // compute dx
  Image<float> dx = t0 + t1;

  t0 = *pImg;
  t1 = *cImg;               // copy images
  t0.convolve(ky_22, 2, 2); // compute dy
  t1.convolve(ky_22, 2, 2); // compute dy
  Image<float> dy = t0 + t1;

  t0 = *pImg;
  t1 = *cImg;                // copy images
  t0.convolve(kt_22, 2, 2);  // compute dt
  t1.convolve(nkt_22, 2, 2); // compute dt
  Image<float> dt = t0 + t1;

  // loop so answer converges
  for (int i = 0; i < NUM_ITERS; i++) {
    // get vector components from previous iteration
    du = getChannel(pflow, 0);
    dv = getChannel(pflow, 1);

    // compute laplacian using local average
    du->convolve(lap_33, 3, 3);
    dv->convolve(lap_33, 3, 3);

    // compute optical flow field
    for (int h = 0; h < height; h++) {
      for (int w = 0; w < width; w++) {
        int ind = h * width + w; // index into flat pixel array

        // index to get values for calculation
        ex = dx[ind];
        ey = dy[ind];
        et = dt[ind];
        avgu = du->getPixel(ind);
        avgv = dv->getPixel(ind);

        // factor d is common to both du and dv estimation
        d = (ex * avgu + ey * avgv + et) / (alpha * alpha + ex * ex + ey * ey);

        // estimate du, dv as difference between laplacian and current estimate
        vp[0] = avgu - ex * d;
        vp[1] = avgv - ey * d;

        // update vector estimate
        oflow->setPixel(ind, vp);
      }
    }

    // free vector component images for next iteration
    delete du;
    delete dv;

    if (i < NUM_ITERS - 1) // don't swap on last iteration
    {
      // swap pointers for oflow and pflow for next iteration
      Image<Vec2f_t> *p = pflow;
      pflow = oflow;
      oflow = p;
    }
  }
}

/* Lucas and Kanade optical flow algorithm.  This algorithm assumes the optical
   flow is uniform among neighbors. */
void computeOpticalFlow_LK(const Image<float> *pImg, const Image<float> *cImg,
                           const Image<float> *u0, const Image<float> *v0,
                           const int &winSize, Image<float> *u,
                           Image<float> *v) {
  int height = pImg->height();
  int width = pImg->width();
  Vec2f_t vp;

  // compute derivatives for pImg
  Image<float> pImgDx = *pImg;
  pImgDx.convolve(kx_22, 2, 2); // compute dx

  Image<float> pImgDy = *pImg;
  pImgDy.convolve(ky_22, 2, 2); // compute dy

  Image<float> pImgDt = *pImg;
  pImgDt.convolve(kt_22, 2, 2); // compute dt

  // compute derivatives for cImg
  Image<float> cImgDx = *cImg;
  cImgDx.convolve(kx_22, 2, 2); // compute dx

  Image<float> cImgDy = *cImg;
  cImgDy.convolve(ky_22, 2, 2); // compute dy

  Image<float> cImgDt = *cImg;
  cImgDt.convolve(nkt_22, 2, 2); // compute dt

  // make uniform kernel
  float *k = new float[winSize * winSize];
  for (int i = 0; i < winSize * winSize; i++) {
    k[i] = 1.0;
  }

  // compute derivatives for cImg using displacements if available
  Image<float> dx_2, dy_2, dxy, dxt, dyt;
  if (u0 && v0) {
    // compute derivaties for cImg using displacements
    Image<float> pdx, pdy, pdt;
    int winSize_2 = winSize / 2;

    // initialize output images
    dx_2.init(height, width);
    dy_2.init(height, width);
    dxy.init(height, width);
    dxt.init(height, width);
    dyt.init(height, width);

    for (int h = 0; h < height; h++) {
      for (int w = 0; w < width; w++) {
        // initialize kernels to zeros
        pdx.init(winSize, winSize);
        pdy.init(winSize, winSize);
        pdt.init(winSize, winSize);

        // loop over kernel
        for (int i = 0; i < winSize; i++) {
          for (int j = 0; j < winSize; j++) {
            int ip = h + i - winSize_2;
            int jp = w + j - winSize_2;

            int dip = ip + v0->getPixel(h * width + w);
            int djp = jp + u0->getPixel(h * width + w);

            if (ip >= 0 && dip >= 0 && jp >= 0 && djp >= 0 && ip < height &&
                dip < height && jp < width && djp < width) {
              pdx[i * winSize + j] =
                  pImgDx[ip * width + jp] + cImgDx[dip * width + djp];

              pdy[i * winSize + j] =
                  pImgDy[ip * width + jp] + cImgDy[dip * width + djp];

              pdt[i * winSize + j] =
                  pImgDt[ip * width + jp] + cImgDt[dip * width + djp];
            }
          }
        }

        // compute quadratic functions of the the derivative estimates
        Image<float> t_dx_2 = pdx * pdx;
        Image<float> t_dy_2 = pdy * pdy;
        Image<float> t_dxy = pdx * pdy;
        Image<float> t_dxt = pdx * pdt;
        Image<float> t_dyt = pdy * pdt;

        // sum the constraints
        dx_2[h * width + w] = t_dx_2.sum();
        dy_2[h * width + w] = t_dy_2.sum();
        dxy[h * width + w] = t_dxy.sum();
        dxt[h * width + w] = t_dxt.sum();
        dyt[h * width + w] = t_dyt.sum();
      }
    }
  } else {
    // sum derivatives
    Image<float> dx = pImgDx + cImgDx;
    Image<float> dy = pImgDy + cImgDy;
    Image<float> dt = pImgDt + cImgDt;

    // compute quadratic functions of the the derivative estimates
    dx_2 = dx * dx;
    dy_2 = dy * dy;
    dxy = dx * dy;
    dxt = dx * dt;
    dyt = dy * dt;

    // convolve with uniform which accumulates constraints
    dx_2.convolve(k, winSize, winSize);
    dy_2.convolve(k, winSize, winSize);
    dxy.convolve(k, winSize, winSize);
    dxt.convolve(k, winSize, winSize);
    dyt.convolve(k, winSize, winSize);
  }

  // compute optical flow field
  float m[2][2], m_inv[2][2], b[2];
  for (int h = 0; h < height; h++) {
    for (int w = 0; w < width; w++) {
      // compute second moment matrix
      int ind = h * width + w;
      m[0][0] = dx_2[ind];
      m[1][1] = dy_2[ind];
      m[0][1] = m[1][0] = dxy[ind];
      b[0] = -dxt[ind];
      b[1] = -dyt[ind];

      // compute determinant and trace
      float d = m[0][0] * m[1][1] - m[0][1] * m[1][0];
      float tr = m[0][0] + m[1][1];

      // compute descriminant
      float desc = tr * tr / 4.0 - d;

      // compute eigenvalues
      float eig1 = 0.0;
      float eig2 = 0.0;
      if (desc > 0) {
        eig1 = tr / 2.0 + sqrt(desc);
        eig2 = tr / 2.0 - sqrt(desc);
      }

      // check if inverse exists and eigenvalues are not too small
      float eps = 0.001;
      if (fabs(d) < eps || fabs(eig1) < eps || fabs(eig2) < eps ||
          fabs(eig2 / eig1) < eps) {
        if (u0 && v0) {
          u->setPixel(ind, u0->getPixel(ind));
          v->setPixel(ind, v0->getPixel(ind));
        } else {
          u->setPixel(ind, 0.0);
          v->setPixel(ind, 0.0);
        }
        continue;
      }

      // invert 2x2 matrix
      d = 1.0 / d;
      m_inv[0][0] = m[1][1] * d;
      m_inv[1][1] = m[0][0] * d;
      m_inv[0][1] = m_inv[1][0] = -m[0][1] * d;

      // solve for du, dv
      vp[0] = m_inv[0][0] * b[0] + m_inv[0][1] * b[1];
      vp[1] = m_inv[1][0] * b[0] + m_inv[1][1] * b[1];

      // set estimate
      if (u0 && v0) {
        vp[0] = 2.0 * u0->getPixel(ind) - vp[0];
        vp[1] = 2.0 * v0->getPixel(ind) - vp[1];
      }

      u->setPixel(ind, vp[0]);
      v->setPixel(ind, vp[1]);
    }
  }

  delete[] k;
}

/* Hierarchical Lucas and Kanade optical flow algorithm.  This also assumes
   that the optical flow is uniform among local neighboring samples. */
void computeOpticalFlow_HLK(const Image<float> *pImg, const Image<float> *cImg,
                            const int &winSize, Image<float> *u,
                            Image<float> *v) {
  vector<Image<float> > pyramid_1, pyramid_2;

  // compute gaussian pyramids for both images
  pImg->pyramid(NUM_LEVELS, pyramid_1);
  cImg->pyramid(NUM_LEVELS, pyramid_2);

  // compute simple estimates using LK at highest level
  Image<float> &im1 = pyramid_1.back();
  Image<float> &im2 = pyramid_2.back();
  u->init(im1.height(), im1.width());
  v->init(im1.height(), im1.width());

  // initial estimate at lowest resolution
  computeOpticalFlow_LK(&im1, &im2, 0, 0, winSize, u, v);

  // process all the levels from small to large
  for (int l = NUM_LEVELS - 2; l >= 0; l--) {
    // upsample2 u_i*, v_i* (bilinear) and multiply u_i*, v_i* by 2
    Image<float> u0 = u->upsample2() * 2.0;
    Image<float> v0 = v->upsample2() * 2.0;

    // compute optical flow at next level
    im1 = pyramid_1[l];
    im2 = pyramid_2[l];
    u->init(im1.height(), im1.width());
    v->init(im1.height(), im1.width());

    // compute optical flow estimate update
    computeOpticalFlow_LK(&im1, &im2, &u0, &v0, winSize, u, v);
  }
}

#endif // _OPTICAL_FLOW_H_
