#ifndef MPLAYERCONTROL_H
#define MPLAYERCONTROL_H
#include <QProcess>
#include <QStringList>
#include <QString>
#include <QDebug>
#include <QRegExp>
#include <QStandardPaths>

class MplayerControl : public QObject
{
   Q_OBJECT
public:
   explicit MplayerControl(QObject *parent);
   ~MplayerControl();

   // Get functions:
   QStringList &videoIdList();
   const QString &mplayerAddress();

   //Set functions:
   void setMplayerAddress(const QString& address);
   void setVideoFile(const QString& address);

   //Other
   void locateMplayer();
   void startPlaying(const QString &winId);

signals:
   void mplayerExists();
   void mplayerCrashed(QString*);
   void positionChanged(quint64 second, quint64 msec);
   void videoIdread(QStringList &videoIdList);

public slots:
   void playPause();
   void pause();
   void quit();
   void seekTo(quint64 msec);
   void updateSubtitle();
   void volume(int);
   void fullScreen();

private slots:
   void readVideoId();
   void readStdout();
   void readStderr();
   void checkMplayer(int, QProcess::ExitStatus);

private:
   void startChecking();
   void identifyVideo();
   inline void sendCommand();

   QStringList m_Args;
   QStringList m_VideoIdList;

   QString m_WinId;
   QString *m_VideoIdString;
   QProcess *m_Process;
   QString m_Command;
   QString *m_VideoFile;
   QString m_SubtitleFile;

   QString m_ErrorLog;
   QString m_MplayerAddress;

   bool m_MplayerExists;

   QRegExp *m_RegexLength;
   QRegExp *m_RegexPosition;
};

#endif // MPLAYERCONTROL_H
