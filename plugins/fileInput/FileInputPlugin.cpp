#include "FileInputPlugin.h"
#include "FileInputConnectDialog.h"
#include <QSettings>
#include <QDebug>

#define SETTINGS_PREFIX    "FileInputPlugin/"
#define FILE_NAME_SETTING  SETTINGS_PREFIX "FileName"

FileInputPlugin::FileInputPlugin() :
  m_file(NULL)
{
}

void FileInputPlugin::connect() {
  disconnect();

  QSettings settings;
  FileInputConnectDialog dlg;
  dlg.setFileName(settings.value(FILE_NAME_SETTING, "").toString());
  if(dlg.exec() == QDialog::Accepted){
    QString fileName = dlg.getFileName();

    settings.setValue(FILE_NAME_SETTING, fileName);
    settings.sync();

    openFile(fileName);
    if(m_file) {
      emit connected();
    }
  }
}

bool FileInputPlugin::isConnected() {
  return m_file != NULL;
}

void FileInputPlugin::openFile(const QString &fileName) {
  QFile* file = new QFile(fileName);
  if(!file->exists()) {
    delete file;
    return;
  }
  if(!file->open(QIODevice::ReadOnly)) {
    delete file;
    return;
  }
  qDebug() << "FileInputPlugin: Opened file:" << file->fileName();
  m_file = file;
}

bool FileInputPlugin::readByte(uchar* ch) {
  if(!isConnected()) {
    return false;
  }
  return m_file->getChar((char*)ch);
}

qint64 FileInputPlugin::read(unsigned char* buffer, qint64 bytesToRead) {
  return m_file->read((char*)buffer, bytesToRead);
}

qint64 FileInputPlugin::available() {
  if(!isConnected()) {
    return 0;
  }
  return m_file->bytesAvailable();
}

void FileInputPlugin::closeFile() {
  if(m_file) {
    qDebug() << "FileInputPlugin: Closing file:" << m_file->fileName();
    m_file->close();
    m_file = NULL;
  }
}

void FileInputPlugin::disconnect() {
  if(m_file) {
    closeFile();
    emit disconnected();
  }
}
