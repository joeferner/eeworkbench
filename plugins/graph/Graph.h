#ifndef GRAPH_H
#define GRAPH_H

#include <QWidget>

class Graph : public QWidget
{
  Q_OBJECT
public:
  explicit Graph(QWidget *parent = 0);

signals:

public slots:

protected:
  virtual void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
};

#endif // GRAPH_H
