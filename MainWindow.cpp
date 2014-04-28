#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "plugins/graph/GraphWidget.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  m_ui(new Ui::MainWindow),
  m_inputReaderThread(NULL)
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

void MainWindow::on_actionConnect_triggered() {
  m_ui->actionConnect->setEnabled(false);

  InputPlugin* activeInputPlugin = m_pluginManager.getActiveInputPlugin();

  if(activeInputPlugin->isConnected()) {
    QObject::connect(activeInputPlugin, SIGNAL(disconnected()), this, SLOT(onInputPluginDisconnected()));
    activeInputPlugin->disconnect();
  } else {
    stopInputReaderThread();
    QObject::connect(activeInputPlugin, SIGNAL(connected()), this, SLOT(onInputPluginConnected()));
    activeInputPlugin->connect();
  }
}

void MainWindow::onInputPluginConnected() {
  InputPlugin* activeInputPlugin = m_pluginManager.getActiveInputPlugin();
  QObject::disconnect(activeInputPlugin, SIGNAL(connected()), this, SLOT(onInputPluginConnected()));
  m_ui->actionConnect->setText("Disconnect");
  m_ui->actionConnect->setEnabled(true);
  m_inputReaderThread = new InputReaderThread(this, activeInputPlugin);
  m_inputReaderThread->start();
}

void MainWindow::onInputPluginDisconnected() {
  InputPlugin* activeInputPlugin = m_pluginManager.getActiveInputPlugin();
  QObject::disconnect(activeInputPlugin, SIGNAL(disconnected()), this, SLOT(onInputPluginDisconnected()));
  m_ui->actionConnect->setText("Connect");
  m_ui->actionConnect->setEnabled(true);
  stopInputReaderThread();
}

void MainWindow::closeEvent(QCloseEvent*) {
  m_pluginManager.getActiveInputPlugin()->disconnect();
}

void MainWindow::on_actionExit_triggered() {
  close();
}

void MainWindow::stopInputReaderThread() {
  if(m_inputReaderThread == NULL) {
    return;
  }
  m_inputReaderThread->stop();
  m_inputReaderThread = NULL;
}

void MainWindow::onInputReaderThreadMessage() {
  qDebug() << "MainWindow: onInputReaderThreadMessage";
}
