#include "PluginManager.h"
#include "fileInput/FileInputPlugin.h"
#include "serialPort/SerialPortPlugin.h"
#include "graph/GraphPlugin.h"
#include "label/LabelPlugin.h"

PluginManager::PluginManager() {
  m_inputPlugins.append(m_fileInputPlugin = new FileInputPlugin());
  m_inputPlugins.append(new SerialPortPlugin());

  m_widgetPlugins.append(new GraphPlugin());
  m_widgetPlugins.append(new LabelPlugin());
}

const WidgetPlugin* PluginManager::getWidgetPlugin(const QString& name) {
  foreach(const WidgetPlugin * plugin, m_widgetPlugins) {
    if(QString::compare(plugin->getName(), name, Qt::CaseInsensitive) == 0) {
      return plugin;
    }
  }
  return NULL;
}
