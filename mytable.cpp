#include "mytable.h"
#include <QDebug>

MyTable::MyTable(QWidget *parent)
   :  QTableWidget(parent)
{
   setSelectionBehavior(QAbstractItemView::SelectRows);
   setEditTriggers(QAbstractItemView::NoEditTriggers);
   setSelectionMode(QAbstractItemView::SingleSelection);
   setColumnCount(6);
   setRowCount(3);
   myItem = new QTableWidgetItem("Line #");
   setHorizontalHeaderItem(0, myItem);
}

void MyTable::setDefaultColumnWidth()
{
   for(int i = 0; i < columnCount(); i++)
   setColumnWidth(i, sizeHintForColumn(i));
}


///   Private SLOTs:


///   Events:
void MyTable::mouseReleaseEvent(QMouseEvent *event)
{
   if( event->button() == Qt::RightButton)
   {
      qDebug() << "SHow contex menu:font,color|";
   }

}
