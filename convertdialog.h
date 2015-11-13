#ifndef CONVERTDIALOG_H
#define CONVERTDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QDebug>
#include <QCloseEvent>

namespace Ui {
   class ConvertDialog;
}

class ConvertDialog : public QDialog
{
   Q_OBJECT

public:
   explicit ConvertDialog(const QStringList &argumentsList, int length, QWidget *parent);
   ~ConvertDialog();

protected:
   void closeEvent(QCloseEvent *event);

private slots:
   void toggleDetails(bool);
   void readOutput();
   void readError();
   void convertionFinished(int, QProcess::ExitStatus);

private:
   Ui::ConvertDialog *ui;
   QProcess *m_Process;
   QRegExp *m_PositionRegex;
   int m_length;
};

#endif // CONVERTDIALOG_H
