#include "MainWindow.h"
#include <QApplication>
#include <QCommandLineParser>

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  QCoreApplication::setOrganizationName("EEWorkbench");
  QCoreApplication::setOrganizationDomain("eeworkbench.org");
  QCoreApplication::setApplicationName("EEWorkbench");

  QCommandLineParser parser;
  parser.setApplicationDescription("EEWorkbench");
  parser.addHelpOption();
  parser.addVersionOption();

  parser.addPositionalArgument("file", QCoreApplication::translate("main", "File to load on start."));

  parser.process(app);

  const QStringList args = parser.positionalArguments();

  MainWindow w;
  w.show();
  if(args.length() > 0) {
    w.load(args.at(0));
  }

  return app.exec();
}
