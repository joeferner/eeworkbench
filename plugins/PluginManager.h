#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "InputPlugin.h"
#include "WidgetPlugin.h"
#include <QList>

class FileInputPlugin;

class PluginManager {
public:
  PluginManager();

  const WidgetPlugin* getWidgetPlugin(const QString& name);
  const QList<InputPlugin*> getInputPlugins() const { return m_inputPlugins; }
  FileInputPlugin* getFileInputPlugins() const { return m_fileInputPlugin; }

private:
  QList<InputPlugin*> m_inputPlugins;
  QList<WidgetPlugin*> m_widgetPlugins;
  FileInputPlugin* m_fileInputPlugin;
};

#endif // PLUGINMANAGER_H
