#ifndef _IMAGE_H_
#define _IMAGE_H_

#include <fstream>
#include <math.h>
#include <string.h>
#include <vector>

using namespace std;

#include "Exception.h"
#include "cmap.h"
#include "drawLine.h"
#include "getLinePts.h"

/* Abstraction of a 2-D vector used as an image pixel type below. */
struct Vec2f_t {
  float v[2];

  // initialize to zero
  Vec2f_t() { v[0] = v[1] = 0.0; }

  // accessor
  float &operator[](const int i) { return (v[i]); }
};

/* Abstraction of a RGB color vector used as an image pixel type below. */
struct RGB_t {
  unsigned char c[3];

  // initialize to zero
  RGB_t() { c[0] = c[1] = c[2] = 0; }

  // accessor
  unsigned char &operator[](const int i) { return (c[i]); }
};

/* Abstraction of a 2-D image with template pixel type. */
template <typename T> class Image {
private:
  T *_data;            // dynamic data storage for 2-D array of pixel
  int _height, _width; // image dimensions

public:
  Image() : _data(0), _height(0), _width(0) {}

  Image(const int h, const int w) : _data(0), _height(h), _width(w) {
    init(_height, _width);
  }

  Image(const Image<T> &o) : _data(0), _height(o._height), _width(o._width) {
    init(_height, _width);
    memcpy(_data, o._data, _height * _width * sizeof(T));
  }

  ~Image() { clear(); }

  void init(const int h, const int w) {
    if (_data) {
      clear();
    }

    _height = h;
    _width = w;
    _data = new T[_height * _width]; // row major
    memset(_data, 0, _height * _width * sizeof(T));
  }

  void clear() {
    if (_data)
      delete[] _data;
    _height = _width = 0;
    _data = 0;
  }

  int height() const { return (_height); }

  int width() const { return (_width); }

  Image<T> &operator=(const Image<T> &rhs) {
    if (this != &rhs) {
      _height = rhs._height;
      _width = rhs._width;
      init(_height, _width);
      memcpy(_data, rhs._data, _height * _width * sizeof(T));
    }

    return (*this);
  }

  T &operator[](const int &i) const { return (_data[i]); }

  T &getPixel(const int ind) const { return (_data[ind]); }

  void setPixel(const int ind, const T &val) { _data[ind] = val; }

  void convolve(const float *k, const int ksize);

  void convolve(const float *k, const int kheight, const int kwidth);

  void pyramid(const int levels, vector<Image<T> > &py) const;

  Image<T> upsample2() const;

  T sum() const;

  T bilinear(const float h, const float w) const;

  Image<T> operator+(const Image<T> &im) const;

  Image<T> operator-(const Image<T> &im) const;

  Image<T> operator*(const Image<T> &im) const;

  Image<T> operator*(const T &val) const;

  void readFromFile(const string &fname);

  void writeToFile(const string &fname) const;

  void writeToFile(const string &fname, const float *textimg) const;
};

#include "Image.inl"

#endif // _IMAGE_H_
