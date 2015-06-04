#ifndef __VIDEO_OPTIONS_H__
#define __VIDEO_OPTIONS_H__

#include <QtWidgets/QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "VideoParameters.h"

class VideoOptions : public QWidget {
  Q_OBJECT
public:
  ~VideoOptions();
  VideoOptions(QWidget* parent);
  VideoParameters getVideoParameters(const std::string& code) const;
public slots:
  void onBrowsePress();
signals:
  void onExportPress();
private:
  QLineEdit* path;
  QSpinBox* width;
  QSpinBox* height;
  QSpinBox* fps;
  QSpinBox* bitrate;
  QSpinBox* duration;
  QPushButton* next;
};

#endif // __VIDEO_OPTIONS_H__
