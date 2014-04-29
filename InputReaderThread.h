#ifndef INPUTREADERTHREAD_H
#define INPUTREADERTHREAD_H

#include <QThread>
#include "plugins/InputPlugin.h"
#include "utils/RingBuffer.h"

class InputReaderThread;

class InputReaderThreadTarget {
public:
  virtual void onInputReaderThreadMessage(InputReaderThread* inputReaderThread, const QString& line) = 0;
};

class InputReaderThread : public QThread
{
  Q_OBJECT

public:
  InputReaderThread(InputReaderThreadTarget* inputReaderThreadTarget, InputPlugin* inputPlugin);
  qint64 read(unsigned char* buffer, qint64 bytesToRead);

  void stop();

protected:
  virtual void run();

private:
  bool m_run;
  InputReaderThreadTarget* m_inputReaderThreadTarget;
  InputPlugin* m_inputPlugin;
  RingBuffer m_ringBuffer;
};

#endif // INPUTREADERTHREAD_H
