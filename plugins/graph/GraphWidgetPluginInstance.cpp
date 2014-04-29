#include "GraphWidgetPluginInstance.h"
#include "GraphWidget.h"
#include <QDebug>

GraphWidgetPluginInstance::GraphWidgetPluginInstance() :
  m_widget(NULL)
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
  delete m_widget;
}

void GraphWidgetPluginInstance::runCommand(const QString& functionName, QStringList args) {
  if(functionName == "addSignal") {
    if(args.length() == 4) {
      addSignal(args.at(0), args.at(1).toInt(), args.at(2).toFloat(), args.at(3).toFloat());
    } else {
      qWarning() << "Graph: addSignal: Invalid number of arguments. Expected 4, found " << args.length();
    }
  } else if(functionName == "data") {
    if(args.length() == m_signals.length()) {
      addData(args);
    } else {
      qWarning() << "Graph: data: Invalid number of arguments. Expected" << m_signals.length() << ", found " << args.length();
    }
  } else {
    qWarning() << "Graph: Unknown Command" << functionName << args;
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

  if(m_widget) {
    m_widget->repaint();
  }
}

void GraphWidgetPluginInstance::writeByteToBuffer(unsigned char b) {
  m_buffer[m_bufferWritePos] = b;
  m_bufferAvailable++;
  if(m_bufferAvailable >= m_bufferSize) {
    m_bufferAvailable = m_bufferSize;
  }
  m_bufferWritePos++;
  if(m_bufferWritePos >= m_bufferSize) {
    m_bufferWritePos = 0;
  }
}

void GraphWidgetPluginInstance::addSignal(const QString& name, int bits, float scaleMin, float scaleMax) {
  GraphSignal* signal = new GraphSignal();
  signal->name = name;
  signal->bits = bits;
  signal->scaleMin = scaleMin;
  signal->scaleMax = scaleMax;
  signal->color = m_colors[m_signals.length() % m_colorsCount];
  m_signals.append(signal);
}

QWidget* GraphWidgetPluginInstance::getWidget() {
  if(m_widget == NULL) {
    m_widget = new GraphWidget(this);
  }
  return m_widget;
}
