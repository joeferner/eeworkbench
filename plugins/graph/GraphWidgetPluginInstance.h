#ifndef GRAPHWIDGETPLUGININSTANCE_H
#define GRAPHWIDGETPLUGININSTANCE_H

#include "../WidgetPluginInstance.h"
#include "GraphWidget.h"
#include <QList>
#include <QColor>

struct GraphSignal {
  QString name;
  int bits;
  int maxValue;
  int scaleMin;
  int scaleMax;
  QColor color;
};

class GraphWidgetPluginInstance : public WidgetPluginInstance
{
public:
  GraphWidgetPluginInstance();
  virtual ~GraphWidgetPluginInstance();

  virtual void runCommand(InputReaderThread* inputReaderThread, const QString& functionName, QStringList args);
  virtual QWidget* getWidget();
  void addSignal(const QString& name, int bits, double scaleMin, double scaleMax);
  void addData(QStringList args);
  void beginData(InputReaderThread* inputReaderThread, int numberOfBytes);

  int getSignalCount() const { return m_signals.length(); }
  const GraphSignal* getSignal(int i) const { return m_signals.at(i); }
  const unsigned char* getBuffer() const { return m_buffer; }
  int getBufferSize() const { return m_bufferSize; }
  int getBufferWritePos() const { return m_bufferWritePos; }
  int getBufferAvailable() const { return m_bufferAvailable; }
  double getTimePerSample() const { return m_timePerSample; }
  int getBytesPerSample() const;
  double getValue(int sample, int signal);

private:
  GraphWidget* m_widget;
  QList<GraphSignal*> m_signals;
  double m_timePerSample;
  unsigned char* m_buffer;
  int m_bufferSize;
  int m_bufferWritePos;
  int m_bufferAvailable;
  int m_colorsCount;
  QColor* m_colors;

  void writeByteToBuffer(unsigned char b);
  void incrementBufferWritePos(int i);
  void set(const QString& name, const QString& value);
};

#endif // GRAPHWIDGETPLUGININSTANCE_H
