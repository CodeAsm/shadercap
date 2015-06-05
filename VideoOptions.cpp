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
  configLayout->addStretch();

  QHBoxLayout* pathLayout = new QHBoxLayout(this);
  configLayout->addLayout(pathLayout);
  pathLayout->addStretch();
  pathLayout->addWidget(new QLabel("Output:", this));
  path = new PathWidget(true, this);
  path->setPath("capture.webm");
  pathLayout->addWidget(path);
  pathLayout->addStretch();

  QHBoxLayout* widthLayout = new QHBoxLayout(this);
  configLayout->addLayout(widthLayout);
  widthLayout->addStretch();
  widthLayout->addWidget(new QLabel("Width:", this));
  width = new QSpinBox(this);
  width->setMaximum(65536);
  width->setValue(1280);
  widthLayout->addWidget(width);
  widthLayout->addWidget(new QLabel("pixels"));
  widthLayout->addStretch();

  QHBoxLayout* heightLayout = new QHBoxLayout(this);
  configLayout->addLayout(heightLayout);
  heightLayout->addStretch();
  QLabel* heightLabel = new QLabel("Height:", this);
  heightLayout->addWidget(heightLabel);
  height = new QSpinBox(this);
  height->setMaximum(65536);
  height->setValue(720);
  heightLayout->addWidget(height);
  heightLayout->addWidget(new QLabel("pixels"));
  heightLayout->addStretch();

  QHBoxLayout* fpsLayout = new QHBoxLayout(this);
  configLayout->addLayout(fpsLayout);
  fpsLayout->addStretch();
  QLabel* fpsLabel = new QLabel("Framerate:", this);
  fpsLayout->addWidget(fpsLabel);
  fps = new QSpinBox(this);
  fps->setMaximum(500);
  fps->setValue(60);
  fpsLayout->addWidget(fps);
  fpsLayout->addWidget(new QLabel("fps"));
  fpsLayout->addStretch();

  QHBoxLayout* brLayout = new QHBoxLayout(this);
  configLayout->addLayout(brLayout);
  brLayout->addStretch();
  QLabel* brLabel = new QLabel("Bitrate:", this);
  brLayout->addWidget(brLabel);
  bitrate = new QSpinBox(this);
  bitrate->setMaximum(100 * 1024);
  bitrate->setValue(5000);
  brLayout->addWidget(bitrate);
  brLayout->addWidget(new QLabel("kbps"));
  brLayout->addStretch();

  QHBoxLayout* durLayout = new QHBoxLayout(this);
  configLayout->addLayout(durLayout);
  durLayout->addStretch();
  QLabel* durLabel = new QLabel("Duration:", this);
  durLayout->addWidget(durLabel);
  duration = new QSpinBox(this);
  fps->setMaximum(10 * 60 * 60);
  duration->setValue(30);
  durLayout->addWidget(duration);
  durLayout->addWidget(new QLabel("seconds"));
  durLayout->addStretch();

  configLayout->addStretch();

  QHBoxLayout* hlayout = new QHBoxLayout(this);
  configLayout->addLayout(hlayout);
  prev = new QPushButton("Previous", this);
  connect(prev, SIGNAL(pressed()), this, SIGNAL(onBackPress()));
  hlayout->addWidget(prev);
  hlayout->addStretch();
  next = new QPushButton("Render", this);
  connect(next, SIGNAL(pressed()), this, SIGNAL(onNextPress()));
  hlayout->addWidget(next);
}

VideoParameters VideoOptions::getVideoParameters(const std::string& code) const {
  VideoParameters vp;
  vp.code = code;
  vp.path = path->getPath();
  vp.width = width->value();
  vp.height = height->value();
  vp.fps = fps->value();
  vp.bitrate = bitrate->value();
  vp.duration = duration->value();
  return vp;
}
