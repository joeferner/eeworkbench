#include "GraphWidget.h"
#include <QPainter>
#include <QRect>

GraphWidget::GraphWidget(GraphWidgetPluginInstance* graphWidgetPluginInstance) :
  QWidget(NULL),
  m_graphWidgetPluginInstance(graphWidgetPluginInstance)
{
}

void GraphWidget::paintEvent(QPaintEvent*)
{
  QPainter painter;
  painter.begin(this);

  QRect totalRect(0, 0, width(), height());
  painter.fillRect(totalRect, Qt::black);

  QBrush brush(QColor(255, 0, 0));
  QPen pen(brush, 1, Qt::SolidLine);
  painter.setPen(pen);
  painter.setBrush(brush);

  const unsigned char* buffer = m_graphWidgetPluginInstance->getBuffer();
  int bufferAvailable = m_graphWidgetPluginInstance->getBufferAvailable();
  int bufferSize = m_graphWidgetPluginInstance->getBufferSize();
  int signalCount = m_graphWidgetPluginInstance->getSignalCount();

  if(signalCount == 0) {
    return;
  }

  for(int s = 0; s < signalCount; s++) {
    const GraphSignal* signal = m_graphWidgetPluginInstance->getSignal(s);
    QRect rect(s * 50, 0, s * 50 + 50, 12);
    painter.drawText(rect, signal->name, QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
  }

  int y = 0;
  int bufferIndex = 0;
  for(int bufferCount = 0; bufferCount < bufferAvailable;) {
    unsigned char bufferTemp = buffer[bufferIndex];
    int bufferTempBit = 0;
    for(int s = 0; s < signalCount; s++) {
      const GraphSignal* signal = m_graphWidgetPluginInstance->getSignal(s);
      uint temp = 0;

      for(int signalBit=0; signalBit < signal->bits; signalBit++) {
        temp = (temp << 1) | (bufferTemp & 0x80 ? 0x01 : 0x00);
        bufferTemp = bufferTemp << 1;
        bufferTempBit++;
        if(bufferTempBit == 8) {
          bufferIndex++;
          bufferCount++;
          if(bufferIndex == bufferSize) {
            bufferIndex = 0;
          }
          bufferTempBit = 0;
          bufferTemp = buffer[bufferIndex];
        }
      }

      QString str = QString::number(temp);
      QRect rect(s * 50, y * 12 + 12, s * 50 + 50, y * 12 + 12);
      painter.drawText(rect, str, QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
    }

    if(bufferTempBit != 0) {
      bufferIndex++;
      bufferCount++;
      if(bufferIndex == bufferSize) {
        bufferIndex = 0;
      }
      bufferTempBit = 0;
      bufferTemp = buffer[bufferIndex];
    }

    y++;
  }

  painter.end();
}
