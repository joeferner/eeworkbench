#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include "plugins/PluginManager.h"
#include "InputReaderThread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public InputReaderThreadTarget
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

  virtual void onInputReaderThreadMessage(const QString& line);

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

  void stopInputReaderThread();
};

#endif // MAINWINDOW_H
