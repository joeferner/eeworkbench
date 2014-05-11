#include "WidgetPluginInstance.h"

WidgetPluginInstance::WidgetPluginInstance(WidgetPlugin* widgetPlugin, const QString& name) :
  m_widgetPlugin(widgetPlugin),
  m_name(name)
{
}

WidgetPluginInstance::~WidgetPluginInstance() {

}
