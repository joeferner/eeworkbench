#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "InputPlugin.h"
#include "WidgetPlugin.h"
#include <QList>

class PluginManager
{
public:
  PluginManager();

  const WidgetPlugin* getWidgetPlugin(const QString& name);
  const QList<InputPlugin*> getInputPlugins() const { return m_inputPlugins; }

private:
  QList<InputPlugin*> m_inputPlugins;
  QList<WidgetPlugin*> m_widgetPlugins;
};

#endif // PLUGINMANAGER_H
