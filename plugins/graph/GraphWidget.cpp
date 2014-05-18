#include "GraphWidget.h"
#include <QPainter>
#include <QRect>
#include <QScrollBar>
#include <QDebug>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMenu>
#include <math.h>
#include "../../utils/UnitsUtil.h"
#include "GraphAnalyzer.h"
#include "GraphAnalyzerInstance.h"
#include "GraphMark.h"

GraphWidget::GraphWidget(GraphWidgetPluginInstance* graphWidgetPluginInstance) :
  QAbstractScrollArea(NULL),
  m_pixelsPerSample(25.0f),
  m_marginLeft(0),
  m_marginRight(100),
  m_lastMouseSignal(-1),
  m_lastMouseSample(-1),
  m_graphWidgetPluginInstance(graphWidgetPluginInstance),
  m_signalMarginTop(20),
  m_signalMarginBottom(5) {
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
  if(m_mousePosition.x() > m_marginLeft) {
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

void GraphWidget::wheelEvent(QWheelEvent* event) {
  if(event->modifiers() & Qt::ControlModifier) {
    m_pixelsPerSample += event->delta() / 120.0;
    setPixelsPerSample(m_pixelsPerSample);
  } else {
    int v = horizontalScrollBar()->value() - (event->delta() / 5.0);
    horizontalScrollBar()->setValue(v);
    viewport()->update();
  }
}

void GraphWidget::setPixelsPerSample(double pixelsPerSample) {
  if(pixelsPerSample < 1) {
    pixelsPerSample = 1;
  }
  m_pixelsPerSample = pixelsPerSample;
  viewport()->update();
}

void GraphWidget::mousePressEvent(QMouseEvent* event) {
  if(m_addAnalyzerRect.contains(event->pos())) {
    onAddAnalyzerPressed();
  }
}

void GraphWidget::updateData() {
  viewport()->update();
}

void GraphWidget::paintEvent(QPaintEvent*) {
  QPainter painter;
  painter.begin(viewport());

  QRect totalRect(0, 0, width(), height());
  painter.fillRect(totalRect, Qt::black);

  m_marginLeft = 75;

  updateHorizontalScrollBar();
  paintScale(painter);
  paintAnalyzers(painter);
  paintAnalyzerMarks(painter);
  paintSignals(painter);
  paintMeasurements(painter);

  painter.end();
}

void GraphWidget::paintSignals(QPainter& painter) {
  const unsigned char* buffer = m_graphWidgetPluginInstance->getBuffer();
  int bufferSize = m_graphWidgetPluginInstance->getBufferSize();
  const QList<GraphSignal*> graphSignals = m_graphWidgetPluginInstance->getSignals();
  int signalCount = graphSignals.length();
  int bufferAvailable = m_graphWidgetPluginInstance->getBufferAvailable();
  double timePerSample = m_graphWidgetPluginInstance->getTimePerSample();
  int bytesPerSample = m_graphWidgetPluginInstance->getBytesPerSample();

  if(signalCount == 0) {
    return;
  }

  int signalHeight = ((viewport()->height() - m_marginTop - m_marginBottom) / signalCount) - (m_signalMarginTop + m_signalMarginBottom);
  int signalTop, s;

  for(s = 0, signalTop = m_marginTop + m_signalMarginTop; s < signalCount; s++, signalTop += signalHeight + (m_signalMarginTop + m_signalMarginBottom)) {
    m_signalRects[s].setRect(m_marginLeft, signalTop, viewport()->width() - m_marginLeft - m_marginRight, signalHeight);
  }
  m_signalRects[s].setRect(-1, -1, 0, 0);

  // print signal labels
  for(s = 0, signalTop = m_marginTop + m_signalMarginTop; s < signalCount; s++, signalTop += signalHeight + (m_signalMarginTop + m_signalMarginBottom)) {
    const GraphSignal* signal = graphSignals.at(s);
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
    for(s = 0, signalTop = m_marginTop + m_signalMarginTop; s < signalCount; s++, signalTop += signalHeight + (m_signalMarginTop + m_signalMarginBottom)) {
      const GraphSignal* signal = graphSignals.at(s);
      QBrush brush(signal->color);
      QPen pen(brush, 1, Qt::SolidLine);
      painter.setPen(pen);
      painter.setBrush(brush);
      uint temp = 0;

      for(int signalBit = 0; signalBit < signal->bits; signalBit++) {
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
  QString widthHString = "-";
  QString widthLString = "-";
  QString dutyCycleString = "-";

  if(m_lastMouseSignal != -1) {
    const GraphSignal* signal = m_graphWidgetPluginInstance->getSignals().at(m_lastMouseSignal);
    if(signal != NULL) {
      channel = signal->name;
    }
  }

  if(m_lastMouseSample != -1) {
    double timePerSample = m_graphWidgetPluginInstance->getTimePerSample();
    int sample = (int)floor(m_lastMouseSample);

    sampleString = QString::number(sample);
    time = UnitsUtil::toString((float)sample * timePerSample, "s");

    if(m_lastMouseSignal != -1) {
      double value = m_graphWidgetPluginInstance->getValue(sample, m_lastMouseSignal);
      valueString = UnitsUtil::toString(value, "V");

      // find the beginning of this value
      int sample0 = m_graphWidgetPluginInstance->findSample(m_lastMouseSample, m_lastMouseSignal, -1, GraphWidgetPluginInstance::sampleCompareNotEqual);

      bool rising = m_graphWidgetPluginInstance->getValue(sample0, m_lastMouseSignal) > m_graphWidgetPluginInstance->getValue(sample0 - 1, m_lastMouseSignal);

      // find the next crossing point.
      int sample1 = m_graphWidgetPluginInstance->findSample(
                      m_lastMouseSample,
                      m_lastMouseSignal,
                      1,
                      rising ? GraphWidgetPluginInstance::sampleCompareFallingThrough : GraphWidgetPluginInstance::sampleCompareRisingThrough);

      // find the next equal point
      int sample2 = m_graphWidgetPluginInstance->findSample(
                      sample1,
                      m_lastMouseSignal,
                      1,
                      rising ? GraphWidgetPluginInstance::sampleCompareRisingThrough : GraphWidgetPluginInstance::sampleCompareFallingThrough);

      if(sample2 > sample0) {
        double period = (sample2 - sample0) * m_graphWidgetPluginInstance->getTimePerSample();

        periodString = UnitsUtil::toString(period, "s");
        frequencyString = UnitsUtil::toString(1.0 / period, "Hz");

        double widthH, widthL;
        if(rising) {
          widthH = (sample1 - sample0) * m_graphWidgetPluginInstance->getTimePerSample();
          widthL = (sample2 - sample1) * m_graphWidgetPluginInstance->getTimePerSample();
        } else {
          widthL = (sample1 - sample0) * m_graphWidgetPluginInstance->getTimePerSample();
          widthH = (sample2 - sample1) * m_graphWidgetPluginInstance->getTimePerSample();
        }
        widthHString = UnitsUtil::toString(widthH, "s");
        widthLString = UnitsUtil::toString(widthL, "s");

        double dutyCycle = fabs(widthH / (widthH + widthL));
        dutyCycleString = QString::number(dutyCycle * 100.0, 'f', 1) + "%";

        QBrush brush(QColor(255, 255, 255));
        QPen pen(brush, 1, Qt::SolidLine);
        painter.setPen(pen);
        painter.setBrush(brush);

        int y = (m_signalRects[m_lastMouseSignal].top() + m_signalRects[m_lastMouseSignal].bottom()) / 2;
        int x0 = sampleToX(sample0);
        int x1 = sampleToX(sample1);
        int x2 = sampleToX(sample2);
        int arrowSize = 5;

        pen.setColor(QColor(255, 255, 255));
        painter.setPen(pen);
        painter.drawLine(x0, y, x1, y);
        painter.drawLine(x0, y, x0 + arrowSize, y - arrowSize);
        painter.drawLine(x0, y, x0 + arrowSize, y + arrowSize);
        painter.drawLine(x1, y, x1 - arrowSize, y - arrowSize);
        painter.drawLine(x1, y, x1 - arrowSize, y + arrowSize);

        pen.setColor(QColor(255, 255, 255));
        painter.setPen(pen);
        painter.drawLine(x1, y, x2, y);
        painter.drawLine(x1, y, x1 + arrowSize, y - arrowSize);
        painter.drawLine(x1, y, x1 + arrowSize, y + arrowSize);
        painter.drawLine(x2, y, x2 - arrowSize, y - arrowSize);
        painter.drawLine(x2, y, x2 - arrowSize, y + arrowSize);
      }
    }
  }

  QBrush brush(QColor(255, 233, 127));
  QPen pen(brush, 1, Qt::SolidLine);
  painter.setPen(pen);
  painter.setBrush(brush);

  QRect rect(viewport()->width() - m_marginRight, 0, m_marginRight, viewport()->height());
  paintMeasurementField(painter, rect, "Sampel Rate:", UnitsUtil::toString(m_graphWidgetPluginInstance->getSampleRate(), "Hz"));
  paintMeasurementField(painter, rect, "Channel:", channel);
  paintMeasurementField(painter, rect, "Sample:", sampleString);
  paintMeasurementField(painter, rect, "Time:", time);
  paintMeasurementField(painter, rect, "Value:", valueString);
  paintMeasurementField(painter, rect, "Period:", periodString);
  paintMeasurementField(painter, rect, "Frequency:", frequencyString);
  paintMeasurementField(painter, rect, "Width (H):", widthHString);
  paintMeasurementField(painter, rect, "Width (L):", widthLString);
  paintMeasurementField(painter, rect, "Duty Cycle:", dutyCycleString);

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
  if(scrollableWidth < 0) {
    scrollableWidth = 0;
  }
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

void GraphWidget::paintAnalyzerMarks(QPainter& painter) {
  QBrush brush(QColor(150, 50, 50));
  QPen penRect(QBrush(QColor(175, 75, 75)), 1, Qt::SolidLine);
  QPen penText(QBrush(QColor(255, 255, 255)), 1, Qt::SolidLine);
  painter.setBrush(brush);
  painter.setClipping(true);

  QRect clipRect(m_marginLeft, m_marginTop, viewport()->width() - m_marginRight - m_marginLeft, viewport()->height() - m_marginBottom - m_marginTop);
  painter.setClipRect(clipRect);

  foreach(GraphAnalyzerInstance * graphAnalyzerInstance, m_graphWidgetPluginInstance->getAnalyzerInstances()) {
    foreach(GraphMark * mark, graphAnalyzerInstance->getMarks()) {
      int xStart = sampleToX(mark->getStartIndex());
      int xEnd = sampleToX(mark->getEndIndex());
      QRect rect = m_signalRects[mark->getSignalIndex()].translated(0, -m_signalMarginTop - 2);
      rect.setHeight(m_signalMarginTop - 2);
      rect.setLeft(xStart);
      rect.setRight(xEnd);
      painter.setPen(penRect);
      painter.drawRoundedRect(rect, rect.height() / 2, rect.height() / 2);
      painter.setPen(penText);
      painter.drawText(rect, Qt::AlignCenter, mark->getTitle());
    }
  }

  painter.setClipping(false);
}

void GraphWidget::paintAnalyzers(QPainter& painter) {
  QBrush brush(QColor(50, 50, 50));
  QPen pen(QBrush(QColor(255, 255, 255)), 1, Qt::SolidLine);
  painter.setPen(pen);
  painter.setBrush(brush);

  QFontMetrics fm(painter.font());
  QRect fontBoundingBox = fm.boundingRect("i2C Analyzer");

  int margin = 7;
  int height = fontBoundingBox.height();
  height = height + (height % 2);
  m_marginBottom = height + (margin * 2);
  m_analyzerWidth = fontBoundingBox.width();
  int top = viewport()->height() - m_marginBottom;
  int right = viewport()->width() - m_marginRight;

  QRect analyzersRect(m_marginLeft, top, right - m_marginLeft, m_marginBottom);
  painter.setClipRect(analyzersRect);
  painter.setClipping(true);

  // Draw add rect
  m_addAnalyzerRect.setRect(right - height - margin, top + margin, height, height);
  paintAddAnalyzerRect(painter);

  QList<GraphAnalyzerInstance*> graphAnalyzerInstances = m_graphWidgetPluginInstance->getAnalyzerInstances();
  int i = graphAnalyzerInstances.length() - 1;
  foreach(GraphAnalyzerInstance * graphAnalyzerInstance, graphAnalyzerInstances) {
    paintGraphAnalyzerInstance(painter, graphAnalyzerInstance, i);
    i--;
  }

  painter.setClipping(false);
}

void GraphWidget::paintAddAnalyzerRect(QPainter& painter) {
  QRectF rect = m_addAnalyzerRect.translated(0.5, 0.5);

  int centerX = rect.left() + (rect.width() / 2.0);
  int centerY = rect.top() + (rect.height() / 2.0);

  painter.drawRoundedRect(rect, 2, 2);
  painter.drawLine(rect.left() + 4, centerY, rect.right() - 4, centerY);
  painter.drawLine(centerX, rect.top() + 4, centerX, rect.bottom() - 4);
}

void GraphWidget::paintGraphAnalyzerInstance(QPainter& painter, GraphAnalyzerInstance* graphAnalyzerInstance, int index) {
  int left = m_addAnalyzerRect.left() - ((index + 1) * (m_analyzerWidth + 10));
  QRectF rect(left, m_addAnalyzerRect.top(), m_analyzerWidth, m_addAnalyzerRect.height());
  rect = rect.translated(0.5, 0.5);
  graphAnalyzerInstance->setRect(rect);

  painter.drawRoundedRect(rect, 2, 2);
  painter.drawText(rect, graphAnalyzerInstance->getName());
}

void GraphWidget::onAddAnalyzerPressed() {
  QPoint globalPos = mapToGlobal(m_mousePosition);

  QMenu myMenu;
  foreach(GraphAnalyzer * graphAnalyzer, m_graphWidgetPluginInstance->getAnalyzers()) {
    myMenu.addAction(graphAnalyzer->getName());
  }
  QAction* selectedItem = myMenu.exec(globalPos);
  if(selectedItem) {
    foreach(GraphAnalyzer * graphAnalyzer, m_graphWidgetPluginInstance->getAnalyzers()) {
      if(graphAnalyzer->getName() == selectedItem->text()) {
        m_graphWidgetPluginInstance->addAnalyzer(graphAnalyzer);
      }
    }
  }
}

void GraphWidget::refreshAnalyzers() {
  foreach(GraphAnalyzerInstance * graphAnalyzerInstance, m_graphWidgetPluginInstance->getAnalyzerInstances()) {
    graphAnalyzerInstance->refresh(m_graphWidgetPluginInstance);
  }
  viewport()->update();
}

