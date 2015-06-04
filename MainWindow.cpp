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
  setWindowTitle("ShaderCap 1.0 by dila");

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
  QPushButton* next = new QPushButton("Compile", this);
  connect(next, SIGNAL(pressed()), this, SLOT(onConfigurePress()));
  hlayout->addStretch();
  hlayout->addWidget(next);

  configWidget = new QWidget(this);
  QVBoxLayout* configLayout = new QVBoxLayout(configWidget);
  configWidget->hide();

  videoOptions = new VideoOptions(this);
  configLayout->addWidget(videoOptions);
  connect(videoOptions, SIGNAL(onExportPress()), this, SLOT(onRenderPress()));

  show();
}

std::string getShaderParameters(const std::string& code) {
  std::vector<ShaderParameter> params = parseShaderParameters(code);
  std::string ret;
  for (size_t i = 0; i < params.size(); ++i) {
    ret += params[i].uniform?"uniform":"varying";
    ret += " " + params[i].type;
    ret += " " + params[i].name;
    ret += (i!=(params.size()-1)?"\n":"");
  }
  return ret;
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
    preview->hide();
    codeWidget->hide();
    setCentralWidget(configWidget);
    configWidget->show();
  } else {
    if (shaderError.empty()) {
      shaderError = "Error: The shader could not be compiled.";
    }
    preview->setText(shaderError.c_str());
    preview->show();
  }
}

void MainWindow::onRenderPress() {
  configWidget->hide();

  QWidget* renderWidget = new QWidget(this);
  QVBoxLayout* renderLayout = new QVBoxLayout(renderWidget);

  renderLayout->addWidget(new VideoProgress(videoOptions->getVideoParameters(shaderProgram), renderWidget));

  setCentralWidget(renderWidget);
}
