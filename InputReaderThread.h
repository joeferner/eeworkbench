#ifndef INPUTREADERTHREAD_H
#define INPUTREADERTHREAD_H

#include <QThread>
#include "plugins/InputPlugin.h"

class InputReaderThreadTarget {
public:
  virtual void onInputReaderThreadMessage() = 0;
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
};

#endif // INPUTREADERTHREAD_H
