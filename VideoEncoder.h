#ifndef __VIDEO_ENCODER_H__
#define __VIDEO_ENCODER_H__

#include <vpx/vpx_codec.h>
#include <vpx/vpx_encoder.h>
#include <vpx/vpx_image.h>
#include <vpx/vp8cx.h>
#include <string>
#include "EbmlWriter.h"

class VideoEncoder
{
public:

  VideoEncoder(const std::string& outFile, int fps, int br);

  int fpsValue() const { return framesPerSecond; }

  bool writeFrame(vpx_image_t* src);

  void finish();

private:

  int frameNumber_;
  EbmlGlobal ebml_;
  vpx_codec_ctx_t vpxContext_;
  vpx_codec_enc_cfg_t vpxConfig_;
  size_t duration_;
  std::string outFile_;
  int framesPerSecond;
  int bitRate;
  bool init_;
};

#endif // __VIDEO_ENCODER_H__
