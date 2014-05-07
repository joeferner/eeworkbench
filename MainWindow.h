#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QStringList>
#include <QLabel>
#include <QMap>
#include <QComboBox>
#include "plugins/PluginManager.h"
#include "plugins/WidgetPluginInstance.h"
#include "InputReaderThread.h"
#include "CommandRunner.h"

namespace Ui {
class MainWindow;
}

class MainWindow :
    public QMainWindow,
    public CommandRunnerWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  virtual void runCommand(InputReaderThread* inputReaderThread, const QString& scope, const QString& functionName, QStringList args);

protected:
  virtual void closeEvent(QCloseEvent *event);

signals:
  void addWidgetPluginInstance(WidgetPluginInstance* widgetPluginInstance, int row, int column, int rowSpan, int columnSpan);

private slots:
  void on_actionConnect_triggered();
  void onInputPluginConnected();
  void onInputPluginDisconnected();
  void onAddWidgetPluginInstance(WidgetPluginInstance* widgetPluginInstance, int row, int column, int rowSpan, int columnSpan);

  void on_actionExit_triggered();

private:
  Ui::MainWindow* m_ui;
  QGridLayout* m_layout;
  PluginManager m_pluginManager;
  InputReaderThread* m_inputReaderThread;
  CommandRunner m_commandRunner;
  QLabel* m_descriptionLabel;
  QComboBox* m_inputSelectComboBox;
  InputPlugin* m_connectedInputPlugin;
  QMap<QString, WidgetPluginInstance*> m_widgets;

  void stopInputReaderThread();
  void clearWidgets();
  void runCommand(const QString& functionName, QStringList args);
  void runSetCommand(const QString& name, const QString& value);
  void runAddCommand(const QString& type, const QString& name, int row, int column, int rowSpan, int columnSpan);
};

#endif // MAINWINDOW_H
