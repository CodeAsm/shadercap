#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QProgressBar>
#include <QLineEdit>
#include "RenderSurface.h"
#include "VideoEncoder.h"

class CanvasWidget;
class CameraControlWidget;
class CodeEditor;

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  ~MainWindow();
  MainWindow();
public slots:
  void onConfigurePress();
  void onBrowsePress();
  void onRenderPress();
  void onNextFrame();
  void onFinalPress();
public:
  QWidget* codeWidget;
  CodeEditor* code;
  QLabel* preview;

  QWidget* configWidget;
  QLineEdit* path;
  QSpinBox* width;
  QSpinBox* height;
  QSpinBox* fps;
  QSpinBox* bitrate;
  QSpinBox* duration;

  QWidget* renderWidget;
  QLabel* frame;
  QProgressBar* bar;
  QPushButton* button;

  RenderSurface* renderSurface;
  VideoEncoder* encoder;

  bool finalFrame;
  size_t frameCount;
  size_t durationSeconds;
  std::string shaderProgram;
};

#endif // __MAIN_WINDOW_H__
