#ifndef EM4305ANALYZERCONFIGDIALOG_H
#define EM4305ANALYZERCONFIGDIALOG_H

#include <QDialog>

class GraphWidgetPluginInstance;
class GraphSignal;

namespace Ui {
class Em4305AnalyzerConfigDialog;
}

class Em4305AnalyzerConfigDialog : public QDialog {
  Q_OBJECT

public:
  explicit Em4305AnalyzerConfigDialog(GraphWidgetPluginInstance* graphWidgetPluginInstance);
  ~Em4305AnalyzerConfigDialog();

  GraphSignal* getToDeviceSignal() const;

private:
  Ui::Em4305AnalyzerConfigDialog* ui;
};

#endif // EM4305ANALYZERCONFIGDIALOG_H
