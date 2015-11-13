#ifndef MYTABLEWIDGETITEM_H
#define MYTABLEWIDGETITEM_H
#include <QTableWidgetItem>

class MyTableWidgetItem : public QTableWidgetItem
{
   //Q_OBJECT
public:
   MyTableWidgetItem(const QString & text, int type = Type);

};

#endif // MYTABLEWIDGETITEM_H
