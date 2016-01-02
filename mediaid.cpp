#include "mediaid.h"

MediaID::MediaID(QString mplayer, QObject *parent)
   :  QObject(parent)
{
   clear();
   m_IdRegex.setPattern("^ID_([A-Z|0-9|_]+)=([A-Z|0-9|_|:|-|.|/|\\s|\\w]+)$");
   m_IdRegex.setCaseSensitivity(Qt::CaseInsensitive);
   m_FileFormatRegex.setPattern("^Detected\\s+file\\s+format:\\s+(*)$");
   m_FileFormatRegex.setCaseSensitivity(Qt::CaseInsensitive);

   m_MPlayerAddress = mplayer;
   m_Process = new QProcess(this);
   m_Process->setProgram(m_MPlayerAddress);
   connect(m_Process, SIGNAL(finished(int)), this, SLOT(identifyMedia(int)));
}


void MediaID::clear()
{
   m_Length = 0;

   m_HasAudio = false;
   m_AudioFormat = "";
   m_AudioCodec = "";
   m_AudioBitrate = 0;
   m_AudioRate = 0;
   m_AudioChannels = 0;

   m_HasVideo = false;
    m_VideoFormat = "";
    m_VideoCodec = "";
   m_Width = 0;
   m_Height = 0;
   m_fps = 0;
   m_VideoBitrate = 0;
   m_AspectRatio = 0;

   m_HasSubtitle = false;
    m_SubtitleAddress = "";
}


void MediaID::setMedia(const QString &media)
{
   clear();
   m_Address = media;
   QString command;
   command = m_MPlayerAddress + " -identify -endpos 0 " + "\"" + m_Address + "\"";
   m_Process->start(command);
}


void MediaID::identifyMedia(int exitCode)
{
   m_StdOutErr = m_Process->readAll();
   if(exitCode != 0)
   {
      emit failed(false);
      return;
   }

   QStringList lines = m_StdOutErr.split("\n");
   foreach(QString str, lines)
   {
      str.contains(m_IdRegex);
      str.contains(m_FileFormatRegex);
      if(m_FileFormatRegex.cap(1).contains("subtitle"))
         m_HasSubtitle = true;
      else if(m_IdRegex.cap(1) == "AUDIO_ID")
         m_HasAudio = true;
      else if(m_IdRegex.cap(1) == "VIDEO_ID")
         m_HasVideo = true;
      else if(m_IdRegex.cap(1) == "FILE_SUB_ID")
         m_HasSubtitle = true;
      else if(m_IdRegex.cap(1) == "SUBTITLE_ID")
         m_HasSubtitle = true;
      else if(m_IdRegex.cap(1) == "LENGTH")
      {
         QString length = m_IdRegex.cap(2).remove(".");
         m_Length = length.toInt();
      }
      else if(m_IdRegex.cap(1) == "AUDIO_FORMAT")
         m_AudioFormat = m_IdRegex.cap(2);
      else if(m_IdRegex.cap(1) == "AUDIO_CODEC")
         m_AudioCodec = m_IdRegex.cap(2);
      else if(m_IdRegex.cap(1) == "AUDIO_BITRATE")
         m_AudioBitrate = m_IdRegex.cap(2).toInt();
      else if(m_IdRegex.cap(1) == "AUDIO_RATE")
         m_AudioRate = m_IdRegex.cap(2).toInt();
      else if(m_IdRegex.cap(1) == "AUDIO_NCH")
         m_AudioChannels = m_IdRegex.cap(2).toInt();
      else if(m_IdRegex.cap(1) == "VIDEO_FORMAT")
         m_VideoFormat = m_IdRegex.cap(2);
      else if(m_IdRegex.cap(1) == "VIDEO_CODEC")
         m_VideoCodec = m_IdRegex.cap(2);
      else if(m_IdRegex.cap(1) == "VIDEO_WIDTH")
         m_Width = m_IdRegex.cap(2).toInt();
      else if(m_IdRegex.cap(1) == "VIDEO_HEIGHT")
         m_Height = m_IdRegex.cap(2).toInt();
      else if(m_IdRegex.cap(1) == "VIDEO_FPS")
         m_fps = m_IdRegex.cap(2).toInt();
      else if(m_IdRegex.cap(1) == "VIDEO_BITRATE")
         m_VideoBitrate = m_IdRegex.cap(2).toInt();
      else if(m_IdRegex.cap(1) == "VIDEO_ASPECT")
         m_AspectRatio = m_IdRegex.cap(2).toDouble();
      else if(m_IdRegex.cap(1) == "FILE_SUB_FILENAME")
      {
         QFile file(m_IdRegex.cap(2));
         m_SubtitleAddress = file.fileName();
      }
   }

   //qDebug() << "Audio:" << m_HasAudio << m_AudioFormat << m_AudioCodec << QString::number(m_AudioBitrate) << QString::number(m_AudioRate) << QString::number(m_AudioChannels);
   //qDebug() << "Video:" << m_HasVideo << m_VideoFormat << m_VideoCodec << QString::number(m_Width) << QString::number(m_Height) << QString::number(m_fps) << QString::number(m_VideoBitrate) << QString::number(m_AspectRatio);
   //qDebug() << "Subtitle:" << m_HasSubtitle << m_SubtitleAddress;
   emit mediaIdentified();
}
