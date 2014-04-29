#ifndef WIDGETPLUGININSTANCE_H
#define WIDGETPLUGININSTANCE_H

#include <QWidget>

class WidgetPluginInstance
{
public:
  WidgetPluginInstance();
  virtual ~WidgetPluginInstance();

  virtual void runCommand(const QString& functionName, QStringList args) = 0;
  virtual QWidget* getWidget() = 0;
};

#endif // WIDGETPLUGININSTANCE_H
