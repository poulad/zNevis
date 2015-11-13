#include "timeeditbox.h"
#include <QDebug>

TimeEditBox::TimeEditBox(QWidget *parent)
   :  QTimeEdit(parent)
{
   //qDebug() << "hei";
   //timeLineEdit = new QLineEdit(this);
}

void TimeEditBox::setTimeWithoutSignal(QTime newTime)
{
   setTime( newTime );
   //time().setHMS( newTime.hour(), newTime.minute(), newTime.second() );
   //qDebug() << "time Changed to " + time().toString("hh:mm:ss,zzz");
}

//QTime TimeEditBox::getTime
