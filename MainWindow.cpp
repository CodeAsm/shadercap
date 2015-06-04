#include "MainWindow.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include "CodeEditor.h"
#include "ShaderParameter.h"
#include "VideoProgress.h"

MainWindow::~MainWindow() {
}

MainWindow::MainWindow() {
  setWindowTitle("ShaderCap 1.1 by dila");

  codeWidget = new QWidget(this);
  setCentralWidget(codeWidget);
  QVBoxLayout* layout = new QVBoxLayout(codeWidget);

  layout->addWidget(new QLabel("Input your shader program:"));

  code = new CodeEditor(this);
  layout->addWidget(code);

  preview = new QLabel(this);
  preview->setStyleSheet("QLabel { color : red; }");
  preview->hide();
  layout->addWidget(preview);

  QHBoxLayout* hlayout = new QHBoxLayout(this);
  layout->addLayout(hlayout);
  QPushButton* next = new QPushButton("Next", this);
  connect(next, SIGNAL(pressed()), this, SLOT(onConfigurePress()));
  hlayout->addStretch();
  hlayout->addWidget(next);

  bindingsWidget = new QWidget(this);
  bindingsWidget->hide();

  configWidget = new QWidget(this);
  QVBoxLayout* configLayout = new QVBoxLayout(configWidget);
  configWidget->hide();
  videoOptions = new VideoOptions(this);
  configLayout->addWidget(videoOptions);
  connect(videoOptions, SIGNAL(onExportPress()), this, SLOT(onRenderPress()));

  show();
}

void MainWindow::onConfigurePress() {
  renderSurface = new RenderSurface(128, 128);
  renderSurface->renderNow(0.0f);
  QString program = code->document()->toPlainText();
  shaderProgram = program.toStdString();
  bool shaderRet = renderSurface->setShaderCode(shaderProgram);
  std::string shaderError = renderSurface->getShaderError();
  delete renderSurface;
  renderSurface = 0;
  if (shaderRet) {
    codeWidget->hide();

    QVBoxLayout* bindingsLayout = new QVBoxLayout(bindingsWidget);
    shaderBindings = new ShaderBindings(shaderProgram, this);
    connect(shaderBindings, SIGNAL(onNextPress()), this, SLOT(onBindingsPress()));
    bindingsLayout->addWidget(shaderBindings);

    bindingsWidget->show();
    setCentralWidget(bindingsWidget);
    adjustSize();
  } else {
    if (shaderError.empty()) {
      shaderError = "Error: The shader could not be compiled.";
    }
    preview->setText(shaderError.c_str());
    preview->show();
  }
}

void MainWindow::onBindingsPress() {
  bindingsWidget->hide();
  setCentralWidget(configWidget);
  configWidget->show();
  adjustSize();
}

void MainWindow::onRenderPress() {
  configWidget->hide();

  QWidget* renderWidget = new QWidget(this);
  QVBoxLayout* renderLayout = new QVBoxLayout(renderWidget);

  renderLayout->addWidget(new VideoProgress(videoOptions->getVideoParameters(shaderProgram), renderWidget));

  setCentralWidget(renderWidget);

  adjustSize();
}
