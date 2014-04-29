#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "plugins/graph/GraphWidget.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  m_ui(new Ui::MainWindow),
  m_inputReaderThread(NULL),
  m_commandRunner(this)
{
  m_ui->setupUi(this);

  m_layout = new QGridLayout();
  centralWidget()->setLayout(m_layout);

  m_descriptionLabel = new QLabel(this);
  statusBar()->addWidget(m_descriptionLabel);

  connect(this, SIGNAL(addWidgetPluginInstance(WidgetPluginInstance*,int,int,int,int)), this, SLOT(onAddWidgetPluginInstance(WidgetPluginInstance*,int,int,int,int)));
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
    clearWidgets();
    QObject::connect(activeInputPlugin, SIGNAL(connected()), this, SLOT(onInputPluginConnected()));
    activeInputPlugin->connect();
  }
}

void MainWindow::onInputPluginConnected() {
  InputPlugin* activeInputPlugin = m_pluginManager.getActiveInputPlugin();
  QObject::disconnect(activeInputPlugin, SIGNAL(connected()), this, SLOT(onInputPluginConnected()));
  m_ui->actionConnect->setText("Disconnect");
  m_ui->actionConnect->setEnabled(true);
  m_inputReaderThread = new InputReaderThread(&m_commandRunner, activeInputPlugin);
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

void MainWindow::clearWidgets() {
  foreach(WidgetPluginInstance* widgetPluginInstance, m_widgets.values()) {
    delete widgetPluginInstance;
  }
  m_widgets.clear();
}

void MainWindow::stopInputReaderThread() {
  if(m_inputReaderThread == NULL) {
    return;
  }
  m_inputReaderThread->stop();
  m_inputReaderThread = NULL;
}

void MainWindow::runCommand(InputReaderThread* inputReaderThread, const QString& scope, const QString& functionName, QStringList args) {
  if(scope == "window") {
    runCommand(functionName, args);
  } else {
    WidgetPluginInstance* widgetPluginInstance = m_widgets.value(scope);
    if(widgetPluginInstance == NULL) {
      qWarning() << "Could not find scope" << scope;
      return;
    }
    widgetPluginInstance->runCommand(inputReaderThread, functionName, args);
  }
}

void MainWindow::runCommand(const QString& functionName, QStringList args) {
  if(functionName == "set") {
    if(args.length() == 2) {
      runSetCommand(args.at(0), args.at(1));
    } else {
      qWarning() << "invalid number of arguments for set. Expected 2, found" << args.length();
    }
  } else if(functionName == "add") {
    if(args.length() == 6) {
      runAddCommand(args.at(0), args.at(1), args.at(2).toInt(), args.at(3).toInt(), args.at(4).toInt(), args.at(5).toInt());
    } else {
      qWarning() << "invalid number of arguments for set. Expected 6, found" << args.length();
    }
  } else {
    qWarning() << "invalid command" << functionName << args;
  }
}

void MainWindow::runSetCommand(const QString& name, const QString& value) {
  if(name == "name") {
    setWindowTitle("EEWorkbench: " + value);
  } else if(name == "description") {
    m_descriptionLabel->setText(value);
  } else {
    qWarning() << "Unknown set variable" << name;
  }
}

void MainWindow::runAddCommand(const QString& type, const QString& name, int row, int column, int rowSpan, int columnSpan) {
  const WidgetPlugin* widgetPlugin = m_pluginManager.getWidgetPlugin(type);
  if(widgetPlugin == NULL) {
    qWarning() << "Invalid type" << type << "for add.";
    return;
  }
  WidgetPluginInstance* widgetPluginInstance = widgetPlugin->createInstance();
  m_widgets.insert(name, widgetPluginInstance);
  emit addWidgetPluginInstance(widgetPluginInstance, row, column, rowSpan, columnSpan);
}

void MainWindow::onAddWidgetPluginInstance(WidgetPluginInstance* widgetPluginInstance, int row, int column, int rowSpan, int columnSpan) {
  QWidget* widget = widgetPluginInstance->getWidget();
  m_layout->addWidget(widget, row, column, rowSpan, columnSpan);
}

