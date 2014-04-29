#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>

class GraphWidgetPluginInstance;

class GraphWidget : public QWidget
{
  Q_OBJECT
public:
  explicit GraphWidget(GraphWidgetPluginInstance* graphWidgetPluginInstance);

signals:

public slots:

protected:
  virtual void paintEvent(QPaintEvent *);

private:
  GraphWidgetPluginInstance* m_graphWidgetPluginInstance;
};

#include "GraphWidgetPluginInstance.h"

#endif // GRAPHWIDGET_H
