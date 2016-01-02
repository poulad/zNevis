#include "mplayerwidget.h"

MPlayerWidget::MPlayerWidget(const QString& mplayer, QWidget *parent)
   : QWidget(parent)
{
   qDebug() << "mplayerWidget::mplayerWidget: ";
   m_MPlayerAddress = mplayer;
   m_RegexPosition.setPattern("^A:\\s*\\d+[.]?\\d*\\s+V:\\s*(\\d+).(\\d+)\\s+");
   m_RegexPosition.setCaseSensitivity(Qt::CaseInsensitive);

   m_MediaID = new MediaID(m_MPlayerAddress, this);
   m_Process = new QProcess(this);

   m_Process->setProgram( m_MPlayerAddress );

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

   QString str = m_MPlayerAddress;
   foreach(QString s, args)
      str += " " + s;
   qDebug() << str;

   emit mplayerStdOutErr( str );
   connect(m_Process, SIGNAL(readyReadStandardOutput()), this, SLOT(readOutput()));
   connect(m_MediaID, SIGNAL(mediaIdentified()), this, SLOT(readVideoId()));
   m_Process->start(QIODevice::ReadWrite);
}


MPlayerWidget::~MPlayerWidget()
{
   quit();
   //delete m_ErrorLog;
}


const MediaID* MPlayerWidget::mediaID()
{
   qDebug() << "mplayerWidget::mediaID: ";
   return m_MediaID;
}


/// ----------------------------------------- Get Functions ------------------------------------------




const QString &MPlayerWidget::mplayerAddress()
{
   return m_MPlayerAddress;
}




/// ------------------------------------------ Set Functions -------------------------------------------




void MPlayerWidget::setMplayerAddress(const QString &address)
{
   qDebug() << "mplayerWidget::setMplayerAddress: ";
   m_MPlayerAddress = address;
   m_Process->setProgram(address);
}


void MPlayerWidget::setVideoFile(const QString &address)
{
   qDebug() << "mplayerWidget::setVideoFile: " << address;
   m_MediaID->setMedia(address);
}


void MPlayerWidget::setSubtitleFile(const QString &address)
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





void MPlayerWidget::playPause()
{
   qDebug() << "mplayerWidget::playPause: ";
   m_Command = "pause";
   sendCommand();
   m_IsPlaying = !m_IsPlaying;
   qDebug() << m_IsPlaying;
}


void MPlayerWidget::pause()
{
   m_Command = "pausing_keep_force pause";
   m_IsPlaying = false;
   sendCommand();
}


void MPlayerWidget::quit()
{
   if(m_Process->isOpen())
   {
      m_Command = "q";
      sendCommand();
      m_Process->waitForFinished(200);
      m_Process->close();
   }
}


void MPlayerWidget::seekTo(int msec)
{
   QString sec = QString::number(msec/1000);
   QString milisec = QString::number(msec%1000);
   m_Command = "pausing_keep seek " + sec + "." + milisec + " 2";
   sendCommand();
}


void MPlayerWidget::readOutput()
{
    //qDebug() << "mplayerWidget::readOutput: ";
   QString outputString = m_Process->readAll();

   if( outputString.contains(m_RegexPosition) )
   {
      m_Position = m_RegexPosition.cap(1).toInt() * 1000 + m_RegexPosition.cap(2).toInt() * 100;
      emit positionChanged(m_Position);
   }

   emit mplayerStdOutErr(outputString);
}


void MPlayerWidget::updateSubtitle()
{
   qDebug() << "mplayerWidget::updateSubtitle: ";

   m_Command = "pausing_keep sub_remove 0";
   sendCommand();
   m_Command = "pausing_keep sub_file -1";
   sendCommand();
   m_Command = "pausing_keep sub_load \"" + m_SubtitleAddress + "\"";
   sendCommand();
   m_Command = "pausing_keep sub_select 0";
   sendCommand();
}


void MPlayerWidget::sendCommand()
{
   m_Command += "\n";
   m_Process->write(QByteArray(m_Command.toLatin1()));
}


void MPlayerWidget::volume(int v)
{
   m_Command = "pausing_keep set_property volume " + QString::number(v);
   qDebug() << m_Command;
   sendCommand();
}


void MPlayerWidget::readVideoId()
{
   qDebug() << "mplayerWidget::readVideoId: ";
   if(m_MediaID->hasVideo())
   {
      emit videoIdChanged(m_MediaID);
      m_Command = "loadfile \"" + m_MediaID->fileName() + "\" 0";
      sendCommand();
      m_IsPlaying = true;
   }
}


void MPlayerWidget::fullScreen()
{
   m_Command = "f";
   sendCommand();
}








