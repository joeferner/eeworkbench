#ifndef FILEINPUTCONNECTDIALOG_H
#define FILEINPUTCONNECTDIALOG_H

#include <QDialog>

namespace Ui {
class FileInputConnectDialog;
}

class FileInputConnectDialog : public QDialog {
  Q_OBJECT

public:
  explicit FileInputConnectDialog(QWidget* parent = 0);
  ~FileInputConnectDialog();
  void setFileName(const QString& fileName);
  QString getFileName() const;

private slots:
  void on_fileNameBrowse_clicked();

private:
  Ui::FileInputConnectDialog* ui;
};

#endif // FILEINPUTCONNECTDIALOG_H
