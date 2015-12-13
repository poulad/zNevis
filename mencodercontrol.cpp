#include "mencodercontrol.h"

MencoderControl::MencoderControl(QObject *parent)
   :  QObject(parent)
{
   m_RegexPosition.setPattern("Pos:\\s+(\\d+).(\\d+)s\\s+");
   m_StartTime = 0;
   m_EndTime = 0;

   m_Process = new QProcess(parent);
   connect(m_Process, SIGNAL(readyRead()), this, SLOT(readStdOutErr()));
   connect(m_Process, SIGNAL(finished(int)), this, SLOT(processFinished(int)));
}


MencoderControl::~MencoderControl()
{
   delete m_StartTime;
   delete m_EndTime;
}





/// ------------------------------------------ Set Functions -------------------------------------------





void MencoderControl::setMencoderAddress(const QString &address)
{
   m_MencoderAddress = address;
}


void MencoderControl::setInputMedia(const QString &input)
{
   m_Input = input;
}


void MencoderControl::setSubtitle(const QString &address)
{
   m_Subtitle = address;
}


void MencoderControl::setOutput(const QString &output)
{
   m_Output = output;
}


void MencoderControl::setClipTime(const QTime &startTime, const QTime &endTime)
{
   m_StartTime = new QTime();
   m_EndTime = new QTime();

   *m_StartTime = startTime;
   *m_EndTime = endTime;
}





/// ------------------------------------------ Other Functions -------------------------------------------




void MencoderControl::startConversion()
{
   qDebug() << "MencoderControl::startConversion: ";
   if(m_Input.isEmpty() || m_Output.isEmpty())
      return;

   updateArgs();
   m_Process->setProgram(m_MencoderAddress);
   m_Process->setArguments(m_Args);

   qDebug() << "### command: ### " << m_Process->program() << " " << m_Args.join(' ') << " ###";
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







/// ------------------------------------------ Private -------------------------------------------






void MencoderControl::updateArgs()
{
   m_Args.clear();
   m_Args
         << m_Input
         << "-o"
         << m_Output
         << "-oac"
         << "mp3lame"
         << "-ovc"
         << "x264"
            ;
   if(m_Subtitle.isEmpty() == false)
   {
      m_Args << "-sub" << m_Subtitle << "-ass";
   }
   if(m_StartTime != 0)
   {
      m_Args << "-ss" << m_StartTime->toString("hh:mm:ss.zz");
   }
   if(m_EndTime != 0)
   {
      m_Args << "-endpos" << m_EndTime->toString("hh:mm:ss.zz");
   }
}
