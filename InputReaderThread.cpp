#include "InputReaderThread.h"

InputReaderThread::InputReaderThread(InputReaderThreadTarget* inputReaderThreadTarget, InputPlugin* inputPlugin) :
  m_run(true),
  m_inputReaderThreadTarget(inputReaderThreadTarget),
  m_inputPlugin(inputPlugin)
{

}

void InputReaderThread::run() {
  while(m_run) {
    QThread::msleep(1000);
    m_inputReaderThreadTarget->onInputReaderThreadMessage();
  }
}

void InputReaderThread::stop() {
  m_run = false;
}
