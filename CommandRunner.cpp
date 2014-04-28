#include "CommandRunner.h"
#include <QDebug>

CommandRunner::CommandRunner(CommandRunnerWindow *window) :
  m_window(window)
{
}

void CommandRunner::onInputReaderThreadMessage(const QString& line) {
  processLine(line);
}

void CommandRunner::processLine(const QString& line) {
  if(line.length() == 0) {
    return;
  }

  QChar type = line.at(0);
  if(type == '?') {
    qDebug() << line.mid(1);
  } else if(type == '!') {
    runCommand(line.mid(1).trimmed());
  } else {
    qDebug() << "MainWindow: Unknown line type:" << line;
  }
}

void CommandRunner::runCommand(const QString& command) {
  int scopeAndFunctionNameIndex = command.indexOf(' ');
  if(scopeAndFunctionNameIndex < 0) {
    scopeAndFunctionNameIndex = command.length() - 1;
  }
  QString scopeAndFunctionName = command.left(scopeAndFunctionNameIndex);
  QString args = command.mid(scopeAndFunctionNameIndex + 1);

  int scopeIndex = scopeAndFunctionName.indexOf('.');
  if(scopeIndex < 0) {
    scopeIndex = -1;
  }
  QString scope = scopeAndFunctionName.left(qMax(0, scopeIndex));
  if(scope == "") {
    scope = "window";
  }
  QString functionName = scopeAndFunctionName.mid(scopeIndex + 1);

  QStringList argsList = splitArgs(args);

  m_window->runCommand(scope, functionName, argsList);
}

QStringList CommandRunner::splitArgs(const QString& argsString) {
  QStringList results;

  QString arg = "";
  QChar startCh;
  for(int i=0; i<argsString.length(); i++) {
    QChar ch = argsString.at(i);
    if(ch == '"' || ch == '\'') {
      startCh = ch;
      i++;
      for(; i<argsString.length(); i++) {
        ch = argsString.at(i);
        if(ch == startCh) {
          i++;
          break;
        }
        arg.append(ch);
      }
    } else if(ch == ',') {
      results.append(arg.trimmed());
      arg.clear();
    } else {
      arg.append(ch);
    }
  }
  results.append(arg.trimmed());

  return results;
}
