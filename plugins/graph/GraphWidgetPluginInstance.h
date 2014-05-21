#ifndef GRAPHWIDGETPLUGININSTANCE_H
#define GRAPHWIDGETPLUGININSTANCE_H

#include "../WidgetPluginInstance.h"
#include "GraphWidget.h"
#include "GraphAnalyzer.h"
#include <QList>
#include <QColor>
#include <QObject>

struct GraphSignal {
  QString name;
  int bits;
  int maxValue;
  int scaleMin;
  int scaleMax;
  QColor color;
};

class GraphWidgetPluginInstance : public WidgetPluginInstance {
  Q_OBJECT

public:
  GraphWidgetPluginInstance(WidgetPlugin* widgetPlugin, const QString& name);
  virtual ~GraphWidgetPluginInstance();

  typedef bool(*sampleCompareFn)(double originalValue, double d1, double d2);
  static bool sampleCompareNotEqual(double originalValue, double d1, double d2);
  static bool sampleCompareEqual(double originalValue, double d1, double d2);
  static bool sampleCompareFallingThrough(double originalValue, double d1, double d2);
  static bool sampleCompareRisingThrough(double originalValue, double d1, double d2);

  virtual void runCommand(const QString& functionName, QStringList args, InputPlugin* inputPlugin);
  virtual QWidget* getWidget() { return m_widget; }
  virtual void save(QTextStream& out);
  void addSignal(const QString& name, int bits, double scaleMin, double scaleMax);
  void addData(QStringList args);
  void beginData(InputPlugin* inputPlugin, int numberOfBytes);
  int getSignalIndex(GraphSignal* signal);

  const QList<GraphSignal*> getSignals() const { return m_signals; }
  const unsigned char* getBuffer() const { return m_buffer; }
  int getBufferSize() const { return m_bufferSize; }
  int getBufferWritePos() const { return m_bufferWritePos; }
  int getBufferAvailable() const { return m_bufferAvailable; }
  double getTimePerSample() const { return m_timePerSample; }
  int getSampleRate() const { return 1.0 / m_timePerSample; }
  int getBytesPerSample() const;
  double getValue(int sample, int signal);
  int getBinaryValue(int sample, int signal);
  int getBinaryValues(int startSample, int signal, unsigned char* buffer, int bufferLength);
  int findSample(int startingSample, int signalNumber, int direction, sampleCompareFn fn);
  int findBinarySample(int startingSample, int signalNumber, int value);
  int findBinarySample(int startingSample, int signalNumber, int direction, int value);
  void clear();
  GraphAnalyzer* getAnalyzer(const QString& name);
  const QList<GraphAnalyzer*> getAnalyzers() const { return m_graphAnalyzers; }
  const QList<GraphAnalyzerInstance*> getAnalyzerInstances() const { return m_graphAnalyzerInstances; }

  void addAnalyzer(const QString& name, const QString& config);
  void addAnalyzer(GraphAnalyzer* graphAnalyzer);

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
  QList<GraphAnalyzer*> m_graphAnalyzers;
  QList<GraphAnalyzerInstance*> m_graphAnalyzerInstances;

  void writeByteToBuffer(unsigned char b);
  void incrementBufferWritePos(int i);
  void set(const QString& name, const QString& value);
  void refreshAnalyzers();

signals:
  void dataAdded();
};

#endif // GRAPHWIDGETPLUGININSTANCE_H
