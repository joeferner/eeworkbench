#include "PluginManager.h"
#include "fileInput/FileInputPlugin.h"
#include "graph/GraphPlugin.h"

PluginManager::PluginManager() :
  m_activeInputPlugin(0)
{
  m_inputPlugins.append(new FileInputPlugin());

  m_widgetPlugins.append(new GraphPlugin());
}

InputPlugin* PluginManager::getActiveInputPlugin() {
  if(m_activeInputPlugin < 0 || m_activeInputPlugin >= m_inputPlugins.count()) {
    return NULL;
  }
  return m_inputPlugins.at(m_activeInputPlugin);
}
