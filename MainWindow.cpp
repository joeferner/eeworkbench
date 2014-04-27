#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "plugins/graph/Graph.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  m_ui(new Ui::MainWindow),
  m_connected(false)
{
  m_ui->setupUi(this);

  m_layout = new QGridLayout();
  centralWidget()->setLayout(m_layout);

  Graph* graph = new Graph(this);
  m_layout->addWidget(graph, 0, 0, 1, 1);
}

MainWindow::~MainWindow()
{
  delete m_ui;
}

void MainWindow::on_actionConnect_triggered()
{
  if(m_connected) {
    m_connected = false;
    m_ui->actionConnect->setText("Connect");
  } else {
    m_connected = true;
    m_ui->actionConnect->setText("Disconnect");
  }
}
