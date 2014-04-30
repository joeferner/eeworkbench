#include "GraphWidget.h"
#include <QPainter>
#include <QRect>
#include <QScrollBar>
#include <QDebug>
#include "../../utils/UnitsUtil.h"

GraphWidget::GraphWidget(GraphWidgetPluginInstance* graphWidgetPluginInstance) :
  QAbstractScrollArea(NULL),
  m_pixelsPerSample(5.0f),
  m_graphWidgetPluginInstance(graphWidgetPluginInstance)
{
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void GraphWidget::paintEvent(QPaintEvent*)
{
  QPainter painter;
  painter.begin(viewport());

  QRect totalRect(0, 0, width(), height());
  painter.fillRect(totalRect, Qt::black);

  m_marginLeft =100;

  updateHorizontalScrollBar();
  paintScale(painter);
  //paintSignals(painter);

  painter.end();
}

void GraphWidget::paintSignals(QPainter& painter) {
  const unsigned char* buffer = m_graphWidgetPluginInstance->getBuffer();
  int bufferSize = m_graphWidgetPluginInstance->getBufferSize();
  int signalCount = m_graphWidgetPluginInstance->getSignalCount();
  int bufferAvailable = m_graphWidgetPluginInstance->getBufferAvailable();

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
}

void GraphWidget::updateHorizontalScrollBar() {
  int bufferAvailable = m_graphWidgetPluginInstance->getBufferAvailable();

  int scrollableWidth = m_pixelsPerSample * (float)bufferAvailable;
  int currentMax = horizontalScrollBar()->maximum();
  if(scrollableWidth != currentMax) {
    horizontalScrollBar()->setMaximum(scrollableWidth);
  }
}

float GraphWidget::xPositionToSample(float x) const {
  return (horizontalScrollBar()->value() + (x - m_marginLeft)) / m_pixelsPerSample;
}

float GraphWidget::xPositionToTime(float x) const {
  float timePerSample = m_graphWidgetPluginInstance->getTimePerSample();
  return xPositionToSample(x) * timePerSample;
}

float GraphWidget::sampleToX(float sample) const {
  return (sample * m_pixelsPerSample) - horizontalScrollBar()->value() + m_marginLeft;
}

float GraphWidget::timeToX(float time) const {
  float timePerSample = m_graphWidgetPluginInstance->getTimePerSample();
  return sampleToX(time / timePerSample);
}

void GraphWidget::paintScale(QPainter& painter) {
  QBrush brush(QColor(255, 255, 255));
  QPen pen(brush, 1, Qt::SolidLine);
  painter.setPen(pen);
  painter.setBrush(brush);

  QFontMetrics fm(painter.font());

  QRect rect;
  QString str;
  int margin = 20;
  int y = 0;
  int textHeight = 20;
  int maximumTextWidth = fm.width("1.000ms");
  float timePerTick = UnitsUtil::roundToOrderOfMagnitude(xPositionToTime(maximumTextWidth + (2 * margin)) - xPositionToTime(0));
  float pixelsPerTick = timeToX(timePerTick) + horizontalScrollBar()->value() - m_marginLeft;
  float time = UnitsUtil::roundToOrderOfMagnitude(xPositionToTime(m_marginLeft));

  qDebug() << "time" << time;
  for(int x = timeToX(time - timePerTick); x < viewport()->width(); x += pixelsPerTick) {
    rect.setRect(x - (maximumTextWidth / 2), y, maximumTextWidth, textHeight);
    str = UnitsUtil::toString(xPositionToTime(x), "s");
    painter.drawText(rect, Qt::AlignHCenter, str);
  }
}


