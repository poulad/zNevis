#include "mainwindow.h"
#include "chooseencodingdialog.h"
#include <QApplication>
#include <QList>
#include <QDebug>

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);
   MainWindow *w = new MainWindow();
   //w->show();
   //ChooseEncodingDialog *d = new ChooseEncodingDialog();
   //d->show();

   return a.exec();
}
