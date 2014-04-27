#include "GraphWidget.h"
#include <QPainter>
#include <QRect>

GraphWidget::GraphWidget(QWidget *parent) :
  QWidget(parent)
{
}

void GraphWidget::paintEvent(QPaintEvent*)
{
  QPainter painter;
  painter.begin(this);

  QRect totalRect(0, 0, width(), height());

  QPen pen(QBrush(QColor(255, 0, 0)), 1, Qt::SolidLine);
  painter.setPen(pen);
  painter.drawRect(totalRect.left(), totalRect.top(), totalRect.right(), totalRect.bottom());

  painter.end();
}
