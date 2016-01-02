#ifndef MPLAYERWIDGET_H
#define MPLAYERWIDGET_H

#include <QWidget>
#include <QProcess>
#include <QStringList>
#include <QString>
#include <QDebug>
#include <QRegExp>
#include <QStandardPaths>

#include "mediaid.h"


class MPlayerWidget : public QWidget
{
   Q_OBJECT
public:
   explicit MPlayerWidget(const QString& mplayer, QWidget *parent);
   ~MPlayerWidget();

   // Get functions:
   const MediaID *mediaID();
   const QString &mplayerAddress();
   bool isPlaying() const { return m_IsPlaying; }

   //Set functions:
   void setMplayerAddress(const QString& address);
   void setVideoFile(const QString& address);
   void setSubtitleFile(const QString& address);

   //Other:
   void identifyVideo();

signals:
   void mplayerExists();
   void mplayerStdOutErr(const QString&);
   void mplayerCrashed(QString*);
   void positionChanged(int mSec);
   void videoIdChanged(const MediaID*);

public slots:
   void playPause();
   void pause();
   void seekTo(int msec);
   void updateSubtitle();
   void volume(int);
   void fullScreen();

private slots:
   void readVideoId();
   void readOutput();

private:
   void quit();
   inline void sendCommand();

   // Private Members
   bool m_IsPlaying;

   QString *m_VideoIdString;

   QProcess *m_Process;
   QProcess *m_IdentifyProcess;

   QString m_Command;
   QString m_VideoAddress;
   QString m_SubtitleAddress;

   QString m_ErrorLog;
   QString m_MPlayerAddress;

   int m_Position;

   QRegExp *m_RegexLength;
   QRegExp m_RegexPosition;

   MediaID *m_MediaID;
};

#endif // MPLAYERWIDGET_H
