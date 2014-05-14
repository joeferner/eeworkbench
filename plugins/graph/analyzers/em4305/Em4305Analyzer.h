#ifndef EM4305ANALYZER_H
#define EM4305ANALYZER_H

#include "../../GraphAnalyzer.h"

class Em4305Analyzer : public GraphAnalyzer {
public:
  Em4305Analyzer();

  virtual QString getName() { return "EM4305"; }
  virtual GraphAnalyzerInstance* configure(GraphWidget* graphWidget, GraphWidgetPluginInstance* graphWidgetPluginInstance);
  virtual GraphAnalyzerInstance* create(GraphWidget* graphWidget, GraphWidgetPluginInstance* graphWidgetPluginInstance, const QString& config);
};

#endif // EM4305ANALYZER_H
