#ifndef GRAPHANALYZERINSTANCE_H
#define GRAPHANALYZERINSTANCE_H

#include <QRect>
#include <QString>
#include <QList>

class GraphWidgetPluginInstance;
class GraphMark;

class GraphAnalyzerInstance {
public:
  GraphAnalyzerInstance(const QString& name);

  virtual void refresh(GraphWidgetPluginInstance* graphWidgetPluginInstance) = 0;
  void setRect(const QRectF& rect) { m_rect = rect; }
  QRectF getRect() const { return m_rect; }
  virtual QString getName() const { return m_name; }
  virtual QString getConfig() const = 0;
  void clearMarks();
  void addMark(int startIndex, int endIndex, int signalIndex, const QString& title);

private:
  QString m_name;
  QRectF m_rect;
  QList<GraphMark*> m_marks;
};

#endif // GRAPHANALYZERINSTANCE_H
