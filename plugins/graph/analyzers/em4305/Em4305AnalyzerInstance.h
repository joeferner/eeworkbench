#ifndef EM4305ANALYZERINSTANCE_H
#define EM4305ANALYZERINSTANCE_H

#include "../../GraphAnalyzerInstance.h"
#include <QList>

class GraphSignal;

class Em4305AnalyzerInstance : public GraphAnalyzerInstance {
public:
  Em4305AnalyzerInstance(const QString& name, int toDeviceSignalIndex);

  virtual void refresh(GraphWidgetPluginInstance* graphWidgetPluginInstance);
  virtual QString getConfig() const;

private:
  int m_toDeviceSignalIndex;

  struct BitData {
    int bit;
    int startIndex;
    int endIndex;
  };

  bool findFieldStop(GraphWidgetPluginInstance* graphWidgetPluginInstance, int i, int signalIndex, int* startIndex, int* endIndex);
  QList<BitData*> readBits(GraphWidgetPluginInstance* graphWidgetPluginInstance, int i, int signalIndex);
};

#endif // EM4305ANALYZERINSTANCE_H
