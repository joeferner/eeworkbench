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
  float m_pixelsPerSample;
  int m_marginLeft;
  GraphWidgetPluginInstance* m_graphWidgetPluginInstance;

  void updateHorizontalScrollBar();
  void paintScale(QPainter& painter);
  void paintSignals(QPainter& painter);

  int getScrollPosition() const;
  float xPositionToSample(float x) const;
  float xPositionToTime(float x) const;
  float timeToX(float time) const;
  float sampleToX(float sample) const;
};

#include "GraphWidgetPluginInstance.h"

#endif // GRAPHWIDGET_H
