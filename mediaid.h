#ifndef MEDIAID_H
#define MEDIAID_H

#include <QProcess>
#include <QStringList>
#include <QRegExp>
#include <QFile>


class MediaID : public QObject
{
   Q_OBJECT
public:
   explicit MediaID(QString mplayer, QObject *parent = 0);
   // Get Functions:
   QString fileName() const {return m_Address;}
   int length() const {return m_Length;}

   bool hasAudtio() const {return m_HasAudio;}
   QString audioFormat() const {return  m_AudioFormat;}
   QString audioCodec() const {return m_AudioCodec;}
   int audioBitrate() const {return m_AudioBitrate;}
   int audioRate() const {return m_AudioRate;}
   int audioChannels() const {return m_AudioChannels;}

   bool hasVideo() const {return m_HasVideo;}
   QString videoFormat() const {return m_VideoFormat;}
   QString videoCodec() const {return m_VideoCodec;}
   int width() const {return m_Width;}
   int height() const {return m_Height;}
   double fps() const {return m_fps;}
   int videoBitrate() const {return m_VideoBitrate;}
   double aspectRatio() const {return m_AspectRatio;}

   bool hasSubtitle() const {return  m_HasSubtitle;}
   QString subtitleAddress() const {return m_SubtitleAddress;}



signals:
   void mediaIdentified();
   void failed(bool isMedia);

public slots:
   void setMedia(const QString &media);


private slots:
   void identifyMedia(int);

private:
   void clear();
   void readAudioId();
   void readVideoId();
   void readSubtitleId();

   QProcess *m_Process;
   QString m_MPlayerAddress;
   QString m_StdOutErr;
   QStringList m_OutputLines;
   QRegExp m_IdRegex;
   QRegExp m_FileFormatRegex;

   QString m_Address;
   int m_Length;

   bool m_HasAudio;
   QString m_AudioFormat;
   QString m_AudioCodec;
   int m_AudioBitrate;
   int m_AudioRate;
   int m_AudioChannels;

   bool m_HasVideo;
   QString m_VideoFormat;
   QString m_VideoCodec;
   int m_Width;
   int m_Height;
   double m_fps;
   int m_VideoBitrate;
   double m_AspectRatio;

   bool m_HasSubtitle;
   QString m_SubtitleAddress;
};

#endif // MEDIAID_H
