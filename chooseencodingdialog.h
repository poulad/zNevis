#ifndef CHOOSEENCODINGDIALOG_H
#define CHOOSEENCODINGDIALOG_H
#include "subtitle.h"
#include <QDialog>

namespace Ui {
   class ChooseEncodingDialog;
}

class ChooseEncodingDialog : public QDialog
{
   Q_OBJECT

public:
   explicit ChooseEncodingDialog(QWidget *parent = 0);
   ~ChooseEncodingDialog();

private slots:
   void read();

private:
   Ui::ChooseEncodingDialog *ui;
   Subtitle *subtitle;
};

#endif // CHOOSEENCODINGDIALOG_H
