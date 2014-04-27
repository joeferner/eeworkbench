#ifndef INPUTPLUGIN_H
#define INPUTPLUGIN_H

#include "Plugin.h"

class InputPlugin : public Plugin
{
  Q_OBJECT

public:
  InputPlugin();

  virtual void connect() = 0;
  virtual void disconnect() = 0;

signals:
  void connected();
  void disconnected();
};

#endif // INPUTPLUGIN_H
