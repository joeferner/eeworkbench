#ifndef WIDGETPLUGININSTANCE_H
#define WIDGETPLUGININSTANCE_H

#include <QWidget>
#include <QObject>
#include <QTextStream>
#include "InputPlugin.h"

class WidgetPlugin;

class WidgetPluginInstance : public QObject {
  Q_OBJECT

public:
  WidgetPluginInstance(WidgetPlugin* widgetPlugin, const QString& name);
  virtual ~WidgetPluginInstance();

  virtual void runCommand(const QString& functionName, QStringList args, InputPlugin* inputPlugin) = 0;
  virtual QWidget* getWidget() = 0;
  virtual void save(QTextStream& out) = 0;
  WidgetPlugin* getWidgetPlugin() { return m_widgetPlugin; }
  QString getName() { return m_name; }

private:
  WidgetPlugin* m_widgetPlugin;
  QString m_name;
};

#endif // WIDGETPLUGININSTANCE_H
