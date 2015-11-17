#include "convertdialog.h"
#include "ui_convertdialog.h"

ConvertDialog::ConvertDialog(const QStringList &filesList, const double length, QWidget *parent) :
   QDialog(parent),
   ui(new Ui::ConvertDialog)
{
   ui->setupUi(this);
   ui->textEdit->setHidden(true);
   connect(ui->detailsButton, SIGNAL(toggled(bool)), this, SLOT(toggleDetails(bool)));
   this->adjustSize();

   m_Length = length;
   m_ConversionFinished = false;

   m_MencoderControl = new MencoderControl(parent);
   connect(m_MencoderControl, SIGNAL(positionChanged(int)), this, SLOT(setProgressBarValue(int)));
   connect(m_MencoderControl, SIGNAL(logRead(QString)), this, SLOT(appendLog(QString)));
   connect(m_MencoderControl, SIGNAL(finished(int)), this, SLOT(conversionFinished(int)));
   m_MencoderControl->setFiles(filesList);
   m_MencoderControl->startConversion();
}


ConvertDialog::~ConvertDialog()
{
   delete ui;
}


void ConvertDialog::closeEvent(QCloseEvent *event)
{
   if(m_ConversionFinished)
      event->accept();
   else
      event->ignore();
}

/*
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
}*/


void ConvertDialog::toggleDetails(bool b)
{
   ui->textEdit->setVisible(b);
   ui->detailsButton->setText( (b ? "Hide details <<" : "Show details >>") );
   if(!b)
      this->adjustSize();
}


void ConvertDialog::setProgressBarValue(int second)
{
   qDebug() << "ConverDialog::setProgressBarValue: " << "second=" << QString::number(second);
   int posPercentage = second*100/m_Length;
   ui->progressBar->setValue(posPercentage);
}


void ConvertDialog::appendLog(QString log)
{
   ui->textEdit->append(log);
}


void ConvertDialog::conversionFinished(int exitCode)
{
   m_ConversionFinished = true;
   if(ui->autoCloseCheckBox->isChecked())
   {
      close();
   }
   else
   {

   }
}
