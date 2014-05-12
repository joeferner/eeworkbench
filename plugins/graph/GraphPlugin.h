#ifndef GRAPHPLUGIN_H
#define GRAPHPLUGIN_H

#include "../WidgetPlugin.h"

class GraphPlugin : public WidgetPlugin {
  Q_OBJECT

public:
  GraphPlugin();

  virtual QString getName() const { return "graph"; }
  virtual WidgetPluginInstance* createInstance(const QString& name) const;
};

#endif // GRAPHPLUGIN_H
