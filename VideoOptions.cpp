#include "VideoOptions.h"
#include <QFileDialog>

VideoOptions::~VideoOptions() {
}

VideoOptions::VideoOptions(QWidget* parent) : QWidget(parent) {
  setWindowTitle("Export");

  QWidget* central = this;//new QWidget(this);
  //setCentralWidget(central);
  QVBoxLayout* configLayout = new QVBoxLayout(central);

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
  bitrate->setMaximum(100 * 1024);
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

  QHBoxLayout* hlayout = new QHBoxLayout(this);
  configLayout->addLayout(hlayout);
  next = new QPushButton("Export", this);
  connect(next, SIGNAL(pressed()), this, SIGNAL(onExportPress()));
  hlayout->addStretch();
  hlayout->addWidget(next);
}

VideoParameters VideoOptions::getVideoParameters(const std::string& code) const {
  VideoParameters vp;
  vp.code = code;
  vp.path = path->text().toStdString();
  vp.width = width->value();
  vp.height = height->value();
  vp.fps = fps->value();
  vp.bitrate = bitrate->value();
  vp.duration = duration->value();
  return vp;
}

void VideoOptions::onBrowsePress() {
  QString pathName = QFileDialog::getSaveFileName(this,
    tr("Save File"), path->text(), tr("WebM Files (*.webm)"));
  path->setText(pathName);
}
