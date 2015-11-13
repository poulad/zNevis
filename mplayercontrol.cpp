#include "mplayercontrol.h"

MplayerControl::MplayerControl(QObject *parent)
   : QObject(parent)
{
   qDebug() << "MplayerControl::MplayerControl: ";
   m_VideoFile = new QString();
   m_VideoIdString = new QString();
   m_RegexPosition = new QRegExp( "^A:\\s*\\d+[.]?\\d*\\s+V:\\s*(\\d+)(?:[.,]?(\\d+))\\s+", Qt::CaseInsensitive);
   m_RegexLength = new QRegExp("ID_LENGTH=(\\d+)(?:[,.](\\d+))?", Qt::CaseInsensitive);

   m_Process = new QProcess(parent);
}


MplayerControl::~MplayerControl()
{
   //delete m_ErrorLog;
}


QStringList &MplayerControl::videoIdList()
{
   qDebug() << "MplayerControl::videoIdList: ";
   return m_VideoIdList;
}


/// ----------------------------------------- Get Functions ------------------------------------------




const QString &MplayerControl::mplayerAddress()
{
   return m_MplayerAddress;
}




/// ------------------------------------------ Set Functions -------------------------------------------




void MplayerControl::setMplayerAddress(const QString &address)
{
   qDebug() << "MplayerControl::setMplayerAddress: ";
   m_MplayerAddress = address;
   m_Process->setProgram(m_MplayerAddress);
   startChecking();
}


void MplayerControl::setVideoFile(const QString &address)
{
   qDebug() << "MplayerControl::setVideoFile: ";
   /*
   if(m_Process->isOpen())
   {
      quit();
      m_Process->close();
   }*/
   m_VideoFile->clear();
   m_VideoFile->append(address);
   m_Args.clear();
   m_Args << *m_VideoFile << "-identify" << "-endpos" << "0";
   m_Process->setArguments(m_Args);
   connect(m_Process, SIGNAL(readyRead()), this, SLOT(readVideoId()));
   m_Process->start();
}




/// ------------------------------------- Other Public Functions ------------------------------------




void MplayerControl::locateMplayer()
{
   m_MplayerAddress = QStandardPaths::findExecutable("mplayer");
   if(m_MplayerAddress.isEmpty())
   {
      m_MplayerAddress = QStandardPaths::findExecutable("mplayer", QStringList(":/mplayer/"));
      if(m_MplayerAddress.isEmpty())
      {
         m_ErrorLog = "MplayerControl::MplayerControl: Failed to find mplayer executable";
         qDebug() << m_ErrorLog;
         emit mplayerCrashed(&m_ErrorLog);
      }
      else
      {
         m_Process->setProgram(m_MplayerAddress);
         startChecking();
      }
   }
   else
   {
      m_Process->setProgram(m_MplayerAddress);
      startChecking();
   }
}




/// ------------------------------------------- Public Slots -------------------------------------------





void MplayerControl::playPause()
{
   m_Command = "p";
   sendCommand();
}


void MplayerControl::pause()
{
   m_Command = "set_property pause 1";
   sendCommand();
}




void MplayerControl::quit()
{
   if(m_Process->isOpen())
   {
      m_Command = "q";
      sendCommand();
      m_Process->close();
   }
}


void MplayerControl::seekTo(quint64 msec)
{
   QString sec = QString::number(msec/1000);
   QString milisec = QString::number(msec%1000);
   m_Command = "seek " + sec + "." + milisec + " 2\r\n";
   sendCommand();
}


void MplayerControl::readStderr()
{
   QString stderrString = m_Process->readAllStandardError();
   //qDebug() << stderrString;
   if( stderrString.contains(*m_RegexPosition) )
   {
      quint64 sec = m_RegexPosition->cap(1).toInt();
      bool ok = true;
      quint64 msec = m_RegexPosition->cap(2).toInt(&ok);
      if(ok == false)
         msec = 0;
      emit positionChanged( sec, msec );
   }

}


void MplayerControl::readStdout()
{

}


void MplayerControl::updateSubtitle()
{
   //qDebug() << "MplayerControl::" << "updateSubtitle# ";
   m_Command = "sub_remove 0\r\n";
   sendCommand();
   m_Command = "sub_file -1\r\n";
   sendCommand();
   m_Command = "sub_load " + m_SubtitleFile + "\r\n";
   sendCommand();
   m_Command = "sub_select 0\r\n";
   sendCommand();
}


void MplayerControl::readVideoId()
{
   qDebug() << "MplayerControl::readVideoId: ";
   QString str = m_Process->readAll();
   m_VideoIdString->append(str);
   if(m_VideoIdString->contains("ID_EXIT=EOF", Qt::CaseInsensitive))
   {
      identifyVideo();
   }
}


void MplayerControl::startPlaying(const QString &winId)
{
   if(m_Process->isOpen())
   {
      quit();
      m_Process->close();
   }
   m_WinId = winId;
   m_Args.removeLast(); // 0
   m_Args.removeLast(); // -endpos
   m_Args.removeLast(); // -identify
   m_Args
         << "-slave"
         << "-osdlevel"
         << "0"
         << "-wid" << m_WinId
            ;
   m_Process->setArguments(m_Args);
   connect(m_Process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdout()));
   connect(m_Process, SIGNAL(readyReadStandardError()), this, SLOT(readStderr()));
   m_Process->start();
}


void MplayerControl::sendCommand()
{
   m_Command += "\r\n";
   m_Process->write(QByteArray(m_Command.toLatin1()));
}


void MplayerControl::volume(int v)
{
   m_Command = "set_property volume " + QString::number(v);
   qDebug() << m_Command;
   sendCommand();
}


void MplayerControl::identifyVideo()
{
   qDebug() << "MplayerControl::identifyVideo: ";
   disconnect(m_Process, SIGNAL(readyRead()), this, SLOT(readVideoId()));
   QStringList list = m_VideoIdString->split("\n");
   QRegExp regexId( "ID_((?:[A-Z]|_|\\d)*)=((?:\\d|[A-Z]|[a-z]|_|:| |\t|-|\\.)*)", Qt::CaseInsensitive );
   foreach(QString strId, list)
   {
      if( strId.contains(regexId) )
         if( regexId.cap(2) != "" )
         {
            m_VideoIdList.append( regexId.cap(1) );
            m_VideoIdList.append( regexId.cap(2) );
         }
   }
   emit videoIdread(m_VideoIdList);
}


void MplayerControl::fullScreen()
{
   m_Command = "f";
   sendCommand();
}




/// ------------------------------------------- Private Slots -------------------------------------------




void MplayerControl::checkMplayer(int exitCode, QProcess::ExitStatus status)
{
   if(exitCode == 0 && status == QProcess::NormalExit)
   {
      qDebug() << "MplayerControl::checkMplayer: exists at \"" + m_MplayerAddress + "\"";
      m_MplayerExists = true;
      emit mplayerExists();
   }
   else
   {
      if(m_MplayerExists == false)
      {
         qDebug() << "MplayerControl::checkMplayer: does not exist at \"" + m_MplayerAddress + "\" (process crashed)";
         m_ErrorLog = m_Process->readAll();
         emit mplayerCrashed(&m_ErrorLog);
      }
      else
      {
         m_MplayerExists = false;
         QString m_MplayerAddress = QStandardPaths::findExecutable("mplayer", QStringList(":/mplayer/"));
         m_Process->setProgram(m_MplayerAddress);
         m_Process->start();
      }
   }
}





/// ------------------------------------------- Private -------------------------------------------
void MplayerControl::startChecking()
{
   qDebug() << "MplayerControl::startChecking: " << "looking for mplayer in \"" + m_MplayerAddress + "\"";
   connect(m_Process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(checkMplayer(int, QProcess::ExitStatus)));
   m_MplayerExists = true;
   m_Process->start();
}


