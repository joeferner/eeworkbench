#include "PluginManager.h"
#include "fileInput/FileInputPlugin.h"
#include "serialPort/SerialPortPlugin.h"
#include "graph/GraphPlugin.h"

PluginManager::PluginManager()
{
  m_inputPlugins.append(new FileInputPlugin());
  m_inputPlugins.append(new SerialPortPlugin());

  m_widgetPlugins.append(new GraphPlugin());
}

const WidgetPlugin* PluginManager::getWidgetPlugin(const QString& name) {
  foreach(const WidgetPlugin* plugin, m_widgetPlugins) {
    if(QString::compare(plugin->getName(), name, Qt::CaseInsensitive) == 0) {
      return plugin;
    }
  }
  return NULL;
}
