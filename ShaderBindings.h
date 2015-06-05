#ifndef __SHADER_BINDINGS_H__
#define __SHADER_BINDINGS_H__

#include <QtWidgets/QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include "PathWidget.h"
#include "ShaderParameter.h"

class ShaderBindings : public QWidget {
  Q_OBJECT
public:
  ~ShaderBindings();
  ShaderBindings(const std::string& code, QWidget* parent);
  std::vector<ShaderParameter> getShaderParameters() const;
public slots:
  void onNextPressInternal();
signals:
  void onBackPress();
  void onNextPress();
private:
  QWidget* getFloatWidget(QWidget** result);
  QWidget* getVec2Widget(QWidget** result);
  std::vector<ShaderParameter> parameters;
  std::vector<QWidget*> parameterWidgets;
};

#endif // __SHADER_BINDINGS_H__
