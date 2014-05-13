#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QStringList>
#include <QLabel>
#include <QMap>
#include <QComboBox>
#include <QFile>
#include "plugins/PluginManager.h"
#include "plugins/WidgetPluginInstance.h"

namespace Ui {
class MainWindow;
}

class MainWindow :
  public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow();

protected:
  virtual void closeEvent(QCloseEvent* event);

private slots:
  void on_actionConnect_triggered();
  void onInputPluginConnected();
  void onInputPluginDisconnected();
  void onInputPluginReadyRead();

  void on_actionExit_triggered();

  void on_actionSave_triggered();

private:
  Ui::MainWindow* m_ui;
  QGridLayout* m_layout;
  PluginManager m_pluginManager;
  QLabel* m_descriptionLabel;
  QComboBox* m_inputSelectComboBox;
  InputPlugin* m_connectedInputPlugin;
  QMap<QString, WidgetPluginInstance*> m_widgets;
  QString m_inputCurrentLine;

  void clearWidgets();
  void closeConnectedInputPlugin();
  void windowCommand(const QString& functionName, QStringList args);
  void widgetCommand(const QString& scope, const QString& functionName, QStringList args);
  void runCommand(const QString& command);
  void runCommand(const QString& scope, const QString& functionName, QStringList args);
  void runSetCommand(const QString& name, const QString& value);
  void runAddCommand(const QString& type, const QString& name, int column, int row, int columnSpan, int rowSpan);
  void save(const QString& fileName);
  void save(QFile& file);
  int findWidgetPluginInstance(WidgetPluginInstance* widgetPluginInstance);
  void run(const QString& line);
  QStringList splitArgs(const QString& argsString);
};

#endif // MAINWINDOW_H
