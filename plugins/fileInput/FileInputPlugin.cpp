#include "FileInputPlugin.h"
#include "FileInputConnectDialog.h"
#include <QSettings>
#include <QDebug>
#include <QMessageBox>

#define SETTINGS_PREFIX    "FileInputPlugin/"
#define FILE_NAME_SETTING  SETTINGS_PREFIX "FileName"

FileInputPlugin::FileInputPlugin() :
  m_file(NULL) {
}

void FileInputPlugin::connect() {
  disconnect();

  QSettings settings;
  FileInputConnectDialog dlg;
  dlg.setFileName(settings.value(FILE_NAME_SETTING, "").toString());
  if(dlg.exec() == QDialog::Accepted) {
    QString fileName = dlg.getFileName();
    connect(fileName);
  }
}

void FileInputPlugin::connect(const QString& fileName) {
  QSettings settings;
  settings.setValue(FILE_NAME_SETTING, fileName);
  settings.sync();

  m_file = openFile(fileName);
  if(m_file) {
    emit connected();
  }
}

bool FileInputPlugin::isConnected() {
  return m_file != NULL;
}

void FileInputPlugin::onReadyRead() {
  emit readyRead();
}

QFile* FileInputPlugin::openFile(const QString& fileName) {
  QFile* file = new QFile(fileName);
  if(!file->exists()) {
    QMessageBox::warning(NULL, "Error Opening File", QString("File %1 does not exist").arg(fileName));
    delete file;
    return NULL;
  }
  QObject::connect(file, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
  if(!file->open(QIODevice::ReadOnly)) {
    QMessageBox::warning(NULL, "Error Opening File", QString("Could not open %1 for read").arg(fileName));
    delete file;
    return NULL;
  }
  qDebug() << "FileInputPlugin: Opened file:" << file->fileName();
  return file;
}

bool FileInputPlugin::readByte(uchar* ch) {
  if(!isConnected()) {
    return false;
  }
  return m_file->getChar((char*)ch);
}

qint64 FileInputPlugin::read(unsigned char* buffer, qint64 bytesToRead) {
  if(!isConnected()) {
    return 0;
  }
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
