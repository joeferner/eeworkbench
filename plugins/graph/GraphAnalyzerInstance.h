#ifndef GRAPHANALYZERINSTANCE_H
#define GRAPHANALYZERINSTANCE_H

#include <QRect>
#include <QString>

class GraphAnalyzerInstance
{
public:
  GraphAnalyzerInstance(const QString& name);

  void setRect(const QRectF& rect) { m_rect = rect; }
  QRectF getRect() const { return m_rect; }
  virtual QString getName() const { return m_name; }

private:
  QString m_name;
  QRectF m_rect;
};

#endif // GRAPHANALYZERINSTANCE_H
