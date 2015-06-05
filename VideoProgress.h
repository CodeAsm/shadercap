#ifndef __VIDEO_PROGRESS_H__
#define __VIDEO_PROGRESS_H__

#include <QtWidgets/QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "VideoParameters.h"
#include "ShaderParameter.h"
#include "RenderSurface.h"
#include "VideoEncoder.h"

class VideoProgress : public QWidget {
  Q_OBJECT
public:
  ~VideoProgress();
  VideoProgress(const VideoParameters& videoParameters, const ShaderParameters& shaderParameters, QWidget* parent);
public slots:
  void onLoadResource();
  void onNextFrame();
  void onFinalPress();
public:
  void finishCapture(const std::string& status);
signals:
  void onComplete();
private:
  QLabel* frame;
  QProgressBar* bar;
  QPushButton* button;

  VideoParameters vp;
  ShaderParameters sp;
  RenderSurface* renderSurface;
  VideoEncoder* encoder;

  bool finalFrame;
  bool captureDone;
  size_t frameCount;
  size_t durationSeconds;

  size_t resourceLoadIndex;
};

#endif // __VIDEO_PROGRESS_H__
