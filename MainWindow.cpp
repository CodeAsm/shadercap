#include "MainWindow.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QStyle>
#include <QDesktopWidget>
#include <QApplication>
#include "CodeEditor.h"
#include "ShaderParameter.h"
#include "VideoProgress.h"

MainWindow::~MainWindow() {
}

MainWindow::MainWindow() {
  setWindowTitle("ShaderCap 1.1 by dila");
  resize(576, 360);
  layoutInitialView();
  centerWindow(false);
  show();
}

void MainWindow::centerWindow(bool resize) {
  if (resize) {
    adjustSize();
  }
  setGeometry(
   QStyle::alignedRect(
   Qt::LeftToRight,
   Qt::AlignCenter,
   size(),
   qApp->desktop()->availableGeometry()
   ));
}

void MainWindow::layoutInitialView() {
  setCentralWidget(new QWidget(this));
  codeLayout = new QVBoxLayout(centralWidget());

  codeLayout->addWidget(new QLabel("Input your shader program:"));
  code = new CodeEditor(this);
  code->document()->setPlainText(shaderProgram.c_str());
  codeLayout->addWidget(code);

  preview = new QLabel(this);
  preview->setStyleSheet("QLabel { color : red; }");
  preview->hide();
  codeLayout->addWidget(preview);

  QHBoxLayout* hlayout = new QHBoxLayout(this);
  codeLayout->addLayout(hlayout);
  QPushButton* next = new QPushButton("Next", this);
  connect(next, SIGNAL(pressed()), this, SLOT(onConfigurePress()));
  hlayout->addStretch();
  hlayout->addWidget(next);
}

void MainWindow::onConfigurePress() {
  QString program = code->document()->toPlainText();
  shaderProgram = program.toStdString();
  std::string shaderError;
  parseShaderParameters(shaderProgram, &shaderError);
  if (shaderError.empty()) {
    onLayoutBindingsView();
  } else {
    preview->setText(shaderError.c_str());
    preview->show();
  }
}

void MainWindow::onLayoutBindingsView() {
  setCentralWidget(new QWidget(this));
  bindingsLayout = new QVBoxLayout(centralWidget());
  shaderBindings = new ShaderBindings(shaderProgram, this);
  connect(shaderBindings, SIGNAL(onNextPress()), this, SLOT(onBindingsNextPress()));
  connect(shaderBindings, SIGNAL(onBackPress()), this, SLOT(onBindingsBackPress()));
  bindingsLayout->addWidget(shaderBindings);
  //shaderBindings->setParent(this);
}

void MainWindow::onBindingsNextPress() {
  shaderBindings->setParent(this);
  setCentralWidget(new QWidget(this));
  configLayout = new QVBoxLayout(centralWidget());
  videoOptions = new VideoOptions(this);
  configLayout->addWidget(videoOptions);
  connect(videoOptions, SIGNAL(onNextPress()), this, SLOT(onRenderPress()));
  connect(videoOptions, SIGNAL(onBackPress()), this, SLOT(onConfigureBackPress()));
  //centerWindow();
}

void MainWindow::onBindingsBackPress() {
  layoutInitialView();
}

void MainWindow::onConfigureBackPress() {
  setCentralWidget(new QWidget(this));
  bindingsLayout = new QVBoxLayout(centralWidget());
  bindingsLayout->addWidget(shaderBindings);
}

void MainWindow::onRenderPress() {
  setCentralWidget(new QWidget(this));
  renderLayout = new QVBoxLayout(centralWidget());
  ShaderParameters sp = shaderBindings->getShaderParameters();
  VideoParameters vp = videoOptions->getVideoParameters(shaderProgram);
  VideoProgress* videoProgress = new VideoProgress(vp, sp, this);
  connect(videoProgress, SIGNAL(onComplete()), this, SLOT(onRenderComplete()));
  renderLayout->addWidget(videoProgress);
  //centerWindow();
}

void MainWindow::onRenderComplete() {
  close();
}
