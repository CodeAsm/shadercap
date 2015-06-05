#include "VideoProgress.h"
#include <QTimer>

VideoProgress::~VideoProgress() {
  delete renderSurface;
}

VideoProgress::VideoProgress(const VideoParameters& videoParameters,  const ShaderParameters& shaderParameters, QWidget* parent) : QWidget(parent), vp(videoParameters), sp(shaderParameters), renderSurface(0) {
  setWindowTitle("Export");

  QWidget* central = this;//new QWidget(this);
  //setCentralWidget(central);
  QVBoxLayout* renderLayout = new QVBoxLayout(central);
  renderLayout->addStretch();

  frame = new QLabel(this);
  frame->setAlignment(Qt::AlignCenter);
  renderLayout->addWidget(frame);
  renderLayout->addStretch();

  bar = new QProgressBar(this);
  renderLayout->addWidget(bar);

  //renderLayout->addStretch();
  QHBoxLayout* hlayout = new QHBoxLayout(this);
  renderLayout->addLayout(hlayout);
  button = new QPushButton("Cancel", this);
  connect(button, SIGNAL(pressed()), this, SLOT(onFinalPress()));
  hlayout->addStretch();
  hlayout->addWidget(button);
  //hlayout->addStretch();

  finalFrame = false;
  captureDone = false;
  renderSurface = new RenderSurface(vp.width, vp.height);
  renderSurface->setShaderCode(vp.code);
  renderSurface->setShaderParameters(sp);
  encoder = new VideoEncoder(vp.path, vp.fps, vp.bitrate);
  frameCount = 0;
  durationSeconds = vp.duration;

  frame->setText("Loading resources...");
  resourceLoadIndex = 0;
  onLoadResource();
}

void VideoProgress::onLoadResource() {
  const size_t numRes = sp.size();
  if (sp[resourceLoadIndex].bind == ShaderParameter::BindTextureSampler) {
    QImage image = QImage(sp[resourceLoadIndex].texture.c_str());
    renderSurface->addTexture(sp[resourceLoadIndex].name, image);
  }
  if (++resourceLoadIndex == numRes) {
    bar->setValue(0);
    onNextFrame();
  } else {
    bar->setValue(resourceLoadIndex/float(numRes)*100);
    QTimer::singleShot(0, this, SLOT(onLoadResource()));
  }
}

void VideoProgress::onNextFrame() {
  size_t maxFrames = encoder->fpsValue() * durationSeconds;

  renderSurface->renderNow(frameCount/float(encoder->fpsValue()));
  QImage image = renderSurface->getImage();

  //if (!frameCount || frameCount % 3 == 0) {
    QImage preview = image.scaled(320,200,Qt::KeepAspectRatio,Qt::FastTransformation);
    frame->setPixmap(QPixmap::fromImage(preview));
  //}

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
      finishCapture("The capture was canceled.");
    }
  }

  if (!finalFrame) {
    QTimer::singleShot(0, this, SLOT(onNextFrame()));
  }
}

void VideoProgress::onFinalPress() {
  if (captureDone) {
    onComplete();
  } else {
    finalFrame = true;
  }
}

void VideoProgress::finishCapture(const std::string& status) {
  frame->setText(status.c_str());
  button->setText("Finish");
  captureDone = true;
}
