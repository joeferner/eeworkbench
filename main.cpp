#include "MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {
  QApplication a(argc, argv);

  QCoreApplication::setOrganizationName("EEWorkbench");
  QCoreApplication::setOrganizationDomain("eeworkbench.org");
  QCoreApplication::setApplicationName("EEWorkbench");

  MainWindow w;
  w.show();

  return a.exec();
}
