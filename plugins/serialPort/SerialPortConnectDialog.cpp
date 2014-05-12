#include "SerialPortConnectDialog.h"
#include "ui_SerialPortConnectDialog.h"

SerialPortConnectDialog::SerialPortConnectDialog(QWidget* parent) :
  QDialog(parent),
  ui(new Ui::SerialPortConnectDialog) {
  ui->setupUi(this);

  refreshPortNameList();
}

SerialPortConnectDialog::~SerialPortConnectDialog() {
  delete ui;
}

void SerialPortConnectDialog::refreshPortNameList() {
  ui->portName->clear();
  foreach(const QSerialPortInfo & info, QSerialPortInfo::availablePorts()) {
    if(info.manufacturer().length() > 0) {
      QString description = " (" + info.description() + "/" + info.manufacturer() + ")";
      QString text = info.portName() + description;
      ui->portName->addItem(text, info.portName());
    }
  }
}

void SerialPortConnectDialog::setPortName(const QString& portName) {
  ui->portName->setEditText(portName);
}

void SerialPortConnectDialog::setBaudRate(int baudRate) {
  ui->baudRate->setText(QString::number(baudRate));
}

QString SerialPortConnectDialog::getPortName() const {
  QString data = ui->portName->currentData().toString();
  if(data.length() > 0) {
    return data;
  }
  return ui->portName->currentText();
}

int SerialPortConnectDialog::getBaudRate() const {
  return ui->baudRate->text().toInt();
}
