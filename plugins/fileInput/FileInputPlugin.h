#ifndef FILEINPUTPLUGIN_H
#define FILEINPUTPLUGIN_H

#include "../InputPlugin.h"
#include <QFile>

class FileInputPlugin : public InputPlugin
{
  Q_OBJECT

public:
  FileInputPlugin();

  virtual void connect();
  virtual void disconnect();
  virtual bool isConnected();

private:
  void closeFile();
  void openFile(const QString &fileName);

  QFile *m_file;
};

#endif // FILEINPUTPLUGIN_H
