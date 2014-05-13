#ifndef LABELPLUGIN_H
#define LABELPLUGIN_H

#include "../WidgetPlugin.h"

class LabelPlugin : public WidgetPlugin {
  Q_OBJECT

public:
  LabelPlugin();

  virtual QString getName() const { return "label"; }
  virtual WidgetPluginInstance* createInstance(const QString& name) const;
};

#endif // LABELPLUGIN_H
