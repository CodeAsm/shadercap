#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QProgressBar>
#include <QLineEdit>
#include "VideoOptions.h"
#include "RenderSurface.h"

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
  void onRenderPress();
private:
  QWidget* codeWidget;
  CodeEditor* code;
  QLabel* preview;

  QWidget* configWidget; /* VideoOptions */
  VideoOptions* videoOptions;

  RenderSurface* renderSurface;
  std::string shaderProgram;
};

#endif // __MAIN_WINDOW_H__
