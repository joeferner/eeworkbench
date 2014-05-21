#include "Em4305AnalyzerInstance.h"
#include "../../GraphWidgetPluginInstance.h"

#define RF_PERIOD_SECONDS  0.000008
#define COMMAND_LOGIN      0x3
#define COMMAND_WRITE_WORD 0x5
#define COMMAND_READ_WORD  0x9
#define COMMAND_PROTECT    0xc
#define COMMAND_DISABLE    0xa

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
  int sampleIndex = 0;

  clearMarks();

  while(sampleIndex < dataAvailable) {
    int startIndex, endIndex;

    // field stop
    if(!findFieldStop(graphWidgetPluginInstance, sampleIndex, m_toDeviceSignalIndex, &startIndex, &endIndex)) {
      return;
    }
    addMark(startIndex, endIndex, m_toDeviceSignalIndex, "Field Stop");
    sampleIndex = endIndex;

    QList<BitData*> bits = readBits(graphWidgetPluginInstance, sampleIndex, m_toDeviceSignalIndex);
    int bitCount = bits.length();
    int i = 0;

    // field stop 0
    if(i < bitCount) {
      BitData* bit = bits.at(i);
      if(bit->bit == 0) {
        addMark(bit->startIndex, bit->endIndex, m_toDeviceSignalIndex, "0");
      } else {
        addMark(bit->startIndex, bit->endIndex, m_toDeviceSignalIndex, "!0");
      }
      i++;
    }

    // command
    int cmd;
    if(i + 4 <= bitCount) {
      BitData* bit0 = bits.at(i++);
      BitData* bit1 = bits.at(i++);
      BitData* bit2 = bits.at(i++);
      BitData* bit3 = bits.at(i++);
      cmd = (bit0->bit << 3) | (bit1->bit << 2) | (bit2->bit << 1) | (bit3->bit << 0);
      QString cmdStr;
      switch(cmd) {
      case COMMAND_LOGIN:
        cmdStr = "Login (0011)";
        break;
      case COMMAND_WRITE_WORD:
        cmdStr = "Write (0101)";
        break;
      case COMMAND_READ_WORD:
        cmdStr = "Read (1001)";
        break;
      case COMMAND_PROTECT:
        cmdStr = "Protect (1100)";
        break;
      case COMMAND_DISABLE:
        cmdStr = "Disable (1010)";
        break;
      default:
        cmdStr = QString("Unknown (%1%2%3%4)")
                 .arg(bit0->bit)
                 .arg(bit1->bit)
                 .arg(bit2->bit)
                 .arg(bit3->bit);
        break;
      }
      addMark(bit0->startIndex, bit3->endIndex, m_toDeviceSignalIndex, cmdStr);
    }

    // address
    int address;
    if(cmd == COMMAND_READ_WORD || cmd == COMMAND_WRITE_WORD) {
      BitData* bit0 = bits.at(i++);
      BitData* bit1 = bits.at(i++);
      BitData* bit2 = bits.at(i++);
      BitData* bit3 = bits.at(i++);
      address = (bit0->bit << 3) | (bit1->bit << 2) | (bit2->bit << 1) | (bit3->bit << 0);
      BitData* bitZero0 = bits.at(i++);
      BitData* bitZero1 = bits.at(i++);
      BitData* bitParity = bits.at(i++);

      QString addressStr = QString("Addr %1 Parity %2")
                           .arg(address)
                           .arg(bitParity->bit);
      if(bitZero0->bit != 0) {
        addressStr += " bad zero(0) bit";
      }
      if(bitZero1->bit != 0) {
        addressStr += " bad zero(1) bit";
      }
      addMark(bit0->startIndex, bitParity->endIndex, m_toDeviceSignalIndex, addressStr);
    }

    // rest
    for(; i < bitCount; i++) {
      BitData* bit = bits.at(i);
      addMark(bit->startIndex, bit->endIndex, m_toDeviceSignalIndex, bit->bit ? "1" : "0");
    }

    foreach(BitData * bit, bits) {
      delete bit;
    }
  }
}

QList<Em4305AnalyzerInstance::BitData*> Em4305AnalyzerInstance::readBits(GraphWidgetPluginInstance* graphWidgetPluginInstance, int startSample, int signalIndex) {
  QList<BitData*> results;
  int dataAvailable = graphWidgetPluginInstance->getBufferAvailable();
  unsigned char* dataBuffer = new unsigned char[dataAvailable];
  int dataLength = graphWidgetPluginInstance->getBinaryValues(startSample, signalIndex, dataBuffer, dataAvailable);

  int oneStartIndex, oneEndIndex;
  int zeroStartIndex, zeroEndIndex;
  for(int i = 0; i < dataLength; ) {
    if(!findBit(dataBuffer, dataLength, graphWidgetPluginInstance->getTimePerSample(), i, 1, &oneStartIndex, &oneEndIndex)) {
      break;
    }
    if(!findBit(dataBuffer, dataLength, graphWidgetPluginInstance->getTimePerSample(), oneEndIndex, 0, &zeroStartIndex, &zeroEndIndex)) {
      break;
    }

    double tOne = (oneEndIndex - oneStartIndex) * graphWidgetPluginInstance->getTimePerSample();
    double tZero = (zeroEndIndex - zeroStartIndex) * graphWidgetPluginInstance->getTimePerSample();

    // zero
    if((tOne > (RF_PERIOD_SECONDS * 15)) && (tOne < (RF_PERIOD_SECONDS * 21))
        && (tZero > (RF_PERIOD_SECONDS * 11)) && (tZero < (RF_PERIOD_SECONDS * 17))) {
      BitData* data = new BitData();
      data->bit = 0;
      data->startIndex = startSample + oneStartIndex;
      data->endIndex = startSample + zeroEndIndex;
      results.append(data);
      i = zeroEndIndex;
    }

    else if(tOne > (RF_PERIOD_SECONDS * 32)) {
      while(tOne > (RF_PERIOD_SECONDS * 32)) {
        BitData* data = new BitData();
        data->bit = 1;
        data->startIndex = startSample + oneStartIndex;
        data->endIndex = startSample + oneStartIndex + ((RF_PERIOD_SECONDS * 32) / graphWidgetPluginInstance->getTimePerSample());
        results.append(data);
        tOne -= RF_PERIOD_SECONDS * 32;
        i = data->endIndex - startSample;
      }
    }

    else {
      i++;
    }
  }

  delete dataBuffer;
  return results;
}

bool Em4305AnalyzerInstance::findBit(unsigned char* data, int dataLen, double timePerSample, int dataIndex, int bitToFind, int* startIndex, int* endIndex) {
  int minBitWidth = (5 * RF_PERIOD_SECONDS) / timePerSample;
  int lastDataChangeIndex = dataIndex;
  unsigned char lastDataBit = data[dataIndex];
  *startIndex = dataIndex;
  if(bitToFind) {
    for(int i = dataIndex; i < dataLen; i++) {
      if(data[i] != lastDataBit) {
        lastDataBit = data[i];
        lastDataChangeIndex = i;
      }
      if((i - lastDataChangeIndex) > minBitWidth) {
        *endIndex = lastDataChangeIndex;
        return true;
      }
    }
  } else {
    for(int i = dataIndex; i < dataLen; i++) {
      if(data[i] != lastDataBit) {
        *startIndex = i;
        lastDataBit = data[i];
        lastDataChangeIndex = i;
      }
      if((i - lastDataChangeIndex) > minBitWidth) {
        // find the end of the zero bit
        for(; (i < dataLen) && (data[i] == lastDataBit); i++) {
        }
        *endIndex = i;
        return true;
      }
    }
  }

  return false;
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
