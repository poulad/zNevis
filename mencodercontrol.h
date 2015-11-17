#ifndef MENCODERCONTROL_H
#define MENCODERCONTROL_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QDebug>

class MencoderControl : public QObject
{
   Q_OBJECT
public:
   explicit MencoderControl(QObject *parent = 0);

   // Set Functions
   void setFiles(const QStringList& files);

   // Other
   void startConversion();

signals:
   void positionChanged(int);
   void logRead(QString);
   void finished(int);

private slots:
   void readStdOutErr();
   void processFinished(int);

private:
   QProcess *m_Process;
   QStringList m_FilesList;
   QString m_Log;
   QRegExp m_RegexPosition;
};

#endif // MENCODERCONTROL_H
