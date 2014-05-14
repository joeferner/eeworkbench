#ifndef EM4305ANALYZERINSTANCE_H
#define EM4305ANALYZERINSTANCE_H

#include "../../GraphAnalyzerInstance.h"

class GraphSignal;

class Em4305AnalyzerInstance : public GraphAnalyzerInstance
{
public:
  Em4305AnalyzerInstance(const QString& name, GraphSignal* toDeviceSignal);

private:
  GraphSignal* m_toDeviceSignal;
};

#endif // EM4305ANALYZERINSTANCE_H
