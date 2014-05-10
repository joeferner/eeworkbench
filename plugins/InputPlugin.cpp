#include "InputPlugin.h"
#include <QThread>
#include <QDebug>

InputPlugin::InputPlugin()
{
}

QString InputPlugin::readLine(int timeout) {
  QString line = "";
  int time = 0;
  while(time < timeout) {
    if(available() == 0) {
      QThread::msleep(100);
      time += 100;
      continue;
    }
    uchar b;
    if(!readByte(&b)) {
      QThread::msleep(100);
      time += 100;
      continue;
    }

    if(b == '\r') {
    } else if(b == '\n') {
      return line;
    } else {
      line += b;
    }
  }
  return line;
}

void InputPlugin::clearRead() {
  uchar b;
  while(available() > 0 && readByte(&b)) {
  }
}
