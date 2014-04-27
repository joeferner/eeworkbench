#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>

class GraphWidget : public QWidget
{
  Q_OBJECT
public:
  explicit GraphWidget(QWidget *parent = 0);

signals:

public slots:

protected:
  virtual void paintEvent(QPaintEvent *);
};

#endif // GRAPHWIDGET_H
