#ifndef __PATH_WIDGET_H__
#define __PATH_WIDGET_H__

#include <QtWidgets/QWidget>
#include <QLineEdit>
#include <QPushButton>

class PathWidget : public QWidget {
  Q_OBJECT
public:
  ~PathWidget();
  PathWidget(bool save, QWidget* parent);
  void setPath(const std::string& text);
  std::string getPath() const;
public slots:
  void onBrowsePress();
  void onTextChanged(const QString& text);
private:
  void checkPath();
  QLineEdit* path;
  QPushButton* browse;
  bool saveDialog;
};

#endif // __PATH_WIDGET_H__
