#ifndef WIDGETPLUGININSTANCE_H
#define WIDGETPLUGININSTANCE_H

#include <QWidget>
#include <QObject>
#include "../InputReaderThread.h"

class WidgetPluginInstance : public QObject
{
  Q_OBJECT

public:
  WidgetPluginInstance();
  virtual ~WidgetPluginInstance();

  virtual void runCommand(InputReaderThread* inputReaderThread, const QString& functionName, QStringList args) = 0;
  virtual QWidget* getWidget() = 0;
};

#endif // WIDGETPLUGININSTANCE_H
