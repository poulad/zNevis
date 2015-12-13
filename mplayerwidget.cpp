#include "mplayerwidget.h"

mplayerWidget::mplayerWidget(const QString& mplayer, QWidget *parent)
   : QWidget(parent)
{
   qDebug() << "mplayerWidget::mplayerWidget: ";
   m_mplayerAddress = mplayer;
   m_RegexPosition.setPattern("^A:\\s*\\d+[.]?\\d*\\s+V:\\s*([\\d|.]+)\\s+");
   m_RegexPosition.setCaseSensitivity(Qt::CaseInsensitive);

   m_MediaID = new MediaID(m_mplayerAddress, this);
   m_Process = new QProcess(this);

   m_Process->setProgram( m_mplayerAddress );

   QStringList args;
   args
         << "-slave"
         << "-idle"
         << "-osdlevel" << "0"
         << "-noconfig" << "all"
         << "-noautosub"
         << "-nocookies"
         << "-noborder"
         << "-ass"
         << "-wid" << QString::number(parent->winId())
            ;
   m_Process->setArguments(args);

   QString str = m_mplayerAddress;
   foreach(QString s, args)
      str += " " + s;
   qDebug() << str;

   emit mplayerStdOutErr( str );
   connect(m_Process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdOut()));
   connect(m_Process, SIGNAL(readyReadStandardError()), this, SLOT(readStderr()));
   connect(m_MediaID, SIGNAL(mediaIdentified()), this, SLOT(readVideoId()));
   m_Process->start(QIODevice::ReadWrite);
}


mplayerWidget::~mplayerWidget()
{
   quit();
   //delete m_ErrorLog;
}


const MediaID* mplayerWidget::mediaID()
{
   qDebug() << "mplayerWidget::mediaID: ";
   return m_MediaID;
}


/// ----------------------------------------- Get Functions ------------------------------------------




const QString &mplayerWidget::mplayerAddress()
{
   return m_mplayerAddress;
}




/// ------------------------------------------ Set Functions -------------------------------------------




void mplayerWidget::setMplayerAddress(const QString &address)
{
   qDebug() << "mplayerWidget::setMplayerAddress: ";
   m_mplayerAddress = address;
   m_Process->setProgram(address);
}


void mplayerWidget::setVideoFile(const QString &address)
{
   qDebug() << "mplayerWidget::setVideoFile: " << address;
   m_MediaID->setMedia(address);
}


void mplayerWidget::setSubtitleFile(const QString &address)
{
   qDebug() << "mplayerWidget::setSubtitleFile: " << address;
   m_SubtitleAddress = address;
   if(m_Process->isOpen() == true)
   {
      updateSubtitle();
   }
}




/// ------------------------------------- Other Public Functions ------------------------------------








/// ------------------------------------------- Public Slots -------------------------------------------





void mplayerWidget::playPause()
{
   qDebug() << "mplayerWidget::playPause: ";
   m_Command = "pause";
   sendCommand();
   m_IsPlaying = !m_IsPlaying;
}


void mplayerWidget::pause()
{
   m_Command = "set_property pause 1";
   sendCommand();
}




void mplayerWidget::quit()
{
   if(m_Process->isOpen())
   {
      m_Command = "q";
      sendCommand();
      m_Process->close();
   }
}


void mplayerWidget::seekTo(quint64 msec)
{
   QString sec = QString::number(msec/1000);
   QString milisec = QString::number(msec%1000);
   m_Command = "seek " + sec + "." + milisec + " 2\r\n";
   sendCommand();
}


void mplayerWidget::readStderr()
{
   QString stderrString = m_Process->readAllStandardError();
   //qDebug() << stderrString;
   if( stderrString.contains(m_RegexPosition) )
   {
      //qDebug() << stderrString;
      m_PositionDeciSec = m_RegexPosition.cap(1).remove(".").toInt();
      emit positionChanged(m_PositionDeciSec);
   }

}


void mplayerWidget::readStdOut()
{
   QString str = m_Process->readAll();
   //qDebug() << "mplayerWidget::readStdOutErr: ";
   //qDebug() << str;
   emit mplayerStdOutErr(str);
}


void mplayerWidget::updateSubtitle()
{
   qDebug() << "mplayerWidget::updateSubtitle: ";

   m_Command = "sub_remove 0\n";
   sendCommand();
   m_Command = "sub_file -1\n";
   sendCommand();
   m_Command = "sub_load " + m_SubtitleAddress + "\n";
   sendCommand();
   m_Command = "sub_select 0\n";
   sendCommand();
}


void mplayerWidget::sendCommand()
{

   m_Command += "\r\n";
   m_Process->write(QByteArray(m_Command.toLatin1()));
}


void mplayerWidget::volume(int v)
{
   m_Command = "set_property volume " + QString::number(v);
   qDebug() << m_Command;
   sendCommand();
}


void mplayerWidget::readVideoId()
{
   qDebug() << "mplayerWidget::readVideoId: ";
   if(m_MediaID->hasVideo())
   {
      emit videoIdChanged(m_MediaID);
      m_Command = "loadfile \"" + m_MediaID->fileName() + "\" 0";
      m_IsPlaying = true;
      sendCommand();
   }
}


void mplayerWidget::fullScreen()
{
   m_Command = "f";
   sendCommand();
}








