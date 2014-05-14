#ifndef GRAPHANALYZER_H
#define GRAPHANALYZER_H

#include <QString>

class GraphWidget;
class GraphWidgetPluginInstance;
class GraphAnalyzerInstance;

class GraphAnalyzer {
public:
  GraphAnalyzer();

  virtual QString getName() = 0;
  virtual GraphAnalyzerInstance* configure(GraphWidget* graphWidget, GraphWidgetPluginInstance* graphWidgetPluginInstance) = 0;
};

#endif // GRAPHANALYZER_H
