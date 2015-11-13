#include "logdialog.h"
#include "ui_logdialog.h"

LogDialog::LogDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::LogDialog)
{
   ui->setupUi(this);
}

LogDialog::~LogDialog()
{
   delete ui;
}




/// ------------------------------------------- Private Slots -------------------------------------------




void LogDialog::appendMplayerLog(QString &log)
{
   ui->mplayerPlainTextEdit->appendPlainText(log);
}
