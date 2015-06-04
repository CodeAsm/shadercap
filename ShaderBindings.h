#ifndef __SHADER_BINDINGS_H__
#define __SHADER_BINDINGS_H__

#include <QtWidgets/QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "ShaderParameter.h"

class ShaderBindings : public QWidget {
  Q_OBJECT
public:
  ~ShaderBindings();
  ShaderBindings(const std::string& code, QWidget* parent);
signals:
  void onNextPress();
private:
};

#endif // __SHADER_BINDINGS_H__
