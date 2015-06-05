#include "PathWidget.h"
#include <QHBoxLayout>
#include <QFileDialog>

PathWidget::~PathWidget() {
}

PathWidget::PathWidget(bool save, QWidget* parent) : QWidget(parent), saveDialog(save) {
  QHBoxLayout* layout = new QHBoxLayout(this);

  path = new QLineEdit(this);
  connect(path, SIGNAL(textChanged(const QString&)), this, SLOT(onTextChanged(const QString&)));
  layout->addWidget(path);

  browse = new QPushButton("...", this);
  connect(browse, SIGNAL(pressed()), this, SLOT(onBrowsePress()));
  layout->addWidget(browse);

  checkPath();
}

void PathWidget::setPath(const std::string& text) {
  path->setText(text.c_str());
}

std::string PathWidget::getPath() const {
  return path->text().toStdString();
}

void PathWidget::onBrowsePress() {
  QString pathName;
  if(saveDialog) {
    pathName = QFileDialog::getSaveFileName(this,
      tr("Save File"), path->text(), tr("WebM Files (*.webm)"));
  } else {
    pathName = QFileDialog::getOpenFileName(this,
      tr("Open File"), path->text(), tr("Image files (*.png *.jpg *.bmp)"));
  }
  path->setText(pathName);
}

void PathWidget::onTextChanged(const QString& text) {
  checkPath();
}

void PathWidget::checkPath() {
  if (!saveDialog) {
    QFileInfo checkFile(path->text());
    if (!checkFile.exists() || checkFile.isDir()) {
      path->setStyleSheet("QLineEdit {background-color: indianred;}");
    } else {
      path->setStyleSheet("QLineEdit {background-color: lightgreen;}");
    }
  }
}
