#ifndef INPUTPLUGIN_H
#define INPUTPLUGIN_H

#include "Plugin.h"

class InputPlugin : public Plugin {
  Q_OBJECT

public:
  InputPlugin();

  virtual void connect() = 0;
  virtual void disconnect() = 0;
  virtual bool isConnected() = 0;
  virtual bool readByte(uchar* ch) = 0;
  virtual qint64 read(unsigned char* buffer, qint64 bytesToRead) = 0;
  virtual qint64 available() = 0;
  virtual QString readLine(int timeout);
  virtual void clearRead();

signals:
  void connected();
  void disconnected();
};

#endif // INPUTPLUGIN_H
