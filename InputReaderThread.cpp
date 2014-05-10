#include "InputReaderThread.h"

#define BUFFER_SIZE 1024

InputReaderThread::InputReaderThread(InputReaderThreadTarget* inputReaderThreadTarget, InputPlugin* inputPlugin) :
  m_run(true),
  m_inputReaderThreadTarget(inputReaderThreadTarget),
  m_inputPlugin(inputPlugin),
  m_ringBuffer(BUFFER_SIZE),
  m_stopped(true)
{
}

void InputReaderThread::run() {
  m_stopped = false;
  while(m_run) {
    if(m_inputPlugin->available() == 0) {
      msleep(100);
      continue;
    }
    uchar b;
    if(!m_inputPlugin->readByte(&b)) {
      msleep(100);
      continue;
    }

    if(b == '\r') {
    } else if(b == '\n') {
      QString str = m_ringBuffer.readAsString();
      m_inputReaderThreadTarget->onInputReaderThreadMessage(this, str);
    } else {
      m_ringBuffer.write(b);
    }
  }
  m_stopped = true;
}

qint64 InputReaderThread::read(unsigned char* buffer, qint64 bytesToRead) {
  return m_inputPlugin->read(buffer, bytesToRead);
}

void InputReaderThread::stop() {
  m_run = false;
  for(int i = 0; !m_stopped && i < 10; i++) {
    msleep(100);
  }
}
