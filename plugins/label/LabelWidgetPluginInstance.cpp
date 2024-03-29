#include "LabelWidgetPluginInstance.h"
#include <QDebug>

LabelWidgetPluginInstance::LabelWidgetPluginInstance(WidgetPlugin* widgetPlugin, const QString& name) :
  WidgetPluginInstance(widgetPlugin, name) {
  m_widget = new LabelWidget(this);
}

LabelWidgetPluginInstance::~LabelWidgetPluginInstance() {
  delete m_widget;
}

void LabelWidgetPluginInstance::runCommand(const QString& functionName, QStringList args, InputPlugin* inputPlugin) {
  if(functionName == "set") {
    if(args.length() == 2) {
      set(args.at(0), args.at(1));
    } else {
      qWarning() << "Label: set: Invalid number of arguments. Expected 2, found " << args.length();
    }
  } else {
    WidgetPluginInstance::runCommand(functionName, args, inputPlugin);
  }
}

void LabelWidgetPluginInstance::set(const QString& name, const QString& value) {
  if(name == "title") {
    m_widget->setTitle(value);
  } else if(name == "text") {
    m_widget->setText(value);
  } else {
    WidgetPluginInstance::set(name, value);
  }
}

void LabelWidgetPluginInstance::save(QTextStream& out) {
  out << QString("!%1.set title,\"%2\"\n").arg(getName()).arg(m_widget->getTitle());
  out << QString("!%1.set text,\"%2\"\n").arg(getName()).arg(m_widget->getText());
}
