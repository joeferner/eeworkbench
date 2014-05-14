#include "GraphMark.h"

GraphMark::GraphMark(int startIndex, int endIndex, int signalIndex, const QString& title) :
  m_startIndex(startIndex),
  m_endIndex(endIndex),
  m_signalIndex(signalIndex),
  m_title(title) {
}
