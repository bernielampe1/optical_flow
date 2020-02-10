template <typename T> void Image<T>::convolve(const float *k, const int ksize) {
  Image<float> temp(_width, _height);
  int center = ksize >> 1;

  // convolve with 1-D kernel in the x direction
  for (int h = 0; h < _height; h++) {
    for (int w = 0; w < _width; w++) {
      float d = 0.0;
      for (int c = -center; c <= center; c++) {
        int wp = w + c;
        if (wp >= 0 && wp < _width)
          d += _data[h * _width + wp] * k[center + c];
      }

      temp._data[w * _height + h] = (T)d; // temp is flipped
    }
  }

  // convolve with 1-D kernel in the y direction
  for (int h = 0; h < temp._height; h++) {
    for (int w = 0; w < temp._width; w++) {
      float d = 0.0;
      for (int c = -center; c <= center; c++) {
        int wp = w + c;
        if (wp >= 0 && wp < temp._width)
          d += temp._data[h * temp._width + wp] * k[center + c];
      }

      _data[w * temp._height + h] = (T)d; // destination if flipped
    }
  }
}

template <typename T>
void Image<T>::convolve(const float *k, const int kheight, const int kwidth) {
  Image<float> temp(_height, _width);

  // special case if the ksize is even
  int kheightp = (kheight % 2) ? kheight : kheight - 1;
  int kwidthp = (kwidth % 2) ? kwidth : kwidth - 1;

  // convolve with 2-D kernel
  for (int h = 0; h < _height; h++) {
    for (int w = 0; w < _width; w++) {
      float d = 0.0; // kernel accumulator

      // loop over kernel
      for (int i = 0; i < kheight; i++) {
        for (int j = 0; j < kwidth; j++) {
          int hp = h + i - (kheightp >> 1);
          int wp = w + j - (kwidthp >> 1);

          if (hp >= 0 && hp < _height && wp >= 0 && wp < _width) {
            d += _data[hp * _width + wp] * k[i * kwidth + j];
          }
        }
      }

      temp._data[h * _width + w] = (T)d; // temp is flipped
    }
  }

  // copy convolved image to this image
  *this = temp;
}

template <typename T> T Image<T>::sum() const {
  T s = (T)0;
  int n = _height * _width;

  for (int i = 0; i < n; i++) {
    s += _data[i];
  }

  return (s);
}

template <typename T> Image<T> Image<T>::operator+(const Image<T> &im) const {
  Image<T> temp(_height, _width);

  for (int h = 0; h < _height; h++) {
    for (int w = 0; w < _width; w++) {
      int ind = h * _width + w;
      temp._data[ind] = _data[ind] + im._data[ind];
    }
  }

  return (temp);
}

template <typename T> Image<T> Image<T>::operator-(const Image<T> &im) const {
  Image<T> temp(_height, _width);

  for (int h = 0; h < _height; h++) {
    for (int w = 0; w < _width; w++) {
      int ind = h * _width + w;
      temp._data[ind] = _data[ind] - im._data[ind];
    }
  }

  return (temp);
}

template <typename T> Image<T> Image<T>::operator*(const Image<T> &im) const {
  Image<T> temp(_height, _width);

  for (int h = 0; h < _height; h++) {
    for (int w = 0; w < _width; w++) {
      int ind = h * _width + w;
      temp._data[ind] = _data[ind] * im._data[ind];
    }
  }

  return (temp);
}

template <typename T> Image<T> Image<T>::operator*(const T &val) const {
  Image<T> temp(_height, _width);

  for (int h = 0; h < _height; h++) {
    for (int w = 0; w < _width; w++) {
      int ind = h * _width + w;
      temp._data[ind] = _data[ind] * val;
    }
  }

  return (temp);
}

template <typename T>
void Image<T>::pyramid(const int levels, vector<Image<T> > &py) const {
  float a = 0.375;
  float k[] = {0.25 - a / 2.0, 0.25, a, 0.25, 0.25 - a / 2.0};
  Image<T> temp;

  py.clear();          // clear return vector
  py.push_back(*this); // add first level

  int hp = _height;
  int wp = _width;
  for (int l = 1; l < levels; l++) {
    // alloc new image with 1/2 dim.
    hp = ceil(hp / 2.0);
    wp = ceil(wp / 2.0);
    temp.init(hp, wp);

    // filter last image
    Image<T> &t = py.back();
    t.convolve(k, 5);

    // subsample last image
    for (int h = 0; h < temp.height(); h++) {
      for (int w = 0; w < temp.width(); w++) {
        int h_2 = 2 * h;
        int w_2 = 2 * w;
        temp._data[h * temp.width() + w] = t.getPixel(h_2 * t.width() + w_2);
      }
    }

    py.push_back(temp);
  }
}

template <typename T> Image<T> Image<T>::upsample2() const {
  int hp = 2 * _height;
  int wp = 2 * _width;
  Image<T> temp(hp, wp);

  for (int h = 0; h < hp; h++) {
    for (int w = 0; w < wp; w++) {
      float hc = h / 2.0;
      float wc = w / 2.0;

      temp._data[h * wp + w] = bilinear(hc, wc);
    }
  }

  return (temp);
}

template <typename T> T Image<T>::bilinear(const float h, const float w) const {
  int lr = int(h);
  int ur = lr + 1;

  int lc = int(w);
  int uc = lc + 1;

  if (ur >= _height) {
    lr--;
    ur--;
  }
  if (uc >= _width) {
    lc--;
    uc--;
  }

  T v0 = _data[lr * _width + lc];
  T v1 = _data[lr * _width + uc];
  T v2 = _data[ur * _width + lc];
  T v3 = _data[ur * _width + uc];

  T t0 = (uc - w) * v0 + (w - lc) * v1;
  T t1 = (uc - w) * v2 + (w - lc) * v3;

  return ((ur - h) * t0 + (h - lr) * t1);
}

template <typename T> void Image<T>::readFromFile(const string &fname) {
  throw Exception("not implemented");
}

template <> void Image<RGB_t>::readFromFile(const string &fname) {
  ifstream ifile;
  string magic, cols, rows, max;
  unsigned char r, g, b;

  ifile.open(fname.c_str());
  if (!ifile) {
    throw Exception("could not read from file");
  }

  ifile >> magic >> cols >> rows >> max;
  init(atoi(rows.c_str()), atoi(cols.c_str()));

  for (int i = 0; i < _width * _height; i++) {
    ifile.read((char *)&r, sizeof(unsigned char));
    ifile.read((char *)&g, sizeof(unsigned char));
    ifile.read((char *)&b, sizeof(unsigned char));

    _data[i][0] = r;
    _data[i][1] = g;
    _data[i][2] = b;
  }

  ifile.close();
}

template <typename T> void Image<T>::writeToFile(const string &fname) const {
  T minVal, maxVal;
  float scaleVal;
  ofstream ofile;
  int numElems = _height * _width;

  if (numElems <= 0) {
    throw(Exception("cannot write a null image object to file"));
  }

  minVal = maxVal = _data[0];
  for (int i = 1; i < numElems; i++) {
    if (minVal > _data[i])
      minVal = _data[i];
    if (maxVal < _data[i])
      maxVal = _data[i];
  }

  scaleVal = 255.0 / (minVal < maxVal ? maxVal - minVal : 1.0);

  ofile.open(fname.c_str());
  if (!ofile) {
    throw(Exception("could not open image file for writing"));
  }

  ofile << "P5\n" << _width << " " << _height << "\n255\n";
  for (int i = 0; i < numElems; i++) {
    unsigned char val = (unsigned char)((_data[i] - minVal) * scaleVal + 0.5);
    ofile.write((char *)&val, sizeof(unsigned char));
  }

  ofile.close();
}

template <> void Image<RGB_t>::writeToFile(const string &fname) const {
  ofstream ofile;
  int numElems = _height * _width;

  ofile.open(fname.c_str());
  if (!ofile) {
    throw(Exception("unable to open file for writing"));
  }

  ofile << "P6" << endl << _width << " " << _height << endl << "255" << endl;
  for (int i = 0; i < numElems; i++) {
    ofile.write((char *)&(_data[i][0]), sizeof(unsigned char));
    ofile.write((char *)&(_data[i][1]), sizeof(unsigned char));
    ofile.write((char *)&(_data[i][2]), sizeof(unsigned char));
  }

  ofile.close();
}

template <> void Image<Vec2f_t>::writeToFile(const string &fname) const {
  ofstream ofile;
  int spac = 10;

  // allocate space
  unsigned char *img = new unsigned char[_height * _width];
  memset(img, 0, _height * _width * sizeof(unsigned char));

  // construct the graphical vector field
  for (int h = 0; h < _height; h += spac) {
    for (int w = 0; w < _width; w += spac) {
      int ex = w + _data[h * _width + w][0];
      int ey = h + _data[h * _width + w][1];

      if (ex >= 0 && ex < _width && ey >= 0 && ey < _height) {
        drawLine(w, h, ex, ey, (unsigned char)255, _height, _width, img);
      }
    }
  }

  // open file and check
  ofile.open(fname.c_str());
  if (!ofile) {
    throw(Exception("unable to open file for writing"));
  }

  // write to file
  ofile << "P5" << endl << _width << " " << _height << endl << "255" << endl;
  ofile.write((char *)img, _width * _height * sizeof(unsigned char));
  ofile.close();

  delete[] img;
}

#if 0
template<>
void Image<Vec2f_t>::writeToFile(
  const string &fname
  ) const
{
  ofstream ofile;
  int spac = 10;

  // allocate space
  Image<float> img(_height, _width);

  // construct the graphical vector field
  for(int h = 0; h < _height; h += spac)
  {
    for(int w = 0; w < _width; w += spac)
    {
      float v = sqrt(_data[h * _width + w][0] * _data[h * _width + w][0] +
                     _data[h * _width + w][1] * _data[h * _width + w][1]);
      img.setPixel(h * _width + w, v);
    }
  }

  img.writeToFile(fname);
}
#endif

template <typename T>
void Image<T>::writeToFile(const string &fname, const float *textimg) const {
  throw Exception("not implemented");
}

template <>
void Image<Vec2f_t>::writeToFile(const string &fname,
                                 const float *textimg) const {
  ofstream ofile;
  vector<int> p_x, p_y;
  int p0_x, p0_y, p1_x, p1_y;
  float dist = 5.0;

  // allocate space
  float *accimg = new float[_height * _width];
  memset(accimg, 0, _height * _width * sizeof(float));

  // allocate space for magnitudes
  float *magimg = new float[_height * _width];
  memset(magimg, 0, _height * _width * sizeof(float));

  // integrate vectors
  float v, mag, sqrt_2 = sqrt(2.0);
  for (int h = 0; h < _height; h++) {
    for (int w = 0; w < _width; w++) {
      mag = sqrt(_data[h * _width + w][0] * _data[h * _width + w][0] +
                 _data[h * _width + w][1] * _data[h * _width + w][1]);

      // remove vectors less than one 8-connected pixel
      if (mag < sqrt_2)
        continue;

      magimg[h * _width + w] = mag;

      p0_x = int(w + _data[h * _width + w][0] / mag * dist + 0.5);
      p0_y = int(h + _data[h * _width + w][1] / mag * dist + 0.5);
      p1_x = int(w - _data[h * _width + w][0] / mag * dist + 0.5);
      p1_y = int(h - _data[h * _width + w][1] / mag * dist + 0.5);

      getLinePts(p0_x, p0_y, p1_x, p1_y, p_x, p_y);

      v = 0;
      for (unsigned i = 0; i < p_x.size(); i++) {
        int hp = p_y[i];
        int wp = p_x[i];

        if (hp >= 0 && hp < _height && wp >= 0 && wp < _width) {
          v += textimg[hp * _width + wp];
        }
      }

      accimg[h * _width + w] = v;
    }
  }

  // convolve vector directions with strength
  for (int i = 0; i < _width * _height; i++) {
    accimg[i] *= magimg[i];
  }

  // get stats
  float min = accimg[0];
  float max = accimg[0];
  for (int i = 0; i < _width * _height; i++) {
    if (min > accimg[i])
      min = accimg[i];
    if (max < accimg[i])
      max = accimg[i];
  }

  // open file and check
  ofile.open(fname.c_str());
  if (!ofile) {
    throw(Exception("unable to open file for writing"));
  }

  ofile << "P6\n" << _width << " " << _height << "\n255\n";

  // scale factor
  float scale = 1.0;
  if (max - min > 0.01)
    scale = 255.0 / (max - min);

  // write to file
  for (int i = 0; i < _width * _height; i++) {
    unsigned char val;
    val = scale * (accimg[i] - min) + 0.5;
    ofile.write((char *)&jetBlackMap[val][0], sizeof(unsigned char));
    ofile.write((char *)&jetBlackMap[val][1], sizeof(unsigned char));
    ofile.write((char *)&jetBlackMap[val][2], sizeof(unsigned char));
  }

  ofile.close();

  delete[] accimg;
}

Image<float> *getChannel(const Image<RGB_t> *img, const int &n) {
  Image<float> *rtn = new Image<float>(img->height(), img->width());
  int numElems = img->height() * img->width();
  float v;

  for (int i = 0; i < numElems; i++) {
    v = (float)((img->getPixel(i))[n]);
    rtn->setPixel(i, v);
  }

  return (rtn);
}

Image<float> *getChannel(const Image<Vec2f_t> *img, const int &n) {
  Image<float> *rtn = new Image<float>(img->height(), img->width());
  int numElems = img->height() * img->width();
  float v;

  for (int i = 0; i < numElems; i++) {
    v = img->getPixel(i).v[n];
    rtn->setPixel(i, v);
  }

  return (rtn);
}

Image<float> *computeBrightness(const Image<RGB_t> *im) {
  Image<float> *rtn = new Image<float>(im->height(), im->width());
  int numElems = im->height() * im->width();
  float v;

  for (int i = 0; i < numElems; i++) {
    v = 0.0;
    for (int j = 0; j < 3; j++) {
      v += im->getPixel(i).c[j] * im->getPixel(i).c[j];
    }
    rtn->setPixel(i, sqrt(v));
  }

  return (rtn);
}

// compute magnitude and direction channels
Image<float> *getMagnitude(const Image<Vec2f_t> *vfield) {
  Image<float> *rtn = new Image<float>(vfield->height(), vfield->width());
  int numElems = vfield->height() * vfield->width();

  for (int i = 0; i < numElems; i++) {
    float v = vfield->getPixel(i).v[0] * vfield->getPixel(i).v[0] +
              vfield->getPixel(i).v[1] * vfield->getPixel(i).v[1];

    rtn->setPixel(i, sqrt(v));
  }

  return (rtn);
}

// compute magnitude and direction channels
Image<float> *getDirection(const Image<Vec2f_t> *vfield) {
  Image<float> *rtn = new Image<float>(vfield->height(), vfield->width());
  int numElems = vfield->height() * vfield->width();

  for (int i = 0; i < numElems; i++) {
    float v = atan2(vfield->getPixel(i).v[1], vfield->getPixel(i).v[0]);
    rtn->setPixel(i, v);
  }

  return (rtn);
}
