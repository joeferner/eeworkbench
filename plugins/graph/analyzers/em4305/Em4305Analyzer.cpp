#include "Em4305Analyzer.h"
#include "Em4305AnalyzerConfigDialog.h"
#include "../../GraphWidgetPluginInstance.h"
#include "../../GraphAnalyzerInstance.h"
#include "Em4305AnalyzerInstance.h"

Em4305Analyzer::Em4305Analyzer() {
}

GraphAnalyzerInstance* Em4305Analyzer::configure(GraphWidget* graphWidget, GraphWidgetPluginInstance* graphWidgetPluginInstance) {
  Em4305AnalyzerConfigDialog dlg(graphWidgetPluginInstance);
  if(dlg.exec() == QDialog::Accepted) {
    GraphSignal* toDeviceSignal = dlg.getToDeviceSignal();
    return new Em4305AnalyzerInstance(getName(), toDeviceSignal);
  }
  return NULL;
}
