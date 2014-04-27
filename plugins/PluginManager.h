#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "InputPlugin.h"
#include "WidgetPlugin.h"
#include <QList>

class PluginManager
{
public:
  PluginManager();

  InputPlugin* getActiveInputPlugin();

private:
  int m_activeInputPlugin;
  QList<InputPlugin*> m_inputPlugins;
  QList<WidgetPlugin*> m_widgetPlugins;
};

#endif // PLUGINMANAGER_H
