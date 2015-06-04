#include "ShaderBindings.h"

ShaderBindings::~ShaderBindings() {
}

ShaderBindings::ShaderBindings(const std::string& code, QWidget* parent) : QWidget(parent) {
  setWindowTitle("Export");

  QWidget* central = this;//new QWidget(this);
  //setCentralWidget(central);
  QVBoxLayout* bindingsLayout = new QVBoxLayout(central);

  bindingsLayout->addWidget(new QLabel("Set bindings:", central));

  bindingsLayout->addStretch();

  QPushButton* next = new QPushButton("Next", central);
  connect(next, SIGNAL(pressed()), this, SIGNAL(onNextPress()));
  QHBoxLayout* nextLayout = new QHBoxLayout(central);
  nextLayout->addStretch();
  nextLayout->addWidget(next);
  bindingsLayout->addLayout(nextLayout);
}
