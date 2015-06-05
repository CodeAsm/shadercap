#ifndef __VIDEO_OPTIONS_H__
#define __VIDEO_OPTIONS_H__

#include <QtWidgets/QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "PathWidget.h"
#include "VideoParameters.h"

class VideoOptions : public QWidget {
  Q_OBJECT
public:
  ~VideoOptions();
  VideoOptions(QWidget* parent);
  VideoParameters getVideoParameters(const std::string& code) const;
signals:
  void onBackPress();
  void onNextPress();
private:
  PathWidget* path;
  QSpinBox* width;
  QSpinBox* height;
  QSpinBox* fps;
  QSpinBox* bitrate;
  QSpinBox* duration;
  QPushButton* prev;
  QPushButton* next;
};

#endif // __VIDEO_OPTIONS_H__
