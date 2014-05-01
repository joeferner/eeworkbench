#include "GraphWidget.h"
#include <QPainter>
#include <QRect>
#include <QScrollBar>
#include <QDebug>
#include <QMouseEvent>
#include <math.h>
#include "../../utils/UnitsUtil.h"

GraphWidget::GraphWidget(GraphWidgetPluginInstance* graphWidgetPluginInstance) :
  QAbstractScrollArea(NULL),
  m_pixelsPerSample(25.0f),
  m_marginLeft(0),
  m_marginRight(100),
  m_lastMouseSignal(-1),
  m_lastMouseSample(-1),
  m_graphWidgetPluginInstance(graphWidgetPluginInstance)
{
  m_signalRects[0].setRect(-1, -1, 0, 0);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
  setMouseTracking(true);
  setUpdatesEnabled(true);
}

void GraphWidget::mouseMoveEvent(QMouseEvent* event) {
  bool requireRepaint = false;
  QAbstractScrollArea::mouseMoveEvent(event);

  m_mousePosition = event->pos();

  int signal = -1;
  for(int s = 0; m_signalRects[s].left() != -1; s++) {
    if(m_signalRects[s].contains(m_mousePosition)) {
      signal = s;
      break;
    }
  }
  if(m_lastMouseSignal != signal) {
    m_lastMouseSignal = signal;
    requireRepaint = true;
  }

  double sample = -1;
  if(m_mousePosition.x() > m_marginLeft && m_mousePosition.y() > m_marginTop) {
    sample = xPositionToSample(m_mousePosition.x());
  }
  if(m_lastMouseSample != sample) {
    m_lastMouseSample = sample;
    requireRepaint = true;
  }

  if(requireRepaint) {
    viewport()->update();
  }
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
  paintMeasurements(painter);

  painter.end();
}

void GraphWidget::paintSignals(QPainter& painter) {
  const unsigned char* buffer = m_graphWidgetPluginInstance->getBuffer();
  int bufferSize = m_graphWidgetPluginInstance->getBufferSize();
  int signalCount = m_graphWidgetPluginInstance->getSignalCount();
  int bufferAvailable = m_graphWidgetPluginInstance->getBufferAvailable();
  double timePerSample = m_graphWidgetPluginInstance->getTimePerSample();
  int bytesPerSample = m_graphWidgetPluginInstance->getBytesPerSample();

  if(signalCount == 0) {
    return;
  }

  int signalMargin = 10;
  int signalHeight = ((viewport()->height() - m_marginTop) / signalCount) - (2 * signalMargin);
  int signalTop, s;

  for(s = 0, signalTop = m_marginTop + signalMargin; s < signalCount; s++, signalTop += signalHeight + (2 * signalMargin)) {
    m_signalRects[s].setRect(m_marginLeft, signalTop, viewport()->width() - m_marginLeft - m_marginRight, signalHeight);
  }
  m_signalRects[s].setRect(-1, -1, 0, 0);

  // print signal labels
  for(s = 0, signalTop = m_marginTop + signalMargin; s < signalCount; s++, signalTop += signalHeight + (2 * signalMargin)) {
    const GraphSignal* signal = m_graphWidgetPluginInstance->getSignal(s);
    QRect rect(0, signalTop, m_marginLeft, signalHeight);
    painter.drawText(rect, signal->name, QTextOption(Qt::AlignRight | Qt::AlignVCenter));
  }

  QRect clippingRect(m_marginLeft, m_marginTop, viewport()->width() - m_marginLeft - m_marginRight, viewport()->height() - m_marginTop);
  painter.setClipRect(clippingRect);
  painter.setClipping(true);

  QPoint lastPoints[100];
  int bufferIndex = 0;
  for(int bufferCount = 0; bufferCount < bufferAvailable;) {
    unsigned char bufferTemp = buffer[bufferIndex];
    int bufferTempBit = 0;
    int x = timeToX((bufferCount / bytesPerSample) * timePerSample);
    for(s = 0, signalTop = m_marginTop + signalMargin; s < signalCount; s++, signalTop += signalHeight + (2 * signalMargin)) {
      const GraphSignal* signal = m_graphWidgetPluginInstance->getSignal(s);
      QBrush brush(signal->color);
      QPen pen(brush, 1, Qt::SolidLine);
      painter.setPen(pen);
      painter.setBrush(brush);
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

      double f = (double)temp / (double)(signal->maxValue);
      int y = signalTop + (signalHeight - (signalHeight * f));

      QPoint pt(x, y);
      if(bufferCount > 0) {
        if(signal->bits == 1) {
          int midX = pt.x() - 1;
          painter.drawLine(lastPoints[s].x(), lastPoints[s].y(), midX, lastPoints[s].y());
          painter.drawLine(midX, lastPoints[s].y(), midX, pt.y());
          painter.drawLine(midX, pt.y(), pt.x(), pt.y());
        } else {
          painter.drawLine(lastPoints[s], pt);
        }
      }

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

void GraphWidget::paintMeasurements(QPainter& painter) {
  QRect clippingRect(0, 0, viewport()->width(), viewport()->height());
  painter.setClipRect(clippingRect);
  painter.setClipping(true);

  QFontMetrics fm(painter.font());
  m_measurementHeadingTextBoundingRect = fm.boundingRect("Channel:");
  m_measurementValueTextBoundingRect = fm.boundingRect("0.00000MHz");
  m_measurementValueTextBoundingRect.setHeight(m_measurementValueTextBoundingRect.height() + 10);

  QString channel = "-";
  QString sampleString = "-";
  QString time = "-";
  QString valueString = "-";
  QString periodString = "-";
  QString frequencyString = "-";
  QString widthH = "-";
  QString widthL = "-";
  QString dutyCycle = "-";

  if(m_lastMouseSignal != -1) {
    const GraphSignal* signal = m_graphWidgetPluginInstance->getSignal(m_lastMouseSignal);
    channel = signal->name;
  }

  if(m_lastMouseSample != -1) {
    double timePerSample = m_graphWidgetPluginInstance->getTimePerSample();
    int sample = (int)floor(m_lastMouseSample);

    sampleString = QString::number(sample);
    time = UnitsUtil::toString((float)sample * timePerSample, "s");

    if(m_lastMouseSignal != -1) {
      double value = m_graphWidgetPluginInstance->getValue(sample, m_lastMouseSignal);
      valueString = UnitsUtil::toString(value, "V");

      // find the next value that doesn't equal the current value to determine if the signal is rising for falling
      m_lastMouseSampleRising = true;
      int i;
      for(i = (int)(ceil(m_lastMouseSample) + 0.1); i<m_graphWidgetPluginInstance->getBufferAvailable(); i++) {
        double v = m_graphWidgetPluginInstance->getValue(i, m_lastMouseSignal);
        if(v > value) {
          m_lastMouseSampleRising = true;
          break;
        } else if(v < value) {
          m_lastMouseSampleRising = false;
        }
      }

      // find the 1st crossing point before the point
      double crossing0Value = 0;
      for(i = (int)(ceil(m_lastMouseSample) + 0.1); i >= 0; i--) {
        crossing0Value = m_graphWidgetPluginInstance->getValue(i, m_lastMouseSignal);
        if((!m_lastMouseSampleRising && crossing0Value <= value) || (m_lastMouseSampleRising && crossing0Value >= value)) {
          m_lastMouseSampleCrossing0 = i;
          break;
        }
      }

      // find the 1st crossing point
      double crossing1Value = 0;
      for(i = (int)(ceil(m_lastMouseSample) + 0.1); i < m_graphWidgetPluginInstance->getBufferAvailable(); i++) {
        crossing1Value = m_graphWidgetPluginInstance->getValue(i, m_lastMouseSignal);
        if((m_lastMouseSampleRising && crossing1Value <= value) || (!m_lastMouseSampleRising && crossing1Value >= value)) {
          m_lastMouseSampleCrossing1 = i;
          break;
        }
      }

      // find the 2st crossing point
      double crossing2Value = 0;
      for(; i < m_graphWidgetPluginInstance->getBufferAvailable(); i++) {
        crossing2Value = m_graphWidgetPluginInstance->getValue(i, m_lastMouseSignal);
        if((!m_lastMouseSampleRising && crossing2Value <= value) || (m_lastMouseSampleRising && crossing2Value >= value)) {
          m_lastMouseSampleCrossing2 = i;
          break;
        }
      }

      double period = (m_lastMouseSampleCrossing2 - m_lastMouseSampleCrossing0) * m_graphWidgetPluginInstance->getTimePerSample();

      periodString = UnitsUtil::toString(period, "s");
      frequencyString = UnitsUtil::toString(1.0 / period, "Hz");
      if(m_lastMouseSampleRising) {
        widthH = UnitsUtil::toString((m_lastMouseSampleCrossing1 - m_lastMouseSampleCrossing0) * m_graphWidgetPluginInstance->getTimePerSample(), "s");
        widthL = UnitsUtil::toString((m_lastMouseSampleCrossing2 - m_lastMouseSampleCrossing1) * m_graphWidgetPluginInstance->getTimePerSample(), "s");
      } else {
        widthL = UnitsUtil::toString((m_lastMouseSampleCrossing1 - m_lastMouseSampleCrossing0) * m_graphWidgetPluginInstance->getTimePerSample(), "s");
        widthH = UnitsUtil::toString((m_lastMouseSampleCrossing2 - m_lastMouseSampleCrossing1) * m_graphWidgetPluginInstance->getTimePerSample(), "s");
      }

      QBrush brush(QColor(255,255,255));
      QPen pen(brush, 1, Qt::SolidLine);
      painter.setPen(pen);
      painter.setBrush(brush);

      int y1 = 0;
      int y2 = viewport()->height();

      qDebug() << m_lastMouseSampleCrossing0 << "," << m_lastMouseSampleCrossing1 << "," << m_lastMouseSampleCrossing2;
      int x = sampleToX(m_lastMouseSampleCrossing0);
      pen.setColor(QColor(255,0,0));
      painter.setPen(pen);
      painter.drawLine(x, y1, x, y2);

      x = sampleToX(m_lastMouseSampleCrossing1);
      pen.setColor(QColor(0,255,0));
      painter.setPen(pen);
      painter.drawLine(x, y1, x, y2);

      x = sampleToX(m_lastMouseSampleCrossing2);
      pen.setColor(QColor(0,0,255));
      painter.setPen(pen);
      painter.drawLine(x, y1, x, y2);
    }
  }

  QBrush brush(QColor(255, 233, 127));
  QPen pen(brush, 1, Qt::SolidLine);
  painter.setPen(pen);
  painter.setBrush(brush);

  QRect rect(viewport()->width() - m_marginRight, 0, m_marginRight, viewport()->height());
  paintMeasurementField(painter, rect, "Channel:", channel);
  paintMeasurementField(painter, rect, "Sample:", sampleString);
  paintMeasurementField(painter, rect, "Time:", time);
  paintMeasurementField(painter, rect, "Value:", valueString);
  paintMeasurementField(painter, rect, "Period:", periodString);
  paintMeasurementField(painter, rect, "Frequency:", frequencyString);
  paintMeasurementField(painter, rect, "Width (H):", widthH);
  paintMeasurementField(painter, rect, "Width (L):", widthL);
  paintMeasurementField(painter, rect, "Duty Cycle:", dutyCycle);

  painter.setClipping(false);
}

void GraphWidget::paintMeasurementField(QPainter& painter, QRect& rect, const QString& title, const QString& value) {
  painter.drawText(rect, title, QTextOption(Qt::AlignLeft | Qt::AlignTop));
  rect.adjust(10, m_measurementHeadingTextBoundingRect.height(), 0, 0);
  painter.drawText(rect, value, QTextOption(Qt::AlignLeft | Qt::AlignTop));
  rect.adjust(-10, m_measurementValueTextBoundingRect.height(), 0, 0);
}

void GraphWidget::updateHorizontalScrollBar() {
  int bufferAvailable = m_graphWidgetPluginInstance->getBufferAvailable();

  int scrollableWidth = (m_pixelsPerSample * (double)bufferAvailable) - (viewport()->width() - m_marginRight) + m_marginLeft;
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

  QRect scaleRect(m_marginLeft, 0, viewport()->width() - m_marginLeft - m_marginRight, m_marginTop);
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


