#include "chooseencodingdialog.h"
#include "ui_chooseencodingdialog.h"
#include <QDebug>
#include <QStringList>
#include <QTextCodec>
#include <QTableWidgetItem>

ChooseEncodingDialog::ChooseEncodingDialog(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::ChooseEncodingDialog)
{
   ui->setupUi(this);
   subtitle = new Subtitle();
   connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(read()));
   //QStringList lines = subtitle->readSubtitleLines(1, 3);
   //qDebug() << lines;

   //for(int i = 0; i < 3 ; i++)
      //ui->tableWidget->setItem(i, 1, new QTableWidgetItem( subtitle->getLineText(i) ) );

}

ChooseEncodingDialog::~ChooseEncodingDialog()
{
   delete ui;
}

void ChooseEncodingDialog::read()
{
   subtitle->load( QTextCodec::codecForName("windows-1256") );
   //for(int i = 0; i < 3; i++)
      //ui->tableWidget->setItem(i, 1, new QTableWidgetItem(subtitle->getText(i+1)) );
}




























