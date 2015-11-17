#include "mencodercontrol.h"

MencoderControl::MencoderControl(QObject *parent)
   :  QObject(parent)
{
   m_RegexPosition.setPattern("Pos:\\s+(\\d+).(\\d+)s\\s+");
   m_Process = new QProcess(parent);
   m_Process->setProgram("mencoder");
   connect(m_Process, SIGNAL(readyRead()), this, SLOT(readStdOutErr()));
   connect(m_Process, SIGNAL(finished(int)), this, SLOT(processFinished(int)));
}





/// ------------------------------------------ Set Functions -------------------------------------------





void MencoderControl::setFiles(const QStringList &files)
{
   m_FilesList = files;
}




/// ------------------------------------------ Other Functions -------------------------------------------




void MencoderControl::startConversion()
{
   qDebug() << "MencoderControl::startConversion: ";
   if(m_FilesList.isEmpty())
      return;
   QStringList args;
   args
         << m_FilesList.first()
         << "-sub"
         << m_FilesList.at(1)
         << "-o"
         << m_FilesList.last()
         << "-oac"
         << "mp3lame"
         << "-ovc"
         << "x264"
         << "-ss"
         << "00:15:00"
         << "-endpos"
         << "15"
            ;

   m_Process->setArguments(args);

   QString str = "mencoder";
   foreach(QString s, args)
      str += " " + s;
   qDebug() << "### command: ### " << str << " ###";
   m_Process->start();
}






/// ------------------------------------------ Private Slots -------------------------------------------





void MencoderControl::readStdOutErr()
{
   QString log = m_Process->readAll();
   if(log.contains(m_RegexPosition))
   {
      emit positionChanged(m_RegexPosition.cap(1).toInt());
   }
   emit logRead(log);
}


void MencoderControl::processFinished(int exitCode)
{
   emit finished(exitCode);
}
