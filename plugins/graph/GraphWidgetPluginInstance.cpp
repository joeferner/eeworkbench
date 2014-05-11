#include "GraphWidgetPluginInstance.h"
#include "GraphWidget.h"
#include <math.h>
#include <QDebug>

GraphWidgetPluginInstance::GraphWidgetPluginInstance(WidgetPlugin* widgetPlugin, const QString& name) :
  WidgetPluginInstance(widgetPlugin, name),
  m_widget(NULL),
  m_timePerSample(0.001)
{
  m_bufferSize = 10 * 1024 * 1024;
  m_buffer = new unsigned char[m_bufferSize];
  m_bufferWritePos = 0;
  m_bufferAvailable = 0;

  m_colorsCount = 3;
  m_colors = new QColor[m_colorsCount];
  m_colors[0].setRgb(255, 0, 0);
  m_colors[1].setRgb(0, 255, 0);
  m_colors[2].setRgb(0, 0, 255);
}

GraphWidgetPluginInstance::~GraphWidgetPluginInstance() {
  delete m_colors;
  foreach(GraphSignal* signal, m_signals) {
    delete signal;
  }

  disconnect(this, SIGNAL(dataAdded()), m_widget, SLOT(repaint()));
  delete m_widget;
}

void GraphWidgetPluginInstance::save(QTextStream& out) {
  out << QString("!%1.set timePerSample,%2\n").arg(getName()).arg(m_timePerSample);

  foreach(GraphSignal* signal, m_signals) {
    out << QString("!%1.addSignal %2,%3,%4,%5\n").arg(getName()).arg(signal->name).arg(signal->bits).arg(signal->scaleMin).arg(signal->scaleMax);
  }

  out << QString("!%1.beginData %2\n").arg(getName()).arg(m_bufferAvailable);
  out.flush();
  out.device()->write((const char*)m_buffer, m_bufferAvailable);
  out << "\n";
}

void GraphWidgetPluginInstance::runCommand(InputReaderThread* inputReaderThread, const QString& functionName, QStringList args) {
  if(functionName == "addSignal") {
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
  } else if(functionName == "data") {
    if(args.length() == m_signals.length()) {
      addData(args);
    } else {
      qWarning() << "Graph: data: Invalid number of arguments. Expected" << m_signals.length() << ", found " << args.length();
    }
  } else if(functionName == "beginData") {
    if(args.length() == 1) {
      beginData(inputReaderThread, args.at(0).toInt());
    } else {
      qWarning() << "Graph: beginData: Invalid number of arguments. Expected 1, found " << args.length();
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

void GraphWidgetPluginInstance::beginData(InputReaderThread* inputReaderThread, int numberOfBytes) {
  while(numberOfBytes > 0) {
    qint64 read = qMin(numberOfBytes, m_bufferSize - m_bufferWritePos);
    read = inputReaderThread->read(&m_buffer[m_bufferWritePos], read);
    incrementBufferWritePos(read);
    numberOfBytes -= read;
  }
}

void GraphWidgetPluginInstance::addData(QStringList args) {
  unsigned char temp = 0;
  int tempBit = 0;
  for(int i=0; i < qMin(args.length(), m_signals.length()); i++) {
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

QWidget* GraphWidgetPluginInstance::getWidget() {
  if(m_widget == NULL) {
    m_widget = new GraphWidget(this);
    connect(this, SIGNAL(dataAdded()), m_widget, SLOT(updateData()));
  }
  return m_widget;
}

int GraphWidgetPluginInstance::getBytesPerSample() const {
  int bits = 0;
  foreach(GraphSignal* signal, m_signals) {
    bits += signal->bits;
  }
  return ceil((float)bits / 8.0);
}

double GraphWidgetPluginInstance::getValue(int sample, int signalNumber) {
  int totalNumberOfSamples = m_bufferSize / getBytesPerSample();
  while(sample < 0) {
    sample += totalNumberOfSamples;
  }
  while(sample >= totalNumberOfSamples) {
    sample -= totalNumberOfSamples;
  }

  int bufferIndex = getBytesPerSample() * sample;
  unsigned char bufferTemp = getBuffer()[bufferIndex];
  int bufferTempBit = 0;

  uint temp = 0;
  const GraphSignal* signal = NULL;
  for(int s = 0; s <= signalNumber; s++) {
    signal = getSignal(s);
    temp = 0;
    for(int signalBit=0; signalBit < signal->bits; signalBit++) {
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

int GraphWidgetPluginInstance::findSample(int startingSample, int signalNumber, int direction, sampleCompareFn fn) {
  double startingValue = getValue(startingSample, signalNumber);
  int count;
  int sample;
  double previousValue = startingValue;
  for(count=0, sample = startingSample + direction; count < m_bufferAvailable; count++, sample += direction) {
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
