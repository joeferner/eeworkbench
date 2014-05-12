#ifndef COMMANDRUNNER_H
#define COMMANDRUNNER_H

#include <QStringList>
#include "InputReaderThread.h"

class CommandRunnerWindow {
public:
  virtual void runCommand(InputReaderThread* inputReaderThread, const QString& scope, const QString& functionName, QStringList args) = 0;
};

class CommandRunner : public InputReaderThreadTarget {
public:
  CommandRunner(CommandRunnerWindow* window);

  void processLine(InputReaderThread* inputReaderThread, const QString& line);
  void runCommand(InputReaderThread* inputReaderThread, const QString& command);
  virtual void onInputReaderThreadMessage(InputReaderThread* inputReaderThread, const QString& line);

private:
  CommandRunnerWindow* m_window;

  QStringList splitArgs(const QString& args);
};

#endif // COMMANDRUNNER_H
