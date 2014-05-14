#include "Em4305AnalyzerInstance.h"
#include "../../GraphWidgetPluginInstance.h"

#define RF_PERIOD_SECONDS  0.000008

Em4305AnalyzerInstance::Em4305AnalyzerInstance(const QString& name, int toDeviceSignalIndex) :
  GraphAnalyzerInstance(name),
  m_toDeviceSignalIndex(toDeviceSignalIndex) {
}

QString Em4305AnalyzerInstance::getConfig() const {
  return QString("toDevice=%1")
         .arg(m_toDeviceSignalIndex);
}

void Em4305AnalyzerInstance::refresh(GraphWidgetPluginInstance* graphWidgetPluginInstance) {
  int dataAvailable = graphWidgetPluginInstance->getBufferAvailable();
  int i = 0;

  clearMarks();

  while(i < dataAvailable) {
    int startIndex, endIndex;

    // field stop
    if(!findFieldStop(graphWidgetPluginInstance, i, m_toDeviceSignalIndex, &startIndex, &endIndex)) {
      return;
    }
    addMark(startIndex, endIndex, m_toDeviceSignalIndex, "Field Stop");
    i = endIndex;

    QList<BitData*> bits = readBits(graphWidgetPluginInstance, i, m_toDeviceSignalIndex);
    addMark(startIndex, endIndex, m_toDeviceSignalIndex, "0");
  }
}

QList<Em4305AnalyzerInstance::BitData*> Em4305AnalyzerInstance::readBits(GraphWidgetPluginInstance* graphWidgetPluginInstance, int i, int signalIndex) {
  QList<BitData*> results;
  int dataAvailable = graphWidgetPluginInstance->getBufferAvailable();
  int oneStartIndex, oneEndIndex;
  int zeroStartIndex, zeroEndIndex;
  BitData* data;
  while(i < dataAvailable) {
    oneStartIndex = graphWidgetPluginInstance->findBinarySample(i, signalIndex, 1);
    if(oneStartIndex < 0) {
      return results;
    }

    zeroStartIndex = oneEndIndex = graphWidgetPluginInstance->findBinarySample(oneStartIndex, signalIndex, 0);
    if(oneEndIndex < 0) {
      return results;
    }

    zeroEndIndex = graphWidgetPluginInstance->findBinarySample(zeroStartIndex, signalIndex, 1);
    if(zeroEndIndex < 0) {
      return results;
    }

    double tOne = (oneEndIndex - oneStartIndex) * graphWidgetPluginInstance->getTimePerSample();
    double tZero = (zeroEndIndex - zeroStartIndex) * graphWidgetPluginInstance->getTimePerSample();

    // zero
    if((tOne > (RF_PERIOD_SECONDS * 15)) && (tOne < (RF_PERIOD_SECONDS * 21))
        && (tZero > (RF_PERIOD_SECONDS * 11)) && (tZero < (RF_PERIOD_SECONDS * 17))) {
      data = new BitData();
      data->bit = 0;
      data->startIndex = oneStartIndex;
      data->endIndex = zeroEndIndex;
      results.append(data);
      i = zeroEndIndex;
    }

    else {
      while(tOne > (RF_PERIOD_SECONDS * 32)) {
        data = new BitData();
        data->bit = 1;
        data->startIndex = oneStartIndex;
        data->endIndex = oneStartIndex + ((RF_PERIOD_SECONDS * 32) / graphWidgetPluginInstance->getTimePerSample());
        results.append(data);
        tOne -= RF_PERIOD_SECONDS * 32;
        i = data->endIndex;
      }
    }
  }
  return results;
}

bool Em4305AnalyzerInstance::findFieldStop(GraphWidgetPluginInstance* graphWidgetPluginInstance, int i, int signalIndex, int* startIndex, int* endIndex) {
  int dataAvailable = graphWidgetPluginInstance->getBufferAvailable();
  while(i < dataAvailable) {
    int zeroIndex = graphWidgetPluginInstance->findBinarySample(i, signalIndex, 0);
    if(zeroIndex < 0) {
      return false;
    }
    int oneIndex = graphWidgetPluginInstance->findBinarySample(zeroIndex, signalIndex, 1);
    if(oneIndex < 0) {
      return false;
    }
    double t = (oneIndex - zeroIndex) * graphWidgetPluginInstance->getTimePerSample();
    if((t > (RF_PERIOD_SECONDS * 40)) && (t < (RF_PERIOD_SECONDS * 60))) {
      *startIndex = zeroIndex;
      *endIndex = oneIndex;
      return true;
    }
    i = oneIndex;
  }
  return false;
}
