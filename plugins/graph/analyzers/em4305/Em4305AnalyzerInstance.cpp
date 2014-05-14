#include "Em4305AnalyzerInstance.h"
#include "../../GraphWidgetPluginInstance.h"

Em4305AnalyzerInstance::Em4305AnalyzerInstance(const QString& name, GraphSignal* toDeviceSignal) :
  GraphAnalyzerInstance(name),
  m_toDeviceSignal(toDeviceSignal)
{
}
