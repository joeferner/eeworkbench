#include "Em4305AnalyzerConfigDialog.h"
#include "ui_Em4305AnalyzerConfigDialog.h"
#include "../../GraphWidgetPluginInstance.h"
#include <QVariant>

Em4305AnalyzerConfigDialog::Em4305AnalyzerConfigDialog(GraphWidgetPluginInstance* graphWidgetPluginInstance) :
  QDialog(0),
  ui(new Ui::Em4305AnalyzerConfigDialog) {
  ui->setupUi(this);

  QList<GraphSignal*> graphSignals = graphWidgetPluginInstance->getSignals();
  for(int i = 0; i < graphSignals.length(); i++) {
    const GraphSignal* signal = graphSignals.at(i);
    ui->toDeviceSignal->addItem(signal->name, QVariant::fromValue((void*)signal));
  }
}

Em4305AnalyzerConfigDialog::~Em4305AnalyzerConfigDialog() {
  delete ui;
}

GraphSignal* Em4305AnalyzerConfigDialog::getToDeviceSignal() const {
  return (GraphSignal*)ui->toDeviceSignal->currentData().value<void*>();
}
