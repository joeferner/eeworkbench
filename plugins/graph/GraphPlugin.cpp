#include "GraphPlugin.h"
#include "GraphWidget.h"
#include <QDebug>

GraphPlugin::GraphPlugin()
{
}

QWidget* GraphPlugin::create() const {
  return new GraphWidget();
}

void GraphPlugin::runCommand(QWidget* scope, const QString& functionName, QStringList args) const {
  GraphWidget* graphWidget = (GraphWidget*)scope;
  qDebug() << functionName << args;
}
