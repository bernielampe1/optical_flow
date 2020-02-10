#include <iostream>
#include <sstream>
#include <string>

using namespace std;

#include "Exception.h"
#include "FileStreamDecoder.h"
#include "Image.h"

int main(int argc, char **argv) {
  Image<RGB_t> *cImg;

  if (argc != 2) {
    cerr << argv[0] << " <video stream file>" << endl;
    return (1);
  }

  try {
    // intialize the video stream
    FileStreamDecoder streamObj(argv[1]);

    int i = 0;                   // frame number
    cImg = streamObj.getFrame(); // get first frame
    while (cImg) {
      // compose file name
      ostringstream oss;
      oss << "frame_" << i++ << ".ppm";

      // write test file
      cImg->writeToFile(oss.str());

      // grab frame
      if (cImg)
        delete cImg;
      cImg = streamObj.getFrame();
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
