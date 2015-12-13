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


class mplayerWidget : public QWidget
{
   Q_OBJECT
public:
   explicit mplayerWidget(const QString& mplayer, QWidget *parent);
   ~mplayerWidget();

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
   void positionChanged(int deciSec);
   void videoIdChanged(const MediaID*);

public slots:
   void playPause();
   void pause();
   void seekTo(quint64 msec);
   void updateSubtitle();
   void volume(int);
   void fullScreen();

private slots:
   void readVideoId();
   void readStdOut();
   void readStderr();

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
   QString m_mplayerAddress;

   int m_PositionDeciSec;

   QRegExp *m_RegexLength;
   QRegExp m_RegexPosition;

   MediaID *m_MediaID;
};

#endif // MPLAYERWIDGET_H
