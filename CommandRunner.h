#ifndef COMMANDRUNNER_H
#define COMMANDRUNNER_H

#include <QStringList>

class CommandRunnerWindow {
public:
  virtual void runCommand(const QString& functionName, QStringList args) = 0;
};

class CommandRunner
{
public:
  CommandRunner(CommandRunnerWindow *window);

  void processLine(const QString& line);
  void runCommand(const QString& command);

private:
  CommandRunnerWindow *m_window;

  QStringList splitArgs(const QString& args);
};

#endif // COMMANDRUNNER_H
