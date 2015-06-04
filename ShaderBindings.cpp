#include "ShaderBindings.h"

ShaderBindings::~ShaderBindings() {
}

ShaderBindings::ShaderBindings(const std::string& code, QWidget* parent) : QWidget(parent) {
  setWindowTitle("Export");

  QWidget* central = this;//new QWidget(this);
  //setCentralWidget(central);
  QVBoxLayout* bindingsLayout = new QVBoxLayout(central);

  bindingsLayout->addWidget(new QLabel("Configure shader parameters:"));

  std::vector<ShaderParameter> parameters = parseShaderParameters("precision highp float;\n" + code);

  std::string fmt;

  for (size_t i = 0; i < parameters.size(); ++i) {
    QPushButton* btn = new QPushButton(parameters[i].name.c_str(), this);
    bindingsLayout->addWidget(btn);
  }

  bindingsLayout->addWidget(new QLabel(fmt.c_str(), central));

  bindingsLayout->addStretch();

  QPushButton* next = new QPushButton("Next", central);
  connect(next, SIGNAL(pressed()), this, SIGNAL(onNextPress()));
  QHBoxLayout* nextLayout = new QHBoxLayout(central);
  nextLayout->addStretch();
  nextLayout->addWidget(next);
  bindingsLayout->addLayout(nextLayout);
}
