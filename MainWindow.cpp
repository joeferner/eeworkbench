#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "plugins/graph/GraphWidget.h"
#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QErrorMessage>

#define SETTINGS_PREFIX       "MainWindow/"
#define INPUT_PLUGIN_SETTING  SETTINGS_PREFIX "InputPlugin"

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  m_ui(new Ui::MainWindow),
  m_inputReaderThread(NULL),
  m_commandRunner(this),
  m_connectedInputPlugin(NULL) {
  m_ui->setupUi(this);

  m_layout = new QGridLayout();
  centralWidget()->setLayout(m_layout);

  m_descriptionLabel = new QLabel(this);
  statusBar()->addWidget(m_descriptionLabel);

  m_inputSelectComboBox = new QComboBox(m_ui->mainToolBar);
  foreach(InputPlugin * inputPlugin, m_pluginManager.getInputPlugins()) {
    m_inputSelectComboBox->addItem(inputPlugin->getName(), QVariant::fromValue(inputPlugin));
  }
  m_ui->mainToolBar->insertWidget(m_ui->actionConnect, m_inputSelectComboBox);

  QSettings settings;
  QString selectedInputPlugin = settings.value(INPUT_PLUGIN_SETTING, "").toString();
  if(selectedInputPlugin.length() > 0) {
    m_inputSelectComboBox->setCurrentText(selectedInputPlugin);
  }

  connect(this, SIGNAL(addWidgetPluginInstance(WidgetPluginInstance*, int, int, int, int)), this, SLOT(onAddWidgetPluginInstance(WidgetPluginInstance*, int, int, int, int)));
}

MainWindow::~MainWindow() {
  delete m_ui;
}

void MainWindow::on_actionConnect_triggered() {
  m_ui->actionConnect->setEnabled(false);

  if(m_connectedInputPlugin != NULL && m_connectedInputPlugin->isConnected()) {
    closeConnectedInputPlugin();
  } else {
    m_ui->actionConnect->setText("Connecting");

    closeConnectedInputPlugin();

    m_connectedInputPlugin = m_inputSelectComboBox->currentData().value<InputPlugin*>();
    QObject::connect(m_connectedInputPlugin, SIGNAL(connected()), this, SLOT(onInputPluginConnected()));
    QObject::connect(m_connectedInputPlugin, SIGNAL(disconnected()), this, SLOT(onInputPluginDisconnected()));

    if(m_connectedInputPlugin == NULL) {
      qDebug() << "Could not get selected plugin";
      return;
    }
    stopInputReaderThread();
    clearWidgets();
    m_connectedInputPlugin->connect();

    QSettings settings;
    settings.setValue(INPUT_PLUGIN_SETTING, m_connectedInputPlugin->getName());
    settings.sync();
  }
}

void MainWindow::onInputPluginConnected() {
  m_ui->actionConnect->setText("Disconnect");
  m_ui->actionConnect->setEnabled(true);
  m_inputReaderThread = new InputReaderThread(&m_commandRunner, m_connectedInputPlugin);
  m_inputReaderThread->start();
}

void MainWindow::onInputPluginDisconnected() {
  m_ui->actionConnect->setText("Connect");
  m_ui->actionConnect->setEnabled(true);
  stopInputReaderThread();

  if(m_connectedInputPlugin != NULL) {
    QObject::disconnect(m_connectedInputPlugin, SIGNAL(connected()), this, SLOT(onInputPluginConnected()));
    QObject::disconnect(m_connectedInputPlugin, SIGNAL(disconnected()), this, SLOT(onInputPluginDisconnected()));
    m_connectedInputPlugin = NULL;
  }
}

void MainWindow::closeConnectedInputPlugin() {
  if(m_connectedInputPlugin != NULL) {
    m_connectedInputPlugin->disconnect();
  }
}

void MainWindow::closeEvent(QCloseEvent*) {
  closeConnectedInputPlugin();
}

void MainWindow::on_actionExit_triggered() {
  close();
}

void MainWindow::clearWidgets() {
  QList<WidgetPluginInstance*> widgetPluginInstances = m_widgets.values();
  m_widgets.clear();
  foreach(WidgetPluginInstance * widgetPluginInstance, widgetPluginInstances) {
    delete widgetPluginInstance;
  }
}

void MainWindow::stopInputReaderThread() {
  if(m_inputReaderThread == NULL) {
    return;
  }
  m_inputReaderThread->stop();
  delete m_inputReaderThread;
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
  if(functionName == "clear") {
    clearWidgets();
  } else if(functionName == "set") {
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
    qWarning() << "invalid command" << functionName << "args" << args;
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
  WidgetPluginInstance* widgetPluginInstance = widgetPlugin->createInstance(name);
  m_widgets.insert(name, widgetPluginInstance);
  emit addWidgetPluginInstance(widgetPluginInstance, row, column, rowSpan, columnSpan);
}

void MainWindow::onAddWidgetPluginInstance(WidgetPluginInstance* widgetPluginInstance, int row, int column, int rowSpan, int columnSpan) {
  QWidget* widget = widgetPluginInstance->getWidget();
  m_layout->addWidget(widget, row, column, rowSpan, columnSpan);
}


void MainWindow::on_actionSave_triggered() {
  QString fileName = QFileDialog::getSaveFileName(this, "Save...", QString(), "EEWorkbench (*.eew);;All Files (*.*)");
  if(fileName.length() > 0) {
    save(fileName);
  }
}

void MainWindow::save(const QString& fileName) {
  QFile file(fileName);
  if(!file.open(QIODevice::WriteOnly)) {
    QErrorMessage error(this);
    error.showMessage("Could not open file for write");
    return;
  }

  save(file);

  file.close();
}

void MainWindow::save(QFile& file) {
  QTextStream out(&file);
  foreach(WidgetPluginInstance * widgetPluginInstance, m_widgets.values()) {
    int row = 0;
    int column = 0;
    int rowSpan = 1;
    int columnSpan = 1;
    int widgetIndex = findWidgetPluginInstance(widgetPluginInstance);
    if(widgetIndex < 0) {
      qDebug() << "Could not find widget";
    } else {
      m_layout->getItemPosition(widgetIndex, &row, &column, &rowSpan, &columnSpan);
    }

    QString type = widgetPluginInstance->getWidgetPlugin()->getName();
    QString name = widgetPluginInstance->getName();
    out << QString("!add %1,%2,%3,%4,%5,%6\n").arg(type).arg(name).arg(row).arg(column).arg(rowSpan).arg(columnSpan);
    widgetPluginInstance->save(out);
  }
}

int MainWindow::findWidgetPluginInstance(WidgetPluginInstance* widgetPluginInstance) {
  for(int i = 0; i < m_layout->count(); i++) {
    QLayoutItem* layoutItem = m_layout->itemAt(i);
    if(layoutItem->widget() == widgetPluginInstance->getWidget()) {
      return i;
    }
  }
  return -1;
}

