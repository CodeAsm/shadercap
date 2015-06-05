#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QtWidgets/QMainWindow>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QProgressBar>
#include <QLineEdit>
#include "ShaderBindings.h"
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
  void centerWindow(bool resize = true);
private:
  void layoutInitialView();
public slots:
  void onConfigurePress();
  void onLayoutBindingsView();
  void onBindingsNextPress();
  void onBindingsBackPress();
  void onConfigureBackPress();
  void onRenderPress();
  void onRenderComplete();
private:
  QVBoxLayout* codeLayout;
  CodeEditor* code;
  QLabel* preview;

  QVBoxLayout* bindingsLayout;
  ShaderBindings* shaderBindings;

  QVBoxLayout* configLayout;
  VideoOptions* videoOptions;

  QVBoxLayout* renderLayout;
  RenderSurface* renderSurface;
  std::string shaderProgram;
};

#endif // __MAIN_WINDOW_H__
