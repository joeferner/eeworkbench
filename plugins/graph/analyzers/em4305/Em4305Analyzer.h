#ifndef EM4305ANALYZER_H
#define EM4305ANALYZER_H

#include "../../GraphAnalyzer.h"

class Em4305Analyzer : public GraphAnalyzer {
public:
  Em4305Analyzer();

  virtual QString getName() { return "EM4305"; }
  virtual GraphAnalyzerInstance* configure(GraphWidget* graphWidget, GraphWidgetPluginInstance* graphWidgetPluginInstance);
};

#endif // EM4305ANALYZER_H
