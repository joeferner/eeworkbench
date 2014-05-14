#ifndef GRAPHMARK_H
#define GRAPHMARK_H

#include <QString>

class GraphMark {
public:
  GraphMark(int startIndex, int endIndex, int signalIndex, const QString& title);

  int getStartIndex() const { return m_startIndex; }
  int getEndIndex() const { return m_endIndex; }
  int getSignalIndex() const { return m_signalIndex; }
  QString getTitle() const { return m_title; }

private:
  int m_startIndex;
  int m_endIndex;
  int m_signalIndex;
  QString m_title;
};

#endif // GRAPHMARK_H
