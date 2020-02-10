#ifndef _FILESTREAMDECODER_H_
#define _FILESTREAMDECODER_H_

#include <exception>
#include <string>

using namespace std;

extern "C"
{
  #include "avcodec.h"
  #include "avformat.h"
  #include "swscale.h"
}

#include "Image.h"

/* Video decoder class.  This object represents the state of the video stream.
   It permits a user to decode and extract single frames in time order.
   Frames are returned in an Image class object. */
class FileStreamDecoder
{
  private:
    string srcFileName;
    AVFormatContext *fmtContext;
    AVCodecContext *codecContext;
    AVCodec *codec;
    AVFrame *frame, *rgbFrame;
    uint8_t *buffer;
    int numBytes;
    int videoStream;
    struct SwsContext *imgConvertContext;

    Image<RGB_t>* frameToImg(
      const AVFrame *frame,
      const int width,
      const int height
        ) const
    {
      Image<RGB_t> *iptr = new Image<RGB_t>(height, width);
      unsigned char *ptr;
      int numElems = height * width;
      RGB_t val;

      ptr = frame->data[0];
      for(int i = 0; i < numElems; i++)
      {
        val[0] = *ptr; ptr++;
        val[1] = *ptr; ptr++;
        val[2] = *ptr; ptr++;
        iptr->setPixel(i, val);
      }

      return(iptr);
    }

  public:
    FileStreamDecoder(
      const string &f
      ): srcFileName(f), fmtContext(0), codecContext(0), codec(0), frame(0),
         rgbFrame(0), buffer(0), numBytes(0), videoStream(-1),
         imgConvertContext(0)
    {
      av_register_all(); // register file formats

      // open the file stream
      if (av_open_input_file(&fmtContext, srcFileName.c_str(), 0, 0, 0) != 0)
      {
        throw(Exception("unable to open file stream"));
      }

      // get the stream information
      if (av_find_stream_info(fmtContext) < 0)
      {
        throw(Exception("unable to file stream info in file"));
      }

      #if DEBUG
      // writes to standard error
      dump_format(fmtContext, 0, srcFileName.c_str(), 0);
      #endif

      // find the video stream in the file
      for(int i = 0; i < int(fmtContext->nb_streams); i++)
      {
        if (fmtContext->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO)
        {
          videoStream = i;
          break;
        }
      }

      if (videoStream == -1)
      {
        throw(Exception("unable to file stream in file"));
      }

      // save ptr to video stream
      codecContext = fmtContext->streams[videoStream]->codec;

      // get the decoder ptr
      codec = avcodec_find_decoder(codecContext->codec_id);
      if (!codec)
      {
        throw(Exception("unsupported codec"));
      }

      // open the codec
      if (avcodec_open(codecContext, codec) < 0)
      {
        throw(Exception("could not open codec"));
      }

      // allocate space for frame
      frame = avcodec_alloc_frame();
      if (!frame)
      {
        throw(Exception("could not allocate memory for frame"));
      }

      // allocate space for RGB frame
      rgbFrame = avcodec_alloc_frame();
      if (!rgbFrame)
      {
        throw(Exception("could not allocate memory for RGB frame"));
      }

      // get data for and allocate buffer
      numBytes = avpicture_get_size(PIX_FMT_RGB24, codecContext->width,
                    codecContext->height);
      buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

      // associate frame with the buffer
      avpicture_fill((AVPicture*)rgbFrame, buffer, PIX_FMT_RGB24,
                     codecContext->width, codecContext->height);

      // get context to convert frame
      imgConvertContext = sws_getContext(codecContext->width,
                                         codecContext->height,
                                         codecContext->pix_fmt,
                                         codecContext->width,
                                         codecContext->height,
                                         PIX_FMT_RGB24, SWS_BICUBIC,
                                         0, 0, 0);
      if (!imgConvertContext)
      {
        throw(Exception("unable to get conversion context"));
      }
    }

    ~FileStreamDecoder(
      )
    {
      av_free(buffer);
      av_free(rgbFrame);
      av_free(frame);
      avcodec_close(codecContext);
      av_close_input_file(fmtContext);
    }

    Image<RGB_t>* getFrame(
      ) const
    {
      AVPacket packet;
      int done;

      // get packets until the frame is complete
      while (fmtContext && av_read_frame(fmtContext, &packet) >= 0)
      {
        if (packet.stream_index == videoStream)
        {
          // decode stream frame
          avcodec_decode_video(codecContext, frame, &done,
                   packet.data, packet.size);

          // convert context
          if (frame->data[0])
          {
            sws_scale(imgConvertContext, frame->data, frame->linesize, 0,
                      codecContext->height, rgbFrame->data, rgbFrame->linesize);

            // when frame is complete
            if (done)
            {
              av_free_packet(&packet); // release packets
              return(frameToImg(rgbFrame, codecContext->width,
                                          codecContext->height));
            }
          }
        }
      }

      av_free_packet(&packet); // release packets

      return(0); // return null if av_read_frame fails before done
    }
};

#endif // _FILESTREAMDECODER_H_
