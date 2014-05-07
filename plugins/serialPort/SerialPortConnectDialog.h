#ifndef SERIALPORTCONNECTDIALOG_H
#define SERIALPORTCONNECTDIALOG_H

#include <QDialog>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class SerialPortConnectDialog;
}

class SerialPortConnectDialog : public QDialog
{
  Q_OBJECT

public:
  explicit SerialPortConnectDialog(QWidget *parent = 0);
  ~SerialPortConnectDialog();

  void refreshPortNameList();

  void setPortName(const QString& portName);
  void setBaudRate(int baudRate);

  QString getPortName() const;
  int getBaudRate() const;

private:
  Ui::SerialPortConnectDialog *ui;
};

#endif // SERIALPORTCONNECTDIALOG_H
