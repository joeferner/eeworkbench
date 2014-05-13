#include "LabelWidget.h"
#include "LabelWidgetPluginInstance.h"

LabelWidget::LabelWidget(LabelWidgetPluginInstance* labelWidgetPluginInstance) :
  QWidget(NULL),
  m_labelWidgetPluginInstance(labelWidgetPluginInstance)
{
  m_title = new QLabel(this);
  m_title->setText("Title");

  m_text = new QLabel(this);
  m_text->setText("Text");
}

LabelWidget::~LabelWidget() {
  delete m_title;
  delete m_text;
}
