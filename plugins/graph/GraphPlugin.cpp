#include "GraphPlugin.h"
#include "GraphWidget.h"
#include "GraphWidgetPluginInstance.h"
#include <QDebug>

GraphPlugin::GraphPlugin()
{
}

WidgetPluginInstance* GraphPlugin::createInstance() const {
  return new GraphWidgetPluginInstance();
}
