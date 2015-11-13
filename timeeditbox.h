#ifndef TIMEEDITBOX_H
#define TIMEEDITBOX_H
#include <QTimeEdit>
#include <QLineEdit>

class TimeEditBox : public QTimeEdit
{
public:
   TimeEditBox(QWidget *parent = 0);
   void setTimeWithoutSignal(QTime time);

private:
   //QLineEdit *timeLineEdit;

};

#endif // TIMEEDITBOX_H
