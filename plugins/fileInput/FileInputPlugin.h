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
  virtual bool readByte(uchar* ch);
  virtual qint64 read(unsigned char* buffer, qint64 bytesToRead);
  virtual qint64 available();
  virtual QString getName() const { return "fileInput"; }

private:
  void closeFile();
  void openFile(const QString &fileName);

  QFile *m_file;
};

#endif // FILEINPUTPLUGIN_H
