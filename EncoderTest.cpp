#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <algorithm>
#include <math.h>
#include "VideoEncoder.h"

class VpxImage {
public:

  VpxImage(const int width, const int height, vpx_img_fmt format = VPX_IMG_FMT_RGB24) : format_(format) {
    image_ = vpx_img_alloc(NULL, format, width, height, 1);
  }

  VpxImage(const VpxImage& src) {
    *this = src;
  }

  ~VpxImage() {
    if (image_) {
      vpx_img_free(image_);
      image_ = 0;
    }
  }

  VpxImage& operator =(const VpxImage& src) {
    if (image_) {
      vpx_img_free(image_);
      image_ = 0;
    }
    assert(src.format() == VPX_IMG_FMT_RGB24);
    image_ = vpx_img_alloc(NULL, src.format(), src.width(), src.height(), 1);
    if (image_) {
      memcpy(image_->img_data, src.buffer(), width()*height()*3);
    }
    return *this;
  }

  bool valid() const {
    return image_;
  }

  vpx_image_t* image() {
    return image_;
  }

  vpx_img_fmt format() const {
    return format_;
  }

  int width() const {
    return image_->w;
  }

  int height() const {
    return image_->h;
  }

  uint8_t* buffer() {
    return (uint8_t*)image_->img_data;
  }

  const int* buffer() const {
    return (int*)image_->img_data;
  }

  void setPixel(const int x, const int y, const float r, const float g, const float b) {
    assert(format() == VPX_IMG_FMT_RGB24);
    assert(x>=0 && x<width() && y>=0 && y<height());
    uint8_t* buf = buffer();
    buf[(y*width()+x)*3+0] = uint8_t(saturate(r) * 255.0);
    buf[(y*width()+x)*3+1] = uint8_t(saturate(g) * 255.0);
    buf[(y*width()+x)*3+2] = uint8_t(saturate(b) * 255.0);
  }

  static float saturate(const float x) {
    return std::min(std::max(x, 0.0f), 1.0f);
  }

  vpx_image_t* image_;
  vpx_img_fmt format_;
};

VpxImage generate(float time) {
  float f = 0.5+0.5*sinf(time);
  VpxImage r(720, 480);
  for (int y = 0; y < r.height(); ++y) {
    for (int x = 0; x < r.width(); ++x) {
      float fx = x / float(r.width()) * 2.0f - 1.0f;
      float fy = y / float(r.height()) * 2.0f - 1.0f;
      float k = fx * fx + fy * fy;
      float fog = 1.0 / (1.0 + k);
      r.setPixel(x, y, fog*f, fog, fog);
    }
  }
  return r;
}

int main(int argc, char* argv[])
{
  if (argc != 2) {
    printf("Usage: %s <outfile>\n", argv[0]);
    return 0;
  }

  VideoEncoder encoder(argv[1]);

  const int fps = 30;
  const int n = fps * 5;
  for (int i = 0; i < n; ++i) {
    float t = i / float(n);
    VpxImage frame = generate(t*10.0);
    if (!encoder.writeFrame(frame.image())) {
      printf("Error");
      return -1;
    }
  }

  encoder.finish();

  return 0;
}
