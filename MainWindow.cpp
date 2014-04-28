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

void MainWindow::onInputReaderThreadMessage(const QString& line) {
  if(line.length() == 0) {
    return;
  }

  QChar type = line.at(0);
  if(type == '?') {
    qDebug() << line.mid(1);
  } else if(type == '!') {
    runCommand(line.mid(1).trimmed());
  } else {
    qDebug() << "MainWindow: Unknown line type:" << line;
  }
}

void MainWindow::runCommand(const QString& command) {
  int scopeAndFunctionNameIndex = command.indexOf(' ');
  if(scopeAndFunctionNameIndex < 0) {
    scopeAndFunctionNameIndex = command.length() - 1;
  }
  QString scopeAndFunctionName = command.left(scopeAndFunctionNameIndex);
  QString args = command.mid(scopeAndFunctionNameIndex + 1);

  int scopeIndex = scopeAndFunctionName.indexOf('.');
  if(scopeIndex < 0) {
    scopeIndex = 0;
  }
  QString scope = scopeAndFunctionName.left(scopeIndex);
  if(scope == "") {
    scope = "window";
  }
  QString functionName = scopeAndFunctionName.mid(scopeIndex + 1);

  QStringList argsList = splitArgs(args);

  qDebug() << "scope:" << scope << " functionName: " << functionName << " args:" << argsList;
}

QStringList MainWindow::splitArgs(const QString& argsString) {
  QStringList results;

  QString arg = "";
  QChar startCh;
  for(int i=0; i<argsString.length(); i++) {
    QChar ch = argsString.at(i);
    if(ch == '"' || ch == '\'') {
      startCh = ch;
      i++;
      for(; i<argsString.length(); i++) {
        ch = argsString.at(i);
        if(ch == startCh) {
          i++;
          break;
        }
        arg.append(ch);
      }
    } else if(ch == ',') {
      results.append(arg.trimmed());
      arg.clear();
    } else {
      arg.append(ch);
    }
  }
  results.append(arg.trimmed());

  return results;
}
