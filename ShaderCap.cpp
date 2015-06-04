#include <QApplication>

#include <SymbolTable.h>
#include <ParseHelper.h>
#include <Scan.h>
#include <ScanContext.h>
#include <InitializeDll.h>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
  ShInitialize();
  QApplication a(argc, argv);
  MainWindow main;
  int ret = a.exec();
  ShFinalize();
  return ret;
}
