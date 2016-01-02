#include "mencodercontrol.h"

MEncoderControl::MEncoderControl(QObject *parent)
   :  QObject(parent)
{
   m_RegexPosition.setPattern("Pos:\\s*(\\d+).(\\d+)s\\s*");
   m_RegexPosition.setCaseSensitivity(Qt::CaseInsensitive);
   m_StartTime = 0;
   m_EndTime = 0;

   m_Process = new QProcess(parent);
   connect(m_Process, SIGNAL(readyRead()), this, SLOT(readStdOutErr()));
   connect(m_Process, SIGNAL(finished(int)), this, SLOT(processFinished(int)));
}


MEncoderControl::~MEncoderControl()
{
   delete m_StartTime;
   delete m_EndTime;
}





/// ------------------------------------------ Set Functions -------------------------------------------





void MEncoderControl::setMencoderAddress(const QString &address)
{
   m_MEncoderAddress = address;
}


void MEncoderControl::setInputMedia(const QString &input)
{
   m_Input = input;
}


void MEncoderControl::setSubtitle(const QString &address)
{
   m_Subtitle = address;
}


void MEncoderControl::setOutput(const QString &output)
{
   m_Output = output;
}


void MEncoderControl::setClipTime(const QTime &startTime, const QTime &endTime)
{
   m_StartTime = new QTime();
   m_EndTime = new QTime();

   *m_StartTime = startTime;
   *m_EndTime = endTime;
}





/// ------------------------------------------ Other Functions -------------------------------------------




void MEncoderControl::startConversion()
{
   qDebug() << "MencoderControl::startConversion: ";
   if(m_Input.isEmpty() || m_Output.isEmpty() || m_Subtitle.isEmpty() || m_EndTime->msecsSinceStartOfDay() == 0)
      return;

   updateArgs();
   m_Process->setProgram(m_MEncoderAddress);
   m_Process->setArguments(m_Args);

   qDebug() << "### command: ### " << m_Process->program() << " " << m_Args.join(' ') << " ###";
   m_Process->start();
}






/// ------------------------------------------ Private Slots -------------------------------------------





void MEncoderControl::readStdOutErr()
{
   QString log = m_Process->readAll();
   if(log.contains(m_RegexPosition))
   {
      emit positionChanged(m_RegexPosition.cap(1).toInt());
   }
   emit logRead(log);
}


void MEncoderControl::processFinished(int exitCode)
{
   emit finished(exitCode);
}







/// ------------------------------------------ Private -------------------------------------------






void MEncoderControl::updateArgs()
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
