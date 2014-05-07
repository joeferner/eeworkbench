#include "SerialPortPlugin.h"
#include "SerialPortConnectDialog.h"
#include <QSettings>
#include <QDebug>

#define SETTINGS_PREFIX    "SerialPortPlugin/"
#define PORT_NAME_SETTING  SETTINGS_PREFIX "PortName"
#define BAUD_RATE_SETTING  SETTINGS_PREFIX "BaudRate"

#define CMD_CONNECT "!CONNECT"

SerialPortPlugin::SerialPortPlugin() :
  m_serialPort(NULL)
{
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

    openSerialPort(portName, baudRate);
    if(m_serialPort) {
      m_serialPort->write("\n", 1);
      clearRead();
      m_serialPort->write(CMD_CONNECT, strlen(CMD_CONNECT));
      QString line = readLine(5000);
      if(!line.startsWith("+OK")) {
        qDebug() << "Invalid line" << line;
        clearRead();
        m_serialPort->write(CMD_CONNECT, strlen(CMD_CONNECT));
        line = readLine(5000);
        if(!line.startsWith("+OK")) {
          qDebug() << "Invalid line" << line;
          m_serialPort->close();
          m_serialPort = NULL;
        }
      }
      emit connected();
    }
  }
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

void SerialPortPlugin::openSerialPort(const QString& portName, int baudRate) {
  QSerialPort* serialPort = new QSerialPort();
  serialPort->setPortName(portName);
  if(serialPort->open(QIODevice::ReadWrite)) {
    serialPort->setBaudRate(baudRate);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    m_serialPort = serialPort;
  } else {
    delete m_serialPort;
  }
}

void SerialPortPlugin::closeSerialPort() {
  if(m_serialPort == NULL) {
    return;
  }
  m_serialPort->close();
  m_serialPort = NULL;
}

bool SerialPortPlugin::readByte(uchar* ch) {
  if(!isConnected()) {
    return false;
  }
  return m_serialPort->getChar((char*)ch);
}

qint64 SerialPortPlugin::read(unsigned char* buffer, qint64 bytesToRead) {
  if(!isConnected()) {
    return 0;
  }
  return m_serialPort->read((char*)buffer, bytesToRead);
}

qint64 SerialPortPlugin::available() {
  if(!isConnected()) {
    return 0;
  }
  return m_serialPort->bytesAvailable();
}
