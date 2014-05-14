#include "Em4305Analyzer.h"
#include "Em4305AnalyzerConfigDialog.h"
#include "../../GraphWidgetPluginInstance.h"
#include "../../GraphAnalyzerInstance.h"
#include "Em4305AnalyzerInstance.h"
#include <QDebug>

Em4305Analyzer::Em4305Analyzer() {
}

GraphAnalyzerInstance* Em4305Analyzer::configure(GraphWidget*, GraphWidgetPluginInstance* graphWidgetPluginInstance) {
  Em4305AnalyzerConfigDialog dlg(graphWidgetPluginInstance);
  if(dlg.exec() == QDialog::Accepted) {
    GraphSignal* toDeviceSignal = dlg.getToDeviceSignal();
    int toDeviceSignalIndex = graphWidgetPluginInstance->getSignalIndex(toDeviceSignal);
    return new Em4305AnalyzerInstance(getName(), toDeviceSignalIndex);
  }
  return NULL;
}

GraphAnalyzerInstance* Em4305Analyzer::create(GraphWidget*, GraphWidgetPluginInstance*, const QString& config) {
  int toDeviceSignalIndex = -1;
  QStringList args = config.split(';');
  foreach(QString arg, args) {
    int i = arg.indexOf('=');
    QString name = arg.mid(0, i);
    QString value = arg.mid(i + 1);
    if(name == "toDevice") {
      toDeviceSignalIndex = value.toInt();
    } else {
      qDebug() << "Em4305Analyzer: Invalid config parameter: " << arg;
    }
  }
  if(toDeviceSignalIndex == -1) {
    qDebug() << "Em4305Analyzer: Missing toDevice configuration";
    return NULL;
  }
  return new Em4305AnalyzerInstance(getName(), toDeviceSignalIndex);
}
