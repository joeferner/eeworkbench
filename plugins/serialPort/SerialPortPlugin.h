#ifndef SERIALPORTPLUGIN_H
#define SERIALPORTPLUGIN_H

#include "../InputPlugin.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QReadWriteLock>

class SerialPortPlugin : public InputPlugin {
  Q_OBJECT

public:
  SerialPortPlugin();

  virtual void connect();
  virtual void disconnect();
  virtual bool isConnected();
  virtual bool readByte(uchar* ch);
  virtual qint64 read(unsigned char* buffer, qint64 bytesToRead);
  virtual qint64 available();
  virtual QString getName() const { return "Serial Port"; }

private slots:
  void onReadyRead();

protected:
  bool openSerialPort(const QString& portName, int baudRate);
  void closeSerialPort();
  void sendConnectCommand();

  friend class SerialPortConnectThread;

private:
  bool m_connecting;
  QSerialPort* m_serialPort;
};

#endif // SERIALPORTPLUGIN_H
