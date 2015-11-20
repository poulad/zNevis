#include "mplayercontrol.h"

MplayerControl::MplayerControl(const QString &mplayerAddress, const QString &videoAddress, QObject *parent)
   : QObject(parent) , m_VideoAddress(videoAddress), m_MplayerAddress(mplayerAddress)
{
   qDebug() << "MplayerControl::MplayerControl: ";

   m_VideoIdString = new QString();
   m_RegexPosition.setPattern("^A:\\s*\\d+[.]?\\d*\\s+V:\\s*(\\d+)(?:[.,]?(\\d+))\\s+");
   m_RegexPosition.setCaseSensitivity(Qt::CaseInsensitive);
   m_RegexLength = new QRegExp("ID_LENGTH=(\\d+)(?:[,.](\\d+))?", Qt::CaseInsensitive);

   m_SubtitleAddress.clear();
   m_WinId.clear();
   updateArgsList();

   m_Process = 0;
   m_IdentifyProcess = 0;
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
   m_Process->setProgram(address);
}


void MplayerControl::setWinId(const QString &winId)
{
   m_WinId = winId;
   updateArgsList();
}


void MplayerControl::setVideoFile(const QString &address)
{
   qDebug() << "MplayerControl::setVideoFile: ";
   quit();
   m_VideoAddress = address;
   updateArgsList();
}


void MplayerControl::setSubtitleFile(const QString &address)
{
   qDebug() << "MplayerControl::setSubtitleFile: " << address;
   m_SubtitleAddress = address;
   updateArgsList();
   if(m_Process->isWritable() == true)
   {
      updateSubtitle();
   }
}




/// ------------------------------------- Other Public Functions ------------------------------------




void MplayerControl::identifyVideo()
{
   if(m_IdentifyProcess == 0)
   {
      m_IdentifyProcess = new QProcess(this);
      m_IdentifyProcess->setProgram(m_MplayerAddress);
   }
   QStringList identifyArgs;
   identifyArgs
         << "-identify"
         << "-endpos"
         << "0"
         << m_VideoAddress
            ;
   m_IdentifyProcess->setArguments(identifyArgs);
   connect(m_IdentifyProcess, SIGNAL(finished(int)), this, SLOT(readVideoId()));
   m_IdentifyProcess->start(QIODevice::ReadOnly);
}




/// ------------------------------------------- Public Slots -------------------------------------------





void MplayerControl::playPause()
{
   qDebug() << "MplayerControl::playPause: ";
   if( m_Process == 0 )
   {
      m_Process = new QProcess(this);
      m_Process->setProgram( m_MplayerAddress );
      m_Process->setArguments(m_ArgsList);
      QString str = m_MplayerAddress;
      foreach(QString s, m_ArgsList)
         str += " " + s;
      qDebug() << str;
      emit mplayerStdOutErr( str );
      connect(m_Process, SIGNAL(readyReadStandardOutput()), this, SLOT(readStdOutErr()));
      connect(m_Process, SIGNAL(readyReadStandardError()), this, SLOT(readStderr()));
      m_Process->start(QIODevice::ReadWrite);
      m_IsPlaying = true;
   }
   else
   {
      m_Command = "p";
      sendCommand();
      //m_IsPlaying = !m_IsPlaying;
   }
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

   if( stderrString.contains(m_RegexPosition) )
   {
         qDebug() << stderrString;
      quint64 sec = m_RegexPosition.cap(1).toInt();
      bool ok = true;
      quint64 msec = m_RegexPosition.cap(2).toInt(&ok);
      if(ok == false)
         msec = 0;
      emit positionChanged( sec, msec );
   }

}


void MplayerControl::readStdOutErr()
{
   QString str = m_Process->readAll();
   qDebug() << "MplayerControl::readStdOutErr: ";

   emit mplayerStdOutErr(str);
}


void MplayerControl::updateSubtitle()
{
   qDebug() << "MplayerControl::updateSubtitle: ";

   m_Command = "sub_remove 0\r\n";
   sendCommand();
   m_Command = "sub_file -1\r\n";
   sendCommand();
   m_Command = "sub_load " + m_SubtitleAddress + "\r\n";
   sendCommand();
   m_Command = "sub_select 0\r\n";
   sendCommand();
}


void MplayerControl::sendCommand()
{
   m_Command += "\r\n";
   m_Process->write(QByteArray(m_Command.toLatin1()));
}


void MplayerControl::updateArgsList()
{
   m_ArgsList.clear();
   m_ArgsList
         << m_VideoAddress
         << "-slave"
         << "-osdlevel"
         << "0"
            ;

   if(m_SubtitleAddress.isEmpty() == false)
      m_ArgsList
            << "-sub"
            << m_SubtitleAddress
               ;

   if(m_WinId.isEmpty() == false)
      m_ArgsList
            << "-wid"
            << m_WinId
               ;
   qDebug() << "MplayerControl::updateArgsList: " << m_ArgsList;
}


void MplayerControl::volume(int v)
{
   m_Command = "set_property volume " + QString::number(v);
   qDebug() << m_Command;
   sendCommand();
}


void MplayerControl::readVideoId()
{
   qDebug() << "MplayerControl::readVideoId: ";
   m_VideoIdList.clear();
   QString stdOutErr = m_IdentifyProcess->readAll();
   QStringList list = stdOutErr.split("\n");
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
   emit videoIdChanged(m_VideoIdList);
   m_IdentifyProcess->close();
   delete m_IdentifyProcess;
   m_IdentifyProcess = 0;
}


void MplayerControl::fullScreen()
{
   m_Command = "f";
   sendCommand();
}








