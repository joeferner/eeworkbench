#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QAbstractScrollArea>
#include <QRect>
#include <QPoint>

class GraphWidgetPluginInstance;

class GraphWidget : public QAbstractScrollArea
{
  Q_OBJECT
public:
  explicit GraphWidget(GraphWidgetPluginInstance* graphWidgetPluginInstance);

signals:

public slots:

protected:
  virtual void paintEvent(QPaintEvent* painter);
  virtual void mouseMoveEvent(QMouseEvent* event);
  virtual void wheelEvent(QWheelEvent* event);

private:
  double m_pixelsPerSample;
  int m_marginLeft;
  int m_marginRight;
  int m_marginTop;
  int m_lastMouseSignal;
  double m_lastMouseSample;
  QPoint m_mousePosition;
  QRect m_measurementHeadingTextBoundingRect;
  QRect m_measurementValueTextBoundingRect;
  QRect m_signalRects[100];
  GraphWidgetPluginInstance* m_graphWidgetPluginInstance;

  void updateHorizontalScrollBar();
  void paintScale(QPainter& painter);
  void paintSignals(QPainter& painter);
  void paintMeasurements(QPainter& painter);
  void paintMeasurementField(QPainter& painter, QRect& rect, const QString& title, const QString& value);

  int getScrollPosition() const;
  double xPositionToSample(double x) const;
  double xPositionToTime(double x) const;
  double widthToSamples(double width) const;
  double widthToTime(double width) const;
  double timeToX(double time) const;
  double sampleToX(double sample) const;
  double timeToWidth(double time) const;
  double sampleToWidth(double sample) const;
};

#include "GraphWidgetPluginInstance.h"

#endif // GRAPHWIDGET_H
