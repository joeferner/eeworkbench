#include "SerialPortPlugin.h"
#include "SerialPortConnectDialog.h"
#include <QSettings>
#include <QDebug>
#include <QThread>

#define SETTINGS_PREFIX    "SerialPortPlugin/"
#define PORT_NAME_SETTING  SETTINGS_PREFIX "PortName"
#define BAUD_RATE_SETTING  SETTINGS_PREFIX "BaudRate"

#define CMD_CONNECT "!CONNECT\n"

class SerialPortConnectThread : public QThread {
public:
  SerialPortConnectThread(SerialPortPlugin* serialPortPlugin) {
    m_serialPortPlugin = serialPortPlugin;
  }

  void setPortName(const QString& portName) { m_portName = portName; }
  void setBaudRate(int baudRate) { m_baudRate = baudRate; }

  void run() {
    if(m_serialPortPlugin->openSerialPort(m_portName, m_baudRate)) {
      int retryCount = 3;
      bool connectSuccess = false;
      for(int i=0; i<retryCount; i++) {
        connectSuccess = m_serialPortPlugin->sendConnectCommand();
        if(connectSuccess) {
          break;
        }
      }
      if(!connectSuccess) {
        m_serialPortPlugin->disconnect();
      }
    }
  }

private:
  SerialPortPlugin* m_serialPortPlugin;
  QString m_portName;
  int m_baudRate;
};

SerialPortPlugin::SerialPortPlugin() :
  m_serialPort(NULL)
{
  m_connectThread = new SerialPortConnectThread(this);
}

void SerialPortPlugin::connect() {
  disconnect();

  QSettings settings;
  SerialPortConnectDialog dlg;
  dlg.setPortName(settings.value(PORT_NAME_SETTING, "").toString());
  dlg.setBaudRate(settings.value(BAUD_RATE_SETTING, 115200).toInt());
  if(dlg.exec() == QDialog::Accepted){
    QString portName = dlg.getPortName();
    int baudRate = dlg.getBaudRate();

    settings.setValue(PORT_NAME_SETTING, portName);
    settings.setValue(BAUD_RATE_SETTING, baudRate);
    settings.sync();

    m_connectThread->setPortName(portName);
    m_connectThread->setBaudRate(baudRate);
    m_connectThread->start();
  }
}

bool SerialPortPlugin::sendConnectCommand() {
  if(m_serialPort->write("\n", 1) != 1) {
    qDebug() << "Could not send new line";
    return false;
  }
  if(!m_serialPort->waitForBytesWritten(1000)) {
    qDebug() << "Timeout waiting for bytes to be written";
    return false;
  }

  clearRead();
  if(m_serialPort->write(CMD_CONNECT, strlen(CMD_CONNECT)) != strlen(CMD_CONNECT)) {
    qDebug() << "Could not send connect command";
    return false;
  }
  if(!m_serialPort->waitForBytesWritten(1000)) {
    qDebug() << "Timeout waiting for bytes to be written";
    return false;
  }
  QString line = readLine(5000);
  if(line.startsWith("+OK")) {
    emit connected();
    return true;
  }
  qDebug() << "Invalid line" << line;
  return false;
}

void SerialPortPlugin::disconnect() {
  if(m_serialPort) {
    closeSerialPort();
    emit disconnected();
  }
}

bool SerialPortPlugin::isConnected() {
  return m_serialPort != NULL;
}

bool SerialPortPlugin::openSerialPort(const QString& portName, int baudRate) {
  QSerialPort* serialPort = new QSerialPort("/dev/ttyUSB0");//portName);
  serialPort->setReadBufferSize(10 * 1024);
  if(!serialPort->open(QIODevice::ReadWrite)) {
    qDebug() << "Could not open serial port";
    goto openFail;
  }

  qDebug() << "Serial port" << portName << " opened";

  if(!serialPort->setBaudRate(baudRate)) {
    qDebug() << "Could not set baud rate";
    goto openFail;
  }

  if(!serialPort->setDataBits(QSerialPort::Data8)) {
    qDebug() << "Could not set data bits";
    goto openFail;
  }

  if(!serialPort->setParity(QSerialPort::NoParity)) {
    qDebug() << "Could not set parity";
    goto openFail;
  }

  if(!serialPort->setStopBits(QSerialPort::OneStop)) {
    qDebug() << "Could not set stop bits";
    goto openFail;
  }

  qDebug() << "Baud rate:" << serialPort->baudRate();
  qDebug() << "Data bits:" << serialPort->dataBits();
  qDebug() << "Parity:" << serialPort->parity();
  qDebug() << "Stop bits:" << serialPort->stopBits();
  qDebug() << "Flow control:" << serialPort->flowControl();
  qDebug() << "Read buffer size:" << serialPort->readBufferSize();

  m_serialPort = serialPort;
  return true;

openFail:
  delete serialPort;
  return false;
}

void SerialPortPlugin::closeSerialPort() {
  if(m_serialPort == NULL) {
    return;
  }
  qDebug() << "closing serial port";
  m_serialPort->close();
  for(int i = 0; i < 10 && m_serialPort->isOpen(); i++) {
    qDebug() << "waiting for serial port to close";
    QThread::msleep(100);
  }
  delete m_serialPort;
  m_serialPort = NULL;
  qDebug() << "serial port closed";
}

bool SerialPortPlugin::readByte(uchar* ch) {
  if(!isConnected()) {
    return false;
  }
  if(available() < 1 && !m_serialPort->waitForReadyRead(10)) {
    qDebug() << "Timeout waiting for readByte";
    return false;
  }
  return m_serialPort->getChar((char*)ch);
}

qint64 SerialPortPlugin::read(unsigned char* buffer, qint64 bytesToRead) {
  if(!isConnected()) {
    return 0;
  }
  if(available() < bytesToRead && !m_serialPort->waitForReadyRead(10)) {
    qDebug() << "Timeout waiting for read";
    return false;
  }
  return m_serialPort->read((char*)buffer, bytesToRead);
}

qint64 SerialPortPlugin::available() {
  if(!isConnected()) {
    return 0;
  }
  qint64 a = m_serialPort->bytesAvailable();
  if(a == 0) {
    m_serialPort->waitForReadyRead(10);
    a = m_serialPort->bytesAvailable();
  }
  return a;
}
