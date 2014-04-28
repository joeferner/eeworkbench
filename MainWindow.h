#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QStringList>
#include <QLabel>
#include "plugins/PluginManager.h"
#include "InputReaderThread.h"
#include "CommandRunner.h"

namespace Ui {
class MainWindow;
}

class MainWindow :
    public QMainWindow,
    public InputReaderThreadTarget,
    public CommandRunnerWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  virtual void onInputReaderThreadMessage(const QString& line);
  virtual void runCommand(const QString& functionName, QStringList args);

protected:
  virtual void closeEvent(QCloseEvent *event);

private slots:
  void on_actionConnect_triggered();
  void onInputPluginConnected();
  void onInputPluginDisconnected();

  void on_actionExit_triggered();

private:
  Ui::MainWindow* m_ui;
  QGridLayout* m_layout;
  PluginManager m_pluginManager;
  InputReaderThread* m_inputReaderThread;
  CommandRunner m_commandRunner;
  QLabel* m_descriptionLabel;

  void stopInputReaderThread();
  void runSetCommand(const QString& name, const QString& value);
};

#endif // MAINWINDOW_H
