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
   explicit MplayerControl(const QString &mplayerAddress, const QString &videoAddress, QObject *parent);
   ~MplayerControl();

   // Get functions:
   QStringList &videoIdList();
   const QString &mplayerAddress();

   //Set functions:
   void setMplayerAddress(const QString& address);
   void setWinId(const QString& winId);
   void setVideoFile(const QString& address);
   void setSubtitleFile(const QString& address);

   //Other
   void identifyVideo();

signals:
   void mplayerExists();
   void mplayerStdOutErr(const QString&);
   void mplayerCrashed(QString*);
   void positionChanged(quint64 second, quint64 msec);
   void videoIdChanged(QStringList &videoIdList);

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
   void readStdOutErr();
   void readStderr();

private:
   inline void sendCommand();
   inline void updateArgsList();

   bool m_IsPlaying;

   QStringList m_ArgsList;
   QStringList m_VideoIdList;

   QString m_WinId;
   QString *m_VideoIdString;

   QProcess *m_Process;
   QProcess *m_IdentifyProcess;

   QString m_Command;
   QString m_VideoAddress;
   QString m_SubtitleAddress;

   QString m_ErrorLog;
   QString m_MplayerAddress;

   QRegExp *m_RegexLength;
   QRegExp m_RegexPosition;
};

#endif // MPLAYERCONTROL_H
