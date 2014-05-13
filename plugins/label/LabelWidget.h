#ifndef LABELWIDGET_H
#define LABELWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QFormLayout>

class LabelWidgetPluginInstance;

class LabelWidget : public QWidget {
  Q_OBJECT

public:
  explicit LabelWidget(LabelWidgetPluginInstance* labelWidgetPluginInstance);
  virtual ~LabelWidget();

  void setTitle(const QString& title) { m_title->setText(title); }
  void setText(const QString& text) { m_text->setText(text); }
  QString getTitle() const { return m_title->text(); }
  QString getText() const { return m_text->text(); }

private:
  LabelWidgetPluginInstance* m_labelWidgetPluginInstance;
  QFormLayout* m_layout;
  QLabel* m_title;
  QLabel* m_text;
};

#endif // LABELWIDGET_H
