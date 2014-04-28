#ifndef WIDGETPLUGIN_H
#define WIDGETPLUGIN_H

#include "Plugin.h"

class WidgetPlugin : public Plugin
{
public:
  WidgetPlugin();

  virtual QWidget* create() const = 0;
  virtual void runCommand(QWidget* scope, const QString& functionName, QStringList args) const = 0;
};

#endif // WIDGETPLUGIN_H
