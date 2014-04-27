#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>

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

private:
  Ui::MainWindow *m_ui;
  QGridLayout *m_layout;
  bool m_connected;
};

#endif // MAINWINDOW_H
