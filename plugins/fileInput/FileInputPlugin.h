#ifndef FILEINPUTPLUGIN_H
#define FILEINPUTPLUGIN_H

#include "../InputPlugin.h"

class FileInputPlugin : public InputPlugin
{
  Q_OBJECT

public:
  FileInputPlugin();

  virtual void connect();
  virtual void disconnect();
};

#endif // FILEINPUTPLUGIN_H
