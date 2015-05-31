#include "MainWindow.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QFileDialog>
#include "CodeEditor.h"

MainWindow::~MainWindow() {
  delete renderSurface;
}

MainWindow::MainWindow() : renderSurface(0) {
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

  configLayout->addWidget(new QLabel("Set video encoder options:"));

  QHBoxLayout* pathLayout = new QHBoxLayout(this);
  configLayout->addLayout(pathLayout);
  pathLayout->addWidget(new QLabel("Output:", this));
  path = new QLineEdit(this);
  path->setText("capture.webm");
  pathLayout->addWidget(path);
  QPushButton* browse = new QPushButton("...", this);
  connect(browse, SIGNAL(pressed()), this, SLOT(onBrowsePress()));
  pathLayout->addWidget(browse);
  pathLayout->addStretch();

  QHBoxLayout* widthLayout = new QHBoxLayout(this);
  configLayout->addLayout(widthLayout);
  widthLayout->addWidget(new QLabel("Width:", this));
  width = new QSpinBox(this);
  width->setMaximum(65536);
  width->setValue(720);
  widthLayout->addWidget(width);
  widthLayout->addWidget(new QLabel("pixels"));

  QHBoxLayout* heightLayout = new QHBoxLayout(this);
  configLayout->addLayout(heightLayout);
  QLabel* heightLabel = new QLabel("Height:", this);
  heightLayout->addWidget(heightLabel);
  height = new QSpinBox(this);
  height->setMaximum(65536);
  height->setValue(480);
  heightLayout->addWidget(height);
  heightLayout->addWidget(new QLabel("pixels"));

  QHBoxLayout* fpsLayout = new QHBoxLayout(this);
  configLayout->addLayout(fpsLayout);
  QLabel* fpsLabel = new QLabel("Framerate:", this);
  fpsLayout->addWidget(fpsLabel);
  fps = new QSpinBox(this);
  fps->setMaximum(60);
  fps->setValue(30);
  fpsLayout->addWidget(fps);
  fpsLayout->addWidget(new QLabel("fps"));

  QHBoxLayout* brLayout = new QHBoxLayout(this);
  configLayout->addLayout(brLayout);
  QLabel* brLabel = new QLabel("Bitrate:", this);
  brLayout->addWidget(brLabel);
  bitrate = new QSpinBox(this);
  bitrate->setMaximum(10 * 1024);
  bitrate->setValue(500);
  brLayout->addWidget(bitrate);
  brLayout->addWidget(new QLabel("kbps"));

  QHBoxLayout* durLayout = new QHBoxLayout(this);
  configLayout->addLayout(durLayout);
  QLabel* durLabel = new QLabel("Duration:", this);
  durLayout->addWidget(durLabel);
  duration = new QSpinBox(this);
  fps->setMaximum(10 * 60 * 60);
  duration->setValue(60);
  durLayout->addWidget(duration);
  durLayout->addWidget(new QLabel("seconds"));

  configLayout->addStretch();

  hlayout = new QHBoxLayout(this);
  configLayout->addLayout(hlayout);
  next = new QPushButton("Capture", this);
  connect(next, SIGNAL(pressed()), this, SLOT(onRenderPress()));
  hlayout->addStretch();
  hlayout->addWidget(next);

  renderWidget = new QWidget(this);
  QVBoxLayout* renderLayout = new QVBoxLayout(renderWidget);
  renderWidget->hide();

  frame = new QLabel(this);
  frame->setAlignment(Qt::AlignCenter);
  renderLayout->addWidget(frame);

  bar = new QProgressBar(this);
  renderLayout->addWidget(bar);

  hlayout = new QHBoxLayout(this);
  renderLayout->addLayout(hlayout);
  button = new QPushButton("Cancel", this);
  connect(button, SIGNAL(pressed()), this, SLOT(onFinalPress()));
  hlayout->addStretch();
  hlayout->addWidget(button);

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

void MainWindow::onBrowsePress() {
  QString pathName = QFileDialog::getSaveFileName(this,
    tr("Save File"), path->text(), tr("WebM Files (*.webm)"));
  path->setText(pathName);
}

void MainWindow::onRenderPress() {
  configWidget->hide();
  setCentralWidget(renderWidget);
  renderWidget->show();
  finalFrame = false;
  renderSurface = new RenderSurface(width->value(), height->value());
  renderSurface->setShaderCode(shaderProgram);
  encoder = new VideoEncoder(path->text().toStdString(), fps->value(), bitrate->value());
  frameCount = 0;
  durationSeconds = duration->value();
  onNextFrame();
}

void MainWindow::onNextFrame() {
  size_t maxFrames = encoder->fpsValue() * durationSeconds;

  renderSurface->renderNow(frameCount/float(encoder->fpsValue()));
  QImage image = renderSurface->getImage();

  QImage preview = image.scaled(320,200,Qt::KeepAspectRatio,Qt::FastTransformation);
  frame->setPixmap(QPixmap::fromImage(preview));

  image = image.convertToFormat(QImage::Format_RGB888);

  vpx_image_t* vpxImage = vpx_img_alloc(NULL, VPX_IMG_FMT_RGB24, renderSurface->width(), renderSurface->height(), 1);
  if (!vpxImage) {
    finishCapture("An error occured during capture: Out of memory.");
    return;
  }

  size_t size = renderSurface->width() * renderSurface->height() * 3;
  if (size != size_t(image.byteCount())) {
    finishCapture("An error occured during capture: Image size error.");
    vpx_img_free(vpxImage);
    return;
  }

  memcpy(vpxImage->img_data, image.bits(), size);

  if (!encoder->writeFrame(vpxImage)) {
    finishCapture("An error occured during capture: Frame write error.");
    vpx_img_free(vpxImage);
    return;
  }

  if (vpxImage) {
    vpx_img_free(vpxImage);
  }

  bool complete = false;
  if (++frameCount == maxFrames) {
    finalFrame = true;
    complete = true;
  } else {
    bar->setValue(frameCount/float(maxFrames)*100);
  }

  if (finalFrame) {
    bar->setValue(100);
    encoder->finish();
    delete encoder;
    if (complete) {
      finishCapture("The capture is complete.");
    } else {
      finishCapture("The capture was stopped before it finished.");
    }
  }

  if (!finalFrame) {
    QTimer::singleShot(0, this, SLOT(onNextFrame()));
  }
}

void MainWindow::onFinalPress() {
  finalFrame = true;
}

void MainWindow::finishCapture(const std::string& status) {
  frame->setText(status.c_str());
  button->setEnabled(false);
}
