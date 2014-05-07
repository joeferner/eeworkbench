#ifndef SERIALPORTPLUGIN_H
#define SERIALPORTPLUGIN_H

#include "../InputPlugin.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

class SerialPortPlugin : public InputPlugin
{
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

protected:
  void openSerialPort(const QString& portName, int baudRate);
  void closeSerialPort();

private:
  QSerialPort* m_serialPort;
};

#endif // SERIALPORTPLUGIN_H
