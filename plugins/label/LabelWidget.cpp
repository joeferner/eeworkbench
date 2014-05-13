#include "LabelWidget.h"
#include "LabelWidgetPluginInstance.h"

LabelWidget::LabelWidget(LabelWidgetPluginInstance* labelWidgetPluginInstance) :
  QWidget(NULL),
  m_labelWidgetPluginInstance(labelWidgetPluginInstance)
{
  m_layout = new QFormLayout();
  setLayout(m_layout);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  m_title = new QLabel();
  m_title->setText("Title");
  m_layout->addWidget(m_title);

  m_text = new QLabel();
  m_text->setTextInteractionFlags(Qt::TextSelectableByMouse);
  m_text->setIndent(10);
  m_text->setText("Text");
  m_layout->addWidget(m_text);
}

LabelWidget::~LabelWidget() {
  delete m_title;
  delete m_text;
  delete m_layout;
}
