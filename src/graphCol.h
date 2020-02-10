#ifndef _GRAPHCOL_H_
#define _GRAPHCOL_H_

#include <map>
#include <stdlib.h>

/* This function returns a random RGB with values from 0-255. */
RGB_t randRGB() {
  RGB_t c;

  c[0] = (unsigned char)rand();
  c[1] = (unsigned char)rand();
  c[2] = (unsigned char)rand();

  return (c);
}

/* This function returns a RGB Image whereby every set of pixels is given
   a different, random color. */
void colorSegments(const DisjointSet<int> &universe, Image<RGB_t> &segImg) {
  map<unsigned, RGB_t> colorMap;
  int numPixels = segImg.height() * segImg.width();

  for (int i = 0; i < numPixels; i++) {
    int p0 = universe.find(i);

    if (colorMap.find(p0) == colorMap.end()) {
      colorMap[p0] = randRGB();
    }

    segImg.setPixel(i, colorMap[p0]);
  }
}

#endif // _GRAPHCOL_H_
