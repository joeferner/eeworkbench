#ifndef LABELWIDGETPLUGININSTANCE_H
#define LABELWIDGETPLUGININSTANCE_H

#include "../WidgetPluginInstance.h"
#include "LabelWidget.h"

class LabelWidgetPluginInstance : public WidgetPluginInstance {
  Q_OBJECT

public:
  LabelWidgetPluginInstance(WidgetPlugin* widgetPlugin, const QString& name);

  virtual void runCommand(const QString& functionName, QStringList args, InputPlugin* inputPlugin);
  virtual QWidget* getWidget() { return m_widget; }
  virtual void save(QTextStream& out);

private:
  LabelWidget* m_widget;
  QString m_title;
  QString m_text;

  void set(const QString& name, const QString& value);
};

#endif // LABELWIDGETPLUGININSTANCE_H
