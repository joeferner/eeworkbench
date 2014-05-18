#include "SerialPortPlugin.h"
#include "SerialPortConnectDialog.h"
#include <QSettings>
#include <QDebug>
#include <QThread>

#define SETTINGS_PREFIX    "SerialPortPlugin/"
#define PORT_NAME_SETTING  SETTINGS_PREFIX "PortName"
#define BAUD_RATE_SETTING  SETTINGS_PREFIX "BaudRate"

#define CMD_CONNECT "!CONNECT\n"

SerialPortPlugin::SerialPortPlugin() :
  m_connecting(false),
  m_serialPort(NULL) {
}

void SerialPortPlugin::connect() {
  disconnect();

  QSettings settings;
  SerialPortConnectDialog dlg;
  dlg.setPortName(settings.value(PORT_NAME_SETTING, "").toString());
  dlg.setBaudRate(settings.value(BAUD_RATE_SETTING, 115200).toInt());
  if(dlg.exec() == QDialog::Accepted) {
    QString portName = dlg.getPortName();
    int baudRate = dlg.getBaudRate();

    settings.setValue(PORT_NAME_SETTING, portName);
    settings.setValue(BAUD_RATE_SETTING, baudRate);
    settings.sync();

    openSerialPort(portName, baudRate);
  } else {
    emit disconnected();
  }
}

void SerialPortPlugin::sendConnectCommand() {
  if(m_serialPort->write("\n", 1) != 1) {
    qDebug() << "Could not send new line";
    return;
  }
  if(m_serialPort->write("\n", 1) != 1) {
    qDebug() << "Could not send new line";
    return;
  }

  clearRead();

  if(m_serialPort->write(CMD_CONNECT, strlen(CMD_CONNECT)) != strlen(CMD_CONNECT)) {
    qDebug() << "Could not send connect command";
    return;
  }
  onReadyRead();
}

void SerialPortPlugin::disconnect() {
  if(m_serialPort) {
    closeSerialPort();
    emit disconnected();
  }
}

bool SerialPortPlugin::isConnected() {
  return !m_connecting && m_serialPort != NULL;
}

bool SerialPortPlugin::openSerialPort(const QString& portName, int baudRate) {
  m_connecting = true;
  m_serialPort = new QSerialPort(portName);
  QObject::connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
  m_serialPort->setReadBufferSize(10 * 1024);
  if(!m_serialPort->open(QIODevice::ReadWrite)) {
    qDebug() << "Could not open serial port";
    goto openFail;
  }

  qDebug() << "Serial port" << portName << " opened";

  if(!m_serialPort->setBaudRate(baudRate)) {
    qDebug() << "Could not set baud rate";
    goto openFail;
  }

  if(!m_serialPort->setDataBits(QSerialPort::Data8)) {
    qDebug() << "Could not set data bits";
    goto openFail;
  }

  if(!m_serialPort->setParity(QSerialPort::NoParity)) {
    qDebug() << "Could not set parity";
    goto openFail;
  }

  if(!m_serialPort->setStopBits(QSerialPort::OneStop)) {
    qDebug() << "Could not set stop bits";
    goto openFail;
  }

  qDebug() << "Baud rate:" << m_serialPort->baudRate();
  qDebug() << "Data bits:" << m_serialPort->dataBits();
  qDebug() << "Parity:" << m_serialPort->parity();
  qDebug() << "Stop bits:" << m_serialPort->stopBits();
  qDebug() << "Flow control:" << m_serialPort->flowControl();
  qDebug() << "Read buffer size:" << m_serialPort->readBufferSize();

  sendConnectCommand();
  return true;

openFail:
  QObject::disconnect(m_serialPort, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
  delete m_serialPort;
  m_serialPort = NULL;
  return false;
}

void SerialPortPlugin::onReadyRead() {
  if(m_connecting) {
    qDebug() << "onReadyRead";
    if(m_serialPort->canReadLine()) {
      char line[100];
      qint64 lineLength = m_serialPort->readLine(line, 100);
      qDebug() << "onReadyRead" << line;
      if((lineLength > 0) && (strncmp(line, "+OK", 3) == 0)) {
        m_connecting = false;
        emit connected();
        emit readyRead();
      }
    }
  } else {
    emit readyRead();
  }
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
  QObject::disconnect(m_serialPort, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
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
