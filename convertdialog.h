#ifndef CONVERTDIALOG_H
#define CONVERTDIALOG_H

#include "mencodercontrol.h"

#include <QDialog>
#include <QProcess>
#include <QDebug>
#include <QCloseEvent>
#include <QMessageBox>

namespace Ui {
   class ConvertDialog;
}

class ConvertDialog : public QDialog
{
   Q_OBJECT

public:
   explicit ConvertDialog(MencoderControl* mencoderControl, const double length, QWidget *parent);
   ~ConvertDialog();

protected:
   void closeEvent(QCloseEvent *event);

private slots:
   void toggleDetails(bool);
   void setProgressBarValue(int);
   void appendLog(QString);
   void conversionFinished(int);

private:
   Ui::ConvertDialog *ui;
   MencoderControl *m_MencoderControl;
   double m_Length;
   bool m_ConversionFinished;
};

#endif // CONVERTDIALOG_H
