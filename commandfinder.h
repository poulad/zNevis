#ifndef COMMANDFINDER_H
#define COMMANDFINDER_H

#include <QObject>

class commandFinder : public QObject
{
   Q_OBJECT
public:
   explicit commandFinder(QObject *parent = 0);

signals:

public slots:
};

#endif // COMMANDFINDER_H
