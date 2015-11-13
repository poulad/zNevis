#include "convertdialog.h"
#include "ui_convertdialog.h"

ConvertDialog::ConvertDialog(const QStringList &argumentsList, int length, QWidget *parent) :
   QDialog(parent),
   ui(new Ui::ConvertDialog)
{
   ui->setupUi(this);
   ui->textEdit->setHidden(true);
   connect(ui->detailsButton, SIGNAL(toggled(bool)), this, SLOT(toggleDetails(bool)));
   this->adjustSize();
   ui->progressBar->setMaximum(length);
   m_Process = new QProcess(this);
   m_PositionRegex = new QRegExp("Pos:[\\t ]*(\\d+)[,.]\\d+[s]?[ \\t]+", Qt::CaseInsensitive);
   m_Process->setProgram("mencoder");
   m_Process->setArguments(argumentsList);
   connect(m_Process, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
   connect(m_Process, SIGNAL(readyReadStandardError()), this, SLOT(readError()));
   connect(m_Process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(convertionFinished(int,QProcess::ExitStatus)));
   m_Process->start();
}


ConvertDialog::~ConvertDialog()
{
   delete ui;
}


void ConvertDialog::readOutput()
{
   QString output = m_Process->readAllStandardOutput();
   ui->textEdit->append(output);
   qDebug() << output;
   if(output.contains(*m_PositionRegex))
   {
      int position = m_PositionRegex->cap(1).toInt();
      //qDebug() << "======================================" << QString::number(position );
      ui->progressBar->setValue( position );
   }
}


void ConvertDialog::closeEvent(QCloseEvent *event)
{
   if( m_Process->state() == QProcess::NotRunning )
      event->accept();
   else
      event->ignore();
}


void ConvertDialog::readError()
{
   QString error = m_Process->readAllStandardError();
   ui->textEdit->append(error);
   qDebug() << error;
}


void ConvertDialog::convertionFinished(int exitCode, QProcess::ExitStatus status)
{
   qDebug() << "Exit code: " << QString::number(exitCode);
   if( status == QProcess::NormalExit && exitCode == 0 )
   {
      ui->progressBar->setValue( ui->progressBar->maximum() );

      if( ui->autoCloseCheckBox->isChecked() )
      {
         this->done(0);
      }
   }
}


void ConvertDialog::toggleDetails(bool b)
{
   ui->textEdit->setVisible(b);
   ui->detailsButton->setText( (b ? "Hide details <<" : "Show details >>") );
   if(!b)
      this->adjustSize();
}
