#include "GraphWidget.h"
#include <QPainter>
#include <QRect>
#include <QScrollBar>
#include <QDebug>
#include <math.h>
#include "../../utils/UnitsUtil.h"

GraphWidget::GraphWidget(GraphWidgetPluginInstance* graphWidgetPluginInstance) :
  QAbstractScrollArea(NULL),
  m_pixelsPerSample(25.0f),
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

  m_marginLeft = 75;

  updateHorizontalScrollBar();
  paintScale(painter);
  paintSignals(painter);

  painter.end();
}

void GraphWidget::paintSignals(QPainter& painter) {
  const unsigned char* buffer = m_graphWidgetPluginInstance->getBuffer();
  int bufferSize = m_graphWidgetPluginInstance->getBufferSize();
  int signalCount = m_graphWidgetPluginInstance->getSignalCount();
  int bufferAvailable = m_graphWidgetPluginInstance->getBufferAvailable();
  double timePerSample = m_graphWidgetPluginInstance->getTimePerSample();

  if(signalCount == 0) {
    return;
  }

  int signalHeight = (viewport()->height() - m_marginTop) / signalCount;

  int y, s;
  for(s = 0, y = m_marginTop; s < signalCount; s++, y+=signalHeight) {
    const GraphSignal* signal = m_graphWidgetPluginInstance->getSignal(s);
    QRect rect(0, y, m_marginLeft, signalHeight);
    painter.drawText(rect, signal->name, QTextOption(Qt::AlignRight | Qt::AlignVCenter));    
  }

  QRect scaleRect(m_marginLeft, m_marginTop, viewport()->width() - m_marginLeft, viewport()->height() - m_marginTop);
  painter.setClipRect(scaleRect);
  painter.setClipping(true);

  QPoint lastPoints[100];
  int bufferIndex = 0;
  for(int bufferCount = 0; bufferCount < bufferAvailable;) {
    unsigned char bufferTemp = buffer[bufferIndex];
    int bufferTempBit = 0;
    int x = timeToX(bufferCount * timePerSample);
    for(s = 0, y = m_marginTop; s < signalCount; s++, y+=signalHeight) {
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

      QPoint pt(x, y);
      QString str = QString::number(temp);
      QRect rect(x, y, 100, 100);
      painter.drawText(rect, str, QTextOption(Qt::AlignHCenter | Qt::AlignVCenter));
      painter.drawLine(lastPoints[s], pt);

      lastPoints[s].setX(x);
      lastPoints[s].setY(y);
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
  }

  painter.setClipping(false);
}

void GraphWidget::updateHorizontalScrollBar() {
  int bufferAvailable = m_graphWidgetPluginInstance->getBufferAvailable();

  int scrollableWidth = m_pixelsPerSample * (double)bufferAvailable;
  int currentMax = horizontalScrollBar()->maximum();
  if(scrollableWidth != currentMax) {
    horizontalScrollBar()->setMaximum(scrollableWidth);
  }
}

double GraphWidget::xPositionToSample(double x) const {
  return (horizontalScrollBar()->value() + (x - m_marginLeft)) / m_pixelsPerSample;
}

double GraphWidget::xPositionToTime(double x) const {
  double timePerSample = m_graphWidgetPluginInstance->getTimePerSample();
  return xPositionToSample(x) * timePerSample;
}

double GraphWidget::widthToSamples(double width) const {
  return width / m_pixelsPerSample;
}

double GraphWidget::widthToTime(double width) const {
  double timePerSample = m_graphWidgetPluginInstance->getTimePerSample();
  return widthToSamples(width) * timePerSample;
}

double GraphWidget::sampleToX(double sample) const {
  return (sample * m_pixelsPerSample) - horizontalScrollBar()->value() + m_marginLeft;
}

double GraphWidget::timeToX(double time) const {
  double timePerSample = m_graphWidgetPluginInstance->getTimePerSample();
  return sampleToX(time / timePerSample);
}

double GraphWidget::timeToWidth(double time) const {
  double timePerSample = m_graphWidgetPluginInstance->getTimePerSample();
  return sampleToWidth(time / timePerSample);
}

double GraphWidget::sampleToWidth(double sample) const {
  return sample * m_pixelsPerSample;
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
  QRect fontBoundingBox = fm.boundingRect("1.000ms");
  int maximumTextWidth = fontBoundingBox.width();
  m_marginTop = fontBoundingBox.height() + 5;
  double timePerTick = UnitsUtil::roundToOrderOfMagnitude(widthToTime(maximumTextWidth + (2 * margin)));
  double pixelsPerTick = timeToWidth(timePerTick);
  while(pixelsPerTick < maximumTextWidth + (2 * margin)) {
    timePerTick = timePerTick * 2.0;
    pixelsPerTick = timeToWidth(timePerTick);
  }
  while(pixelsPerTick > 2 * (maximumTextWidth + (2 * margin))) {
    timePerTick = timePerTick / 2.0;
    pixelsPerTick = timeToWidth(timePerTick);
  }
  double ticksOnScreen = ceil(viewport()->width() / pixelsPerTick);
  double time = UnitsUtil::roundToOrderOfMagnitude(xPositionToTime(viewport()->width())) - (timePerTick * ticksOnScreen);

  QRect scaleRect(m_marginLeft, 0, viewport()->width() - m_marginLeft, m_marginTop);
  painter.setClipRect(scaleRect);
  painter.setClipping(true);

  for(double x = timeToX(time - timePerTick); x < viewport()->width() + pixelsPerTick; x += pixelsPerTick) {
    if((x + (maximumTextWidth / 2)) < m_marginLeft) {
      continue;
    }
    rect.setRect(x - (maximumTextWidth / 2), y, maximumTextWidth, m_marginTop);
    str = UnitsUtil::toString(xPositionToTime(x), "s");
    painter.drawText(rect, Qt::AlignHCenter, str);
  }

  painter.setClipping(false);
}


