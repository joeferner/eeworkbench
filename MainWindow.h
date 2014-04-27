#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <plugins/PluginManager.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void on_actionConnect_triggered();
  void onInputPluginConnected();
  void onInputPluginDisconnected();

private:
  Ui::MainWindow *m_ui;
  QGridLayout *m_layout;
  bool m_connected;
  PluginManager m_pluginManager;
};

#endif // MAINWINDOW_H
