#ifndef _DRAWLINE_H_
#define _DRAWLINE_H_

#include <stdlib.h>

/* An implementation of Bressanham's mid-point, line drawing algorithm. */
void drawLine(const int p0_x, const int p0_y, const int p1_x, const int p1_y,
              const unsigned char v, const int height, const int width,
              unsigned char *img) {
  int x0, y0, x1, y1;

  /* compute delta x and y */
  int dx = abs(p0_x - p1_x);
  int dy = abs(p0_y - p1_y);

  /* which axis is major */
  if (dx > dy) {
    /* check if starting point is to the right of end, if so then swap pts */
    if (p0_x > p1_x) {
      x0 = p1_x;
      y0 = p1_y;
      x1 = p0_x;
      y1 = p0_y;
    } else {
      x0 = p0_x;
      y0 = p0_y;
      x1 = p1_x;
      y1 = p1_y;
    }

    /* check if the line is going top to bottom */
    int ystep = 1;
    if (y1 < y0) {
      ystep = -1;
    }

    /* init error */
    int e = -1 * (dx >> 1);

    /* loop over x axis and mark pixels */
    while (x0 <= x1) {
      img[y0 * width + x0] = v;
      x0++;

      e += dy;
      if (e > 0) {
        y0 += ystep;
        e -= dx;
      }
    }
  } else // dy > dx
  {
    /* check if starting point is to the above end, if so then swap pts */
    if (p0_y > p1_y) {
      x0 = p1_x;
      y0 = p1_y;
      x1 = p0_x;
      y1 = p0_y;
    } else {
      x0 = p0_x;
      y0 = p0_y;
      x1 = p1_x;
      y1 = p1_y;
    }

    /* check if the line is going left */
    int xstep = 1;
    if (x1 < x0) {
      xstep = -1;
    }

    /* init error */
    int e = -1 * (dy >> 1);

    /* loop over y axis and mark pixels */
    while (y0 <= y1) {
      img[y0 * width + x0] = v;
      y0++;

      e += dx;
      if (e > 0) {
        x0 += xstep;
        e -= dy;
      }
    }
  }
}

#endif // _DRAWLINE_H_
