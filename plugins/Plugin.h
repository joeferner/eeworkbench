#ifndef PLUGIN_H
#define PLUGIN_H

#include <QObject>

class Plugin : public QObject
{
  Q_OBJECT

public:
  Plugin();
  virtual ~Plugin();

  virtual QString getName() const = 0;
};

#endif // PLUGIN_H
