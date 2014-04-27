#include <QPainter>
#include <QRect>
#include "Graph.h"

Graph::Graph(QWidget *parent) :
  QWidget(parent)
{
}

void Graph::paintEvent(QPaintEvent*)
{
  QPainter painter;
  painter.begin(this);

  QRect totalRect(0, 0, width(), height());

  QPen pen(QBrush(QColor(255, 0, 0)), 1, Qt::SolidLine);
  painter.setPen(pen);
  painter.drawRect(totalRect.left(), totalRect.top(), totalRect.right(), totalRect.bottom());

  painter.end();
}
