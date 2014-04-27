#ifndef PLUGIN_H
#define PLUGIN_H

#include <QObject>

class Plugin : public QObject
{
  Q_OBJECT

public:
  Plugin();
  virtual ~Plugin();
};

#endif // PLUGIN_H
