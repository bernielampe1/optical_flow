This file accompanies the code for "Graph-Based Video Segmentation Using Optical Flow Fields" which uses the Hierarchical Lucas and Kanade optical flow algorithm to estimate the flow and graph based segmentation to find connected components of similar flow.

1) Building and Running
The executable, "segment", can be built on any UNIX-like platform supporting GNU make
3.8+, g++ 4.0+ and has FFMPEG version 0.5+ with supporting development header files.

Building FFMPEG

tar jxvf ffmpeg-0.6.1.tar.bz2
cd ffmpeg-0.6.1.tar
./configure --disable-asm --enable-shared
make -j4

Set the FFMPEG_BASE variable in the Makefiles then build segment as follows.

cd segment/src
make

To view what command line arguments are required you can just run the execuatble. You may need to set LD_LIBRAY_PATH or DYLD_LIBRARY_PATH to find the libraries. For example,

./segment
./segment <video stream file> <sigma> <winSize> <tsteps> <threshold> <minSize>

Testing can be done using the sample videos.  To run the simple spinning ball
example, the segment program can be done as follows.

./segment ../vids/vid2.avi 0.25 5 1 400 500

The results are written to the current working directory as PPM image files.
These files are the segmented frames of the video.

The command line arguments in this example are as follows.
  0.25 is the variance of the Gaussian kernel used for pre-filtering
  5 is the dimension of the square window used for optical flow in pixels
  1 is the number of frames to integrate to reduce noise
  400 is the threshold used for graph-based segmentation in pixels
  500 is the minmum number of pixels that a set can be during segmentation

2) Source Files Descriptions
cmap.h              - the color map of used for image visualization
DisjointSet.h        - declaration of the disjoint set using union-find
DisjointSet.inl      - definition of the disjoint set using union-find
drawLine.h          - implementation of Bressanham's mid-point algorithm
Edge.h              - definition of edge for graph-based segmentation
Exception.h          - error handleing class
FileStreamDecoder.h  - definition of video decoding
gaussian.h          - contains function to compute normalized Gaussian function
getLinePts.h        - implementation of Bressanham's that returns pixel locations
graphCol.h          - routine to color disjoint set graph
graphGen.h          - routine to generate complete graph based on image pixels
graphRed.h          - routine to remove sets in the graph that are too small
graphSeg.h          - routine to segment the image based graph into partitions
Image.h             - declaration of image abstraction
Image.inl           - definition of image operations
Makefile            - build file for GNU make 3.8+
opticalFlow.h       - implementation of Horn & Schunck and Lucas and Kanade optical flow estimation algorithms
segment.cpp         - main program to segment video stream based on optical flow
