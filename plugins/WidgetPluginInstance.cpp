#include "WidgetPluginInstance.h"
#include <QDebug>

WidgetPluginInstance::WidgetPluginInstance(WidgetPlugin* widgetPlugin, const QString& name) :
  m_widgetPlugin(widgetPlugin),
  m_name(name) {
}

WidgetPluginInstance::~WidgetPluginInstance() {

}

void WidgetPluginInstance::runCommand(const QString& functionName, QStringList args, InputPlugin*) {
  if(functionName == "set") {
    if(args.length() == 2) {
      set(args.at(0), args.at(1));
    } else {
      qWarning() << "set: Invalid number of arguments. Expected 2, found " << args.length();
    }
  } else {
    qWarning() << getName() << ": Unknown Command" << functionName << args;
  }
}

void WidgetPluginInstance::set(const QString& name, const QString& value) {
  if(name == "minWidth") {
    getWidget()->setMinimumWidth(value.toInt());
  } else {
    qWarning() << getName() << ": Unknown set command" << name;
  }
}
