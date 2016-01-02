#ifndef MENCODERCONTROL_H
#define MENCODERCONTROL_H

#include <QObject>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QRegExp>
#include <QDebug>
#include <QTime>

class MEncoderControl : public QObject
{
   Q_OBJECT
public:
   explicit MEncoderControl(QObject *parent = 0);
   ~MEncoderControl();

   // Set Functions
   void setMencoderAddress(const QString &address);
   void setInputMedia(const QString &input);
   void setSubtitle(const QString &address);
   void setOutput(const QString& output);
   void setClipTime(const QTime &startTime, const QTime &endTime);

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
   void inline updateArgs();

   QProcess *m_Process;

   QString m_MEncoderAddress;
   QString m_Input;
   QString m_Subtitle;
   QString m_Output;
   QTime *m_StartTime;
   QTime *m_EndTime;

   QStringList m_Args;
   QString m_Log;
   QRegExp m_RegexPosition;
};

#endif // MENCODERCONTROL_H
