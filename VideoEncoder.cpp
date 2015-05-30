#include "VideoEncoder.h"

#define RGB2YUV_SHIFT 15
#define BY ( (int)(0.114*219/255*(1<<RGB2YUV_SHIFT)+0.5))
#define BV (-(int)(0.081*224/255*(1<<RGB2YUV_SHIFT)+0.5))
#define BU ( (int)(0.500*224/255*(1<<RGB2YUV_SHIFT)+0.5))
#define GY ( (int)(0.587*219/255*(1<<RGB2YUV_SHIFT)+0.5))
#define GV (-(int)(0.419*224/255*(1<<RGB2YUV_SHIFT)+0.5))
#define GU (-(int)(0.331*224/255*(1<<RGB2YUV_SHIFT)+0.5))
#define RY ( (int)(0.299*219/255*(1<<RGB2YUV_SHIFT)+0.5))
#define RV ( (int)(0.500*224/255*(1<<RGB2YUV_SHIFT)+0.5))
#define RU (-(int)(0.169*224/255*(1<<RGB2YUV_SHIFT)+0.5))

static inline void rgb24toyv12(vpx_image_t *rgbImage, vpx_image_t *yv12Image) {
	unsigned int width = rgbImage->w;
	unsigned int height = rgbImage->h;
	unsigned int planeSize = width * height;

	unsigned char* rgb = rgbImage->img_data;

	unsigned char* yPlane = yv12Image->img_data;
	unsigned char* uPlane = yPlane + planeSize;
	unsigned char* vPlane = uPlane + (planeSize >> 2);

	unsigned int i;

	// Y pass.
	for (i = 0; i < width * height; ++ i)
	{
		unsigned int r = rgb[3*i+0];
		unsigned int g = rgb[3*i+1];
		unsigned int b = rgb[3*i+2];

		unsigned int y = ((RY*r + GY*g + BY*b)>>RGB2YUV_SHIFT) + 16;
		unsigned int u = ((RU*r + GU*g + BU*b)>>RGB2YUV_SHIFT) + 128;
		unsigned int v = ((RV*r + GV*g + BV*b)>>RGB2YUV_SHIFT) + 128;

		rgb[3*i+0] = y;
		rgb[3*i+1] = u;
		rgb[3*i+2] = v;

		yPlane[i] = y;
	}

	// UV pass, 4 x 4 downsampling.
	i = 0;
	for (unsigned int y = 0; y < height; y += 2)
	{
		for (unsigned int x = 0; x < width; x += 2)
		{
			unsigned int sumU = 0, sumV = 0;

			// Left Root.
			//
			sumU += rgb[3*(y * width + x)+1];
			sumV += rgb[3*(y * width + x)+2];

			// Right Root.
			sumU += rgb[3*(y * width + x + 1)+1];
			sumV += rgb[3*(y * width + x + 1)+2];

			// Left Top.
			sumU += rgb[3*((y+1) * width + x)+1];
			sumV += rgb[3*((y+1) * width + x)+2];

			// Right Top.
			sumU += rgb[3*((y+1) * width + x + 1)+1];
			sumV += rgb[3*((y+1) * width + x + 1)+2];

			// Get average.
			uPlane[i] = sumU / 4;
			vPlane[i] = sumV / 4;

			i += 1;
		}
	}
}

VideoEncoder::VideoEncoder(const std::string& outFile, int fps, int br) : frameNumber_(0), outFile_(outFile), framesPerSecond(fps), bitRate(br), init_(false) {
}

bool VideoEncoder::writeFrame(vpx_image_t* src)
{
  vpx_image_t* yv12Image = vpx_img_alloc(NULL, VPX_IMG_FMT_I420, src->w, src->h, 1);
  if (!yv12Image) {
    return false;
  }

  rgb24toyv12(src, yv12Image);

  if (!init_) {
    if (vpx_codec_enc_config_default(vpx_codec_vp8_cx(), &vpxConfig_, 0) != VPX_CODEC_OK) {
      return false;
    }

    vpxConfig_.g_h = yv12Image->h;
    vpxConfig_.g_w = yv12Image->w;

    vpxConfig_.g_threads = 1;

    vpxConfig_.rc_target_bitrate = bitRate;

    vpxConfig_.g_timebase.den = framesPerSecond;
    vpxConfig_.g_timebase.num = 1;

    // Prepare the output .webm file.
    //
    memset(&ebml_, 0, sizeof(EbmlGlobal));
    ebml_.last_pts_ms = -1;
    ebml_.file = fopen(outFile_.c_str(), "wb");
    if (ebml_.file == NULL)
    {
      return false;
    }
    //vpx_rational ebmlFPS = vpxConfig_.g_timebase;
    struct vpx_rational arg_framerate = {framesPerSecond, 1};
    Ebml_WriteWebMFileHeader(&ebml_, &vpxConfig_, &arg_framerate);

    duration_ = (float)arg_framerate.den / (float)arg_framerate.num * 1000;

    if (vpx_codec_enc_init(&vpxContext_, vpx_codec_vp8_cx(), &vpxConfig_, 0) != VPX_CODEC_OK)
    {
        return false;
    }

    init_ = true;
  }

  vpx_codec_err_t vpxError = vpx_codec_encode(&vpxContext_, yv12Image, frameNumber_, duration_, 0, 0);
  if (vpxError != VPX_CODEC_OK)
  {
    return false;
  }
  
  vpx_codec_iter_t iter = NULL;
  const vpx_codec_cx_pkt_t *packet;
  while( (packet = vpx_codec_get_cx_data(&vpxContext_, &iter)) )
  {
    Ebml_WriteWebMBlock(&ebml_, &vpxConfig_, packet);
  }

  frameNumber_++;

  vpx_img_free(yv12Image);
  yv12Image = NULL;

  vpx_img_free(yv12Image);
  return true;
}

void VideoEncoder::finish() {
	Ebml_WriteWebMFileFooter(&ebml_, 0);
	fclose(ebml_.file);
	vpx_codec_destroy(&vpxContext_);
}
