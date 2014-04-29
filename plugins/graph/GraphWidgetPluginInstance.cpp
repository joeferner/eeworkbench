#include "GraphWidgetPluginInstance.h"
#include "GraphWidget.h"
#include <QDebug>

unsigned char GraphWidgetPluginInstance::MASKS[] = { 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff };

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
    int signalBitsLeft = signal->bits;
    while(signalBitsLeft > 0) {
      int numberOfBitsToCopy = qMin(signalBitsLeft, 8 - tempBit);
      unsigned char bitsToCopy = MASKS[numberOfBitsToCopy] & arg;
      temp |= bitsToCopy << (7 - (tempBit - numberOfBitsToCopy));
      arg = arg >> numberOfBitsToCopy;
      signalBitsLeft -= numberOfBitsToCopy;

      tempBit += numberOfBitsToCopy;
      if(tempBit == 8) {
        m_buffer[m_bufferWritePos] = temp;
        m_bufferAvailable++;
        if(m_bufferAvailable >= m_bufferSize) {
          m_bufferAvailable = m_bufferSize;
        }
        m_bufferWritePos++;
        if(m_bufferWritePos >= m_bufferSize) {
          m_bufferWritePos = 0;
        }
        temp = 0;
        tempBit = 0;
      }
    }
  }

  m_buffer[m_bufferWritePos] = temp;
  m_bufferAvailable++;
  if(m_bufferAvailable >= m_bufferSize) {
    m_bufferAvailable = m_bufferSize;
  }
  m_bufferWritePos++;
  if(m_bufferWritePos >= m_bufferSize) {
    m_bufferWritePos = 0;
  }

  if(m_widget) {
    m_widget->repaint();
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
