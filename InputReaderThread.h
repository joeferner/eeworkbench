#ifndef INPUTREADERTHREAD_H
#define INPUTREADERTHREAD_H

#include <QThread>
#include "plugins/InputPlugin.h"
#include "utils/RingBuffer.h"

class InputReaderThreadTarget {
public:
  virtual void onInputReaderThreadMessage(const QString& line) = 0;
};

class InputReaderThread : public QThread
{
  Q_OBJECT

public:
  InputReaderThread(InputReaderThreadTarget* inputReaderThreadTarget, InputPlugin* inputPlugin);

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
