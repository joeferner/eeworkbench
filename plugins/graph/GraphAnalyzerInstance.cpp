#include "GraphAnalyzerInstance.h"
#include "GraphMark.h"

GraphAnalyzerInstance::GraphAnalyzerInstance(const QString& name) :
  m_name(name) {
}

void GraphAnalyzerInstance::clearMarks() {
  foreach(GraphMark * graphMark, m_marks) {
    delete graphMark;
  }
  m_marks.clear();
}

void GraphAnalyzerInstance::addMark(int startIndex, int endIndex, int signalIndex, const QString& title) {
  m_marks.append(new GraphMark(startIndex, endIndex, signalIndex, title));
}
