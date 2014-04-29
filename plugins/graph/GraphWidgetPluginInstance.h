#ifndef GRAPHWIDGETPLUGININSTANCE_H
#define GRAPHWIDGETPLUGININSTANCE_H

#include "../WidgetPluginInstance.h"
#include "GraphWidget.h"
#include <QList>
#include <QColor>

struct GraphSignal {
  QString name;
  int bits;
  int scaleMin;
  int scaleMax;
  QColor color;
};

class GraphWidgetPluginInstance : public WidgetPluginInstance
{
public:
  GraphWidgetPluginInstance();
  virtual ~GraphWidgetPluginInstance();

  virtual void runCommand(const QString& functionName, QStringList args);
  virtual QWidget* getWidget();
  void addSignal(const QString& name, int bits, float scaleMin, float scaleMax);
  void addData(QStringList args);

  int getSignalCount() const { return m_signals.length(); }
  const GraphSignal* getSignal(int i) const { return m_signals.at(i); }
  const unsigned char* getBuffer() const { return m_buffer; }
  int getBufferSize() const { return m_bufferSize; }
  int getBufferWritePos() const { return m_bufferWritePos; }
  int getBufferAvailable() const { return m_bufferAvailable; }

private:
  GraphWidget* m_widget;
  QList<GraphSignal*> m_signals;
  unsigned char* m_buffer;
  int m_bufferSize;
  int m_bufferWritePos;
  int m_bufferAvailable;
  int m_colorsCount;
  QColor* m_colors;

  void writeByteToBuffer(unsigned char b);
};

#endif // GRAPHWIDGETPLUGININSTANCE_H
