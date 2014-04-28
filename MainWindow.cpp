#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "plugins/graph/GraphWidget.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  m_ui(new Ui::MainWindow),
  m_connected(false)
{
  m_ui->setupUi(this);

  m_layout = new QGridLayout();
  centralWidget()->setLayout(m_layout);

  GraphWidget* graph = new GraphWidget(this);
  m_layout->addWidget(graph, 0, 0, 1, 1);
}

MainWindow::~MainWindow()
{
  delete m_ui;
}

void MainWindow::on_actionConnect_triggered()
{
  InputPlugin* activeInputPlugin = m_pluginManager.getActiveInputPlugin();

  if(m_connected) {
    QObject::connect(activeInputPlugin, SIGNAL(disconnected()), this, SLOT(onInputPluginDisconnected()));
    activeInputPlugin->disconnect();
  } else {
    QObject::connect(activeInputPlugin, SIGNAL(connected()), this, SLOT(onInputPluginConnected()));
    activeInputPlugin->connect();
  }
}

void MainWindow::onInputPluginConnected() {
  InputPlugin* activeInputPlugin = m_pluginManager.getActiveInputPlugin();
  QObject::disconnect(activeInputPlugin, SIGNAL(connected()), this, SLOT(onInputPluginConnected()));
  m_connected = true;
  m_ui->actionConnect->setText("Disconnect");
}

void MainWindow::onInputPluginDisconnected() {
  InputPlugin* activeInputPlugin = m_pluginManager.getActiveInputPlugin();
  QObject::disconnect(activeInputPlugin, SIGNAL(disconnected()), this, SLOT(onInputPluginDisconnected()));
  m_connected = false;
  m_ui->actionConnect->setText("Connect");
}

void MainWindow::closeEvent(QCloseEvent *event) {
  m_pluginManager.getActiveInputPlugin()->disconnect();
}

void MainWindow::on_actionExit_triggered() {
  close();
}
