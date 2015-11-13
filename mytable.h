#ifndef MYTABLE_H
#define MYTABLE_H
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QMouseEvent>

class MyTable : public QTableWidget
{
   //Q_OBJECT
public:
   MyTable(QWidget *parent = 0);

   void mouseReleaseEvent(QMouseEvent *event);

signals:

public slots:
   void setDefaultColumnWidth();



private:
   QTableWidgetItem *myItem;
};


#endif // MYTABLE_H
