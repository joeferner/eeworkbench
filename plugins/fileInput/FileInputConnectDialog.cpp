#include <QFileDialog>
#include "FileInputConnectDialog.h"
#include "ui_FileInputConnectDialog.h"

FileInputConnectDialog::FileInputConnectDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::FileInputConnectDialog)
{
  ui->setupUi(this);
}

FileInputConnectDialog::~FileInputConnectDialog()
{
  delete ui;
}

void FileInputConnectDialog::on_fileNameBrowse_clicked()
{
  QString fileName = QFileDialog::getOpenFileName(this, "Open", QString(), "EEWorkbench (*.eew);;All Files (*.*)");
  ui->fileName->setText(fileName);
}

void FileInputConnectDialog::setFileName(const QString& fileName) {
  ui->fileName->setText(fileName);
}

QString FileInputConnectDialog::getFileName() const {
  return ui->fileName->text();
}
