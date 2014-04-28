#ifndef COMMANDRUNNER_H
#define COMMANDRUNNER_H

#include <QStringList>
#include "InputReaderThread.h"

class CommandRunnerWindow
{
public:
  virtual void runCommand(const QString& scope, const QString& functionName, QStringList args) = 0;
};

class CommandRunner : public InputReaderThreadTarget
{
public:
  CommandRunner(CommandRunnerWindow *window);

  void processLine(const QString& line);
  void runCommand(const QString& command);
  virtual void onInputReaderThreadMessage(const QString& line);

private:
  CommandRunnerWindow *m_window;

  QStringList splitArgs(const QString& args);
};

#endif // COMMANDRUNNER_H
