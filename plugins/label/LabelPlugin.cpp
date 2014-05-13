#include "LabelPlugin.h"
#include "LabelWidgetPluginInstance.h"

LabelPlugin::LabelPlugin() {
}

WidgetPluginInstance* LabelPlugin::createInstance(const QString& name) const {
  return new LabelWidgetPluginInstance((WidgetPlugin*)this, name);
}
