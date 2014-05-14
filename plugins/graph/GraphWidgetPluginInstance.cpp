#include "GraphWidgetPluginInstance.h"
#include "GraphWidget.h"
#include "GraphAnalyzerInstance.h"
#include "analyzers/em4305/Em4305Analyzer.h"
#include <math.h>
#include <QDebug>

GraphWidgetPluginInstance::GraphWidgetPluginInstance(WidgetPlugin* widgetPlugin, const QString& name) :
  WidgetPluginInstance(widgetPlugin, name),
  m_timePerSample(0.001) {

  m_bufferSize = 10 * 1024 * 1024;
  m_buffer = new unsigned char[m_bufferSize];

  m_colorsCount = 3;
  m_colors = new QColor[m_colorsCount];
  m_colors[0].setRgb(255, 0, 0);
  m_colors[1].setRgb(0, 255, 0);
  m_colors[2].setRgb(0, 0, 255);

  m_graphAnalyzers.append(new Em4305Analyzer());

  m_widget = new GraphWidget(this);
  connect(this, SIGNAL(dataAdded()), m_widget, SLOT(updateData()));
  clear();
}

GraphWidgetPluginInstance::~GraphWidgetPluginInstance() {
  delete m_colors;
  foreach(GraphSignal * signal, m_signals) {
    delete signal;
  }
  m_signals.clear();

  disconnect(this, SIGNAL(dataAdded()), m_widget, SLOT(repaint()));
  delete m_widget;
}

void GraphWidgetPluginInstance::clear() {
  m_bufferWritePos = 0;
  m_bufferAvailable = 0;
  m_widget->update();
}

void GraphWidgetPluginInstance::save(QTextStream& out) {
  out << QString("!%1.set timePerSample,%2\n").arg(getName()).arg(m_timePerSample);

  foreach(GraphSignal * signal, m_signals) {
    out << QString("!%1.addSignal %2,%3,%4,%5\n")
        .arg(getName())
        .arg(signal->name)
        .arg(signal->bits)
        .arg(signal->scaleMin)
        .arg(signal->scaleMax);
  }

  foreach(GraphAnalyzerInstance * graphAnalyzerInstance, m_graphAnalyzerInstances) {
    out << QString("!%1.addAnalyzer %2,\"%3\"\n")
        .arg(getName())
        .arg(graphAnalyzerInstance->getName())
        .arg(graphAnalyzerInstance->getConfig());
  }

  out << QString("!%1.beginData %2\n").arg(getName()).arg(m_bufferAvailable);
  out.flush();
  out.device()->write((const char*)m_buffer, m_bufferAvailable);
  out << "\n";
}

void GraphWidgetPluginInstance::runCommand(const QString& functionName, QStringList args, InputPlugin* inputPlugin) {
  if(functionName == "data") {
    if(args.length() == m_signals.length()) {
      addData(args);
    } else {
      qWarning() << "Graph: data: Invalid number of arguments. Expected" << m_signals.length() << ", found " << args.length();
    }
  } else if(functionName == "beginData") {
    if(args.length() == 1) {
      beginData(inputPlugin, args.at(0).toInt());
    } else {
      qWarning() << "Graph: beginData: Invalid number of arguments. Expected 1, found " << args.length();
    }
  } else if(functionName == "addSignal") {
    if(args.length() == 4) {
      addSignal(args.at(0), args.at(1).toInt(), args.at(2).toDouble(), args.at(3).toDouble());
    } else {
      qWarning() << "Graph: addSignal: Invalid number of arguments. Expected 4, found " << args.length();
    }
  } else if(functionName == "set") {
    if(args.length() == 2) {
      set(args.at(0), args.at(1));
    } else {
      qWarning() << "Graph: set: Invalid number of arguments. Expected 2, found " << args.length();
    }
  } else if(functionName == "clear") {
    if(args.length() == 0) {
      clear();
    } else {
      qWarning() << "Graph: clear: Invalid number of arguments. Expected 0, found " << args.length();
    }
  } else if(functionName == "addAnalyzer") {
    if(args.length() == 2) {
      addAnalyzer(args.at(0), args.at(1));
    } else {
      qWarning() << "Graph: addGraphAnalyzer: Invalid number of arguments. Expected 2, found " << args.length();
    }
  } else {
    qWarning() << "Graph: Unknown Command" << functionName << args;
  }
}

void GraphWidgetPluginInstance::set(const QString& name, const QString& value) {
  if(name == "timePerSample") {
    m_timePerSample = value.toDouble();
  } else {
    qWarning() << "Graph: Unknown set command" << name;
  }
}

void GraphWidgetPluginInstance::beginData(InputPlugin* inputPlugin, int numberOfBytes) {
  while(numberOfBytes > 0) {
    qint64 read = qMin(numberOfBytes, m_bufferSize - m_bufferWritePos);
    read = inputPlugin->read(&m_buffer[m_bufferWritePos], read);
    incrementBufferWritePos(read);
    numberOfBytes -= read;
  }
}

void GraphWidgetPluginInstance::addData(QStringList args) {
  unsigned char temp = 0;
  int tempBit = 0;
  for(int i = 0; i < qMin(args.length(), m_signals.length()); i++) {
    uint arg = args.at(i).toUInt();
    GraphSignal* signal = m_signals.at(i);
    int signalBitsShift = signal->bits - 1;
    while(signalBitsShift >= 0) {
      unsigned char bit = (arg >> signalBitsShift) & 0x01;
      temp |= bit << (7 - tempBit);

      signalBitsShift--;
      tempBit++;
      if(tempBit == 8) {
        writeByteToBuffer(temp);
        temp = 0;
        tempBit = 0;
      }
    }
  }

  writeByteToBuffer(temp);

  emit dataAdded();
}

void GraphWidgetPluginInstance::writeByteToBuffer(unsigned char b) {
  //qDebug() << "m_buffer[" << m_bufferWritePos << "] = " << QString::number(b, 16);
  m_buffer[m_bufferWritePos] = b;
  incrementBufferWritePos(1);
}

void GraphWidgetPluginInstance::incrementBufferWritePos(int i) {
  m_bufferAvailable += i;
  if(m_bufferAvailable >= m_bufferSize) {
    m_bufferAvailable = m_bufferSize;
  }

  m_bufferWritePos += i;
  while(m_bufferWritePos >= m_bufferSize) {
    m_bufferWritePos -= m_bufferSize;
  }
}

void GraphWidgetPluginInstance::addSignal(const QString& name, int bits, double scaleMin, double scaleMax) {
  GraphSignal* signal = new GraphSignal();
  signal->name = name;
  signal->bits = bits;
  signal->maxValue = bits == 1 ? 1 : pow(2, bits);
  signal->scaleMin = scaleMin;
  signal->scaleMax = scaleMax;
  signal->color = m_colors[m_signals.length() % m_colorsCount];
  m_signals.append(signal);
}

int GraphWidgetPluginInstance::getBytesPerSample() const {
  int bits = 0;
  foreach(GraphSignal * signal, m_signals) {
    bits += signal->bits;
  }
  return ceil((float)bits / 8.0);
}

double GraphWidgetPluginInstance::getValue(int sample, int signalNumber) {
  int bytesPerSample = getBytesPerSample();
  if(bytesPerSample == 0) {
    return 0.0;
  }
  int totalNumberOfSamples = m_bufferSize / bytesPerSample;
  while(sample < 0) {
    sample += totalNumberOfSamples;
  }
  while(sample >= totalNumberOfSamples) {
    sample -= totalNumberOfSamples;
  }

  int bufferIndex = bytesPerSample * sample;
  unsigned char bufferTemp = getBuffer()[bufferIndex];
  int bufferTempBit = 0;

  uint temp = 0;
  const GraphSignal* signal = NULL;
  for(int s = 0; s <= signalNumber; s++) {
    signal = m_signals.at(s);
    temp = 0;
    for(int signalBit = 0; signalBit < signal->bits; signalBit++) {
      temp = (temp << 1) | (bufferTemp & 0x80 ? 0x01 : 0x00);
      bufferTemp = bufferTemp << 1;
      bufferTempBit++;
      if(bufferTempBit == 8) {
        bufferIndex++;
        if(bufferIndex == getBufferSize()) {
          bufferIndex = 0;
        }
        bufferTempBit = 0;
        bufferTemp = getBuffer()[bufferIndex];
      }
    }
  }

  if(signal == NULL) {
    return 0;
  }

  return (double)temp / (double)(signal->maxValue) * (signal->scaleMax - signal->scaleMin) + signal->scaleMin;
}

int GraphWidgetPluginInstance::getBinaryValue(int sample, int signalIndex) {
  const GraphSignal* signal = m_signals.at(signalIndex);
  double mid = (signal->scaleMax + signal->scaleMin) / 2;
  double v = getValue(sample, signalIndex);
  return v > mid ? 1 : 0;
}

int GraphWidgetPluginInstance::findBinarySample(int startingSample, int signalNumber, int value) {
  return findBinarySample(startingSample, signalNumber, 1, value);
}

int GraphWidgetPluginInstance::findBinarySample(int startingSample, int signalNumber, int direction, int value) {
  int count;
  int sample;
  for(count = 0, sample = startingSample; count < m_bufferAvailable; count++, sample += direction) {
    int v = getBinaryValue(sample, signalNumber);
    if(v == value) {
      return sample;
    }
  }
  return -1;
}

int GraphWidgetPluginInstance::findSample(int startingSample, int signalNumber, int direction, sampleCompareFn fn) {
  double startingValue = getValue(startingSample, signalNumber);
  int count;
  int sample;
  double previousValue = startingValue;
  for(count = 0, sample = startingSample + direction; count < m_bufferAvailable; count++, sample += direction) {
    double newValue = getValue(sample, signalNumber);
    if(fn(startingValue, previousValue, newValue)) {
      if(direction < 0) {
        sample++;
      }
      return sample;
    }
    previousValue = newValue;
  }
  return -1;
}

bool GraphWidgetPluginInstance::sampleCompareNotEqual(double originalValue, double d1, double d2) {
  return fabs(originalValue - d2) > 0.000000001;
}

bool GraphWidgetPluginInstance::sampleCompareEqual(double originalValue, double d1, double d2) {
  return fabs(originalValue - d2) < 0.000000001;
}

bool GraphWidgetPluginInstance::sampleCompareFallingThrough(double originalValue, double d1, double d2) {
  return d1 >= originalValue && d2 < originalValue;
}

bool GraphWidgetPluginInstance::sampleCompareRisingThrough(double originalValue, double d1, double d2) {
  return d1 <= originalValue && d2 > originalValue;
}

GraphAnalyzer* GraphWidgetPluginInstance::getAnalyzer(const QString& name) {
  foreach(GraphAnalyzer * graphAnalyzer, m_graphAnalyzers) {
    if(graphAnalyzer->getName() == name) {
      return graphAnalyzer;
    }
  }
  return NULL;
}

void GraphWidgetPluginInstance::addAnalyzer(const QString& name, const QString& config) {
  GraphAnalyzer* graphAnalyzer = getAnalyzer(name);
  if(graphAnalyzer == NULL) {
    qDebug() << "Could not find analyzer " << name;
    return;
  }

  GraphAnalyzerInstance* graphAnalyzerInstance = graphAnalyzer->create((GraphWidget*)getWidget(), this, config);
  if(graphAnalyzerInstance == NULL) {
    return;
  }
  m_graphAnalyzerInstances.append(graphAnalyzerInstance);
}

void GraphWidgetPluginInstance::addAnalyzer(GraphAnalyzer* graphAnalyzer) {
  GraphAnalyzerInstance* graphAnalyzerInstance = graphAnalyzer->configure((GraphWidget*)getWidget(), this);
  if(graphAnalyzerInstance) {
    m_graphAnalyzerInstances.append(graphAnalyzerInstance);
  }
}

int GraphWidgetPluginInstance::getSignalIndex(GraphSignal* signal) {
  int i = 0;
  foreach(GraphSignal * s, m_signals) {
    if(s == signal) {
      return i;
    }
    i++;
  }
  return -1;
}

