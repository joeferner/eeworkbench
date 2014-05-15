#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "plugins/graph/GraphWidget.h"
#include "plugins/fileInput/FileInputPlugin.h"
#include <QDebug>
#include <QSettings>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

#define SETTINGS_PREFIX       "MainWindow/"
#define INPUT_PLUGIN_SETTING  SETTINGS_PREFIX "InputPlugin"

MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  m_ui(new Ui::MainWindow),
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

  restoreGeometry(settings.value("mainWindowGeometry").toByteArray());
  restoreState(settings.value("mainWindowState").toByteArray());
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
    setConnectedInputPlugin(m_inputSelectComboBox->currentData().value<InputPlugin*>());
    clearWidgets();

    m_connectedInputPlugin->connect();

    QSettings settings;
    settings.setValue(INPUT_PLUGIN_SETTING, m_connectedInputPlugin->getName());
    settings.sync();
  }
}

void MainWindow::load(const QString& fileName) {
  FileInputPlugin* fileInputPlugin = m_pluginManager.getFileInputPlugins();

  closeConnectedInputPlugin();
  setConnectedInputPlugin(fileInputPlugin);
  clearWidgets();

  fileInputPlugin->connect(fileName);
}

void MainWindow::setConnectedInputPlugin(InputPlugin* inputPlugin) {
  if(m_connectedInputPlugin != NULL) {
    QObject::disconnect(m_connectedInputPlugin, SIGNAL(connected()), this, SLOT(onInputPluginConnected()));
    QObject::disconnect(m_connectedInputPlugin, SIGNAL(disconnected()), this, SLOT(onInputPluginDisconnected()));
    QObject::disconnect(m_connectedInputPlugin, SIGNAL(readyRead()), this, SLOT(onInputPluginReadyRead()));
  }

  m_connectedInputPlugin = inputPlugin;

  if(m_connectedInputPlugin) {
    QObject::connect(m_connectedInputPlugin, SIGNAL(connected()), this, SLOT(onInputPluginConnected()));
    QObject::connect(m_connectedInputPlugin, SIGNAL(disconnected()), this, SLOT(onInputPluginDisconnected()));
    QObject::connect(m_connectedInputPlugin, SIGNAL(readyRead()), this, SLOT(onInputPluginReadyRead()));
  }
}

void MainWindow::onInputPluginConnected() {
  m_ui->actionConnect->setText("Disconnect");
  m_ui->actionConnect->setEnabled(true);
  onInputPluginReadyRead();
}

void MainWindow::onInputPluginDisconnected() {
  m_ui->actionConnect->setText("Connect");
  m_ui->actionConnect->setEnabled(true);
  setConnectedInputPlugin(NULL);
}

void MainWindow::closeConnectedInputPlugin() {
  if(m_connectedInputPlugin != NULL) {
    m_connectedInputPlugin->disconnect();
  }
}

void MainWindow::closeEvent(QCloseEvent*) {
  QSettings settings;
  settings.setValue("mainWindowGeometry", saveGeometry());
  settings.setValue("mainWindowState", saveState());

  closeConnectedInputPlugin();
}

void MainWindow::on_actionExit_triggered() {
  close();
}

void MainWindow::clearWidgets() {
  qDebug() << "clearing widgets";
  QList<WidgetPluginInstance*> widgetPluginInstances = m_widgets.values();
  m_widgets.clear();
  foreach(WidgetPluginInstance * widgetPluginInstance, widgetPluginInstances) {
    m_layout->removeWidget(widgetPluginInstance->getWidget());
    delete widgetPluginInstance;
  }
}

void MainWindow::onInputPluginReadyRead() {
  while(m_connectedInputPlugin->available() > 0) {
    uchar ch;
    if(!m_connectedInputPlugin->readByte(&ch)) {
      break;
    }

    if(ch == '\r') {

    } else if(ch == '\n') {
      run(m_inputCurrentLine);
      m_inputCurrentLine = "";
    } else {
      m_inputCurrentLine += ch;
    }
  }
}

void MainWindow::run(const QString& line) {
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
    scopeAndFunctionNameIndex = command.length();
  }
  QString scopeAndFunctionName = command.left(scopeAndFunctionNameIndex);
  QString args = command.mid(scopeAndFunctionNameIndex + 1);

  int scopeIndex = scopeAndFunctionName.indexOf('.');
  if(scopeIndex < 0) {
    scopeIndex = -1;
  }
  QString scope = scopeAndFunctionName.left(qMax(0, scopeIndex));
  if(scope == "") {
    scope = "window";
  }
  QString functionName = scopeAndFunctionName.mid(scopeIndex + 1);

  QStringList argsList = splitArgs(args);

  runCommand(scope, functionName, argsList);
}

QStringList MainWindow::splitArgs(const QString& argsString) {
  QStringList results;

  QString arg = "";
  QChar startCh;
  for(int i = 0; i < argsString.length(); i++) {
    QChar ch = argsString.at(i);
    if(ch == '"' || ch == '\'') {
      startCh = ch;
      i++;
      for(; i < argsString.length(); i++) {
        ch = argsString.at(i);
        if(ch == startCh) {
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
  arg = arg.trimmed();
  if(arg.length() > 0) {
    results.append(arg);
  }

  return results;
}

void MainWindow::runCommand(const QString& scope, const QString& functionName, QStringList args) {
  if(scope == "window") {
    windowCommand(functionName, args);
  } else {
    widgetCommand(scope, functionName, args);
  }
}

void MainWindow::widgetCommand(const QString& scope, const QString& functionName, QStringList args) {
  WidgetPluginInstance* widgetPluginInstance = m_widgets.value(scope);
  if(widgetPluginInstance == NULL) {
    qWarning() << "Could not find scope" << scope;
    return;
  }
  widgetPluginInstance->runCommand(functionName, args, m_connectedInputPlugin);
}

void MainWindow::windowCommand(const QString& functionName, QStringList args) {
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

void MainWindow::runAddCommand(const QString& type, const QString& name, int column, int row, int columnSpan, int rowSpan) {
  qDebug() << "adding widget" << name;

  const WidgetPlugin* widgetPlugin = m_pluginManager.getWidgetPlugin(type);
  if(widgetPlugin == NULL) {
    qWarning() << "Invalid type" << type << "for add.";
    return;
  }
  WidgetPluginInstance* widgetPluginInstance = widgetPlugin->createInstance(name);
  m_widgets.insert(name, widgetPluginInstance);

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
    QMessageBox::warning(this, "Error", "Could not open file for write");
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

