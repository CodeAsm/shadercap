#include "ShaderBindings.h"
#include <QHeaderView>
#include <QComboBox>
#include <assert.h>

ShaderBindings::~ShaderBindings() {
}

ShaderBindings::ShaderBindings(const std::string& code, QWidget* parent) : QWidget(parent) {
  setWindowTitle("Export");

  QWidget* central = this;//new QWidget(this);
  //setCentralWidget(central);
  QVBoxLayout* bindingsLayout = new QVBoxLayout(central);

  bindingsLayout->addWidget(new QLabel("Configure shader parameters:"));

  QTableWidget* table = new QTableWidget(central);
  bindingsLayout->addWidget(table);

  parameters = parseShaderParameters("precision highp float;\n#line 0\n" + code);

  std::string fmt;

  table->setSelectionMode(QAbstractItemView::NoSelection);
  table->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

  table->setRowCount(parameters.size());
  table->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  table->verticalHeader()->hide();

  table->setColumnCount(2);
  table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
  table->setHorizontalHeaderItem(0,new QTableWidgetItem("Name"));
  table->setHorizontalHeaderItem(1,new QTableWidgetItem("Value"));
  table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

  for (size_t i = 0; i < parameters.size(); ++i) {
    std::string text = parameters[i].name + " (" + parameters[i].type + ")";
    table->setCellWidget(i, 0, getNameWidget(text));

    QWidget* widget = 0;
    if (parameters[i].type == "float") {
      table->setCellWidget(i, 1, getFloatWidget(&widget));
    } else if (parameters[i].type == "vec2") {
      table->setCellWidget(i, 1, getVec2Widget(&widget));
    } else if (parameters[i].type == "sampler2D") {
      widget = new PathWidget(false, this);
      table->setCellWidget(i, 1, widget);
    } else {
      table->setCellWidget(i, 1, getUnsupportedWidget(&widget));
    }

    parameterWidgets.push_back(widget);
  }

  assert(parameterWidgets.size() == parameters.size());

  bindingsLayout->addWidget(new QLabel(fmt.c_str(), central));

  QHBoxLayout* buttonsLayout = new QHBoxLayout(central);
  QPushButton* prev = new QPushButton("Previous", central);
  connect(prev, SIGNAL(pressed()), this, SIGNAL(onBackPress()));
  buttonsLayout->addWidget(prev);
  buttonsLayout->addStretch();
  QPushButton* next = new QPushButton("Next", central);
  connect(next, SIGNAL(pressed()), this, SLOT(onNextPressInternal()));
  buttonsLayout->addWidget(next);

  bindingsLayout->addLayout(buttonsLayout);

  adjustSize();
}

std::vector<ShaderParameter> ShaderBindings::getShaderParameters() const {
  return parameters;
}

void ShaderBindings::onNextPressInternal() {
  assert(parameterWidgets.size() == parameters.size());
  for (size_t i = 0; i < parameters.size(); ++i) {
    if (parameters[i].type == "float") {
      QComboBox* box = dynamic_cast<QComboBox*>(parameterWidgets[i]);
      assert(box);
      switch (box->currentIndex()) {
      case 0:
        parameters[i].bind = ShaderParameter::BindGlobalTime;
        break;
      default:
        parameters[i].bind = ShaderParameter::BindUnbound;
        break;
      }
    } else if (parameters[i].type == "vec2") {
      QComboBox* box = dynamic_cast<QComboBox*>(parameterWidgets[i]);
      assert(box);
      int texIndex = 0;
      bool gotTex = false;
      switch (box->currentIndex()) {
      case 0:
        parameters[i].bind = ShaderParameter::BindOutputResolution;
        break;
      default:
        parameters[i].bind = ShaderParameter::BindTextureResolution;
        texIndex = box->itemData(box->currentIndex()).toInt(&gotTex);
        assert(gotTex);
        parameters[i].texture = parameters[texIndex].name;
        break;
      }
    } else if (parameters[i].type == "sampler2D") {
      PathWidget* path = dynamic_cast<PathWidget*>(parameterWidgets[i]);
      assert(path);
      parameters[i].bind = ShaderParameter::BindTextureSampler;
      parameters[i].texture = path->getPath();
    } else {
      parameters[i].bind = ShaderParameter::BindUnbound;
    }
  }
  onNextPress();
}

QWidget* ShaderBindings::getNameWidget(const std::string& name) {
  QWidget* parent = new QWidget(this);
  QHBoxLayout* layout = new QHBoxLayout(parent);

  QLabel* widget = new QLabel(name.c_str(), this);
  widget->setAlignment(Qt::AlignCenter);

  layout->addWidget(widget);
  return parent;
}

QWidget* ShaderBindings::getFloatWidget(QWidget** result) {
  QWidget* parent = new QWidget(this);
  QHBoxLayout* layout = new QHBoxLayout(parent);

  QComboBox* widget = new QComboBox(this);
  widget->addItem("Elapsed time (seconds)");

  layout->addWidget(widget);
  *result = widget;
  return parent;
}

QWidget* ShaderBindings::getVec2Widget(QWidget** result) {
  QWidget* parent = new QWidget(this);
  QHBoxLayout* layout = new QHBoxLayout(parent);

  QComboBox* widget = new QComboBox(this);

  widget->addItem("Output resolution");

  for (size_t i = 0; i < parameters.size(); ++i) {
    if (parameters[i].type == "sampler2D") {
      std::string text = "Texture resolution (" + parameters[i].name + ")";
      widget->addItem(text.c_str(), QVariant(int(i)));
    }
  }

  layout->addWidget(widget);
  *result = widget;
  return parent;
}

QWidget* ShaderBindings::getUnsupportedWidget(QWidget** result) {
  QWidget* parent = new QWidget(this);
  QHBoxLayout* layout = new QHBoxLayout(parent);

  QComboBox* widget = new QComboBox(this);
  widget->addItem("Not editable");
  widget->setEnabled(false);
  layout->addWidget(widget);

  *result = widget;
  return parent;
}
