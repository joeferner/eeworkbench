#ifndef WIDGETPLUGIN_H
#define WIDGETPLUGIN_H

#include "Plugin.h"
#include "WidgetPluginInstance.h"

class WidgetPlugin : public Plugin
{
public:
  WidgetPlugin();

  virtual WidgetPluginInstance* createInstance() const = 0;
};

#endif // WIDGETPLUGIN_H
