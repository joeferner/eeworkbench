#ifndef GRAPHPLUGIN_H
#define GRAPHPLUGIN_H

#include "../WidgetPlugin.h"

class GraphPlugin : public WidgetPlugin
{
  Q_OBJECT

public:
  GraphPlugin();

  virtual QWidget* create() const;
  virtual QString getName() const { return "graph"; }
  virtual void runCommand(QWidget* scope, const QString& functionName, QStringList args) const;
};

#endif // GRAPHPLUGIN_H
