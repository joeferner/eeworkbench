#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QAbstractScrollArea>

class GraphWidgetPluginInstance;

class GraphWidget : public QAbstractScrollArea
{
  Q_OBJECT
public:
  explicit GraphWidget(GraphWidgetPluginInstance* graphWidgetPluginInstance);

signals:

public slots:

protected:
  virtual void paintEvent(QPaintEvent *);

private:
  double m_pixelsPerSample;
  int m_marginLeft;
  int m_marginTop;
  GraphWidgetPluginInstance* m_graphWidgetPluginInstance;

  void updateHorizontalScrollBar();
  void paintScale(QPainter& painter);
  void paintSignals(QPainter& painter);

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
