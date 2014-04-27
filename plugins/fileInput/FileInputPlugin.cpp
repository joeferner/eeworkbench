#include "FileInputPlugin.h"

FileInputPlugin::FileInputPlugin()
{
}

void FileInputPlugin::connect() {
  emit connected();
}

void FileInputPlugin::disconnect() {
  emit disconnected();
}
