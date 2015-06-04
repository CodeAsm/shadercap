#ifndef __VIDEO_PARAMETERS_H__
#define __VIDEO_PARAMETERS_H__

struct VideoParameters {
  std::string code;
  std::string path;
  int width;
  int height;
  int fps;
  int bitrate;
  int duration;
};

#endif // __VIDEO_PARAMETERS_H__
