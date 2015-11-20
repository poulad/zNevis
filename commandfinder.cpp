#include "commandfinder.h"

commandFinder::commandFinder(QObject *parent) : QObject(parent)
{

}

/*
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

/// ------------------------------------------- Private -------------------------------------------
void MplayerControl::startChecking()
{
   qDebug() << "MplayerControl::startChecking: " << "looking for mplayer in \"" + m_MplayerAddress + "\"";
   connect(m_Process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(checkMplayer(int, QProcess::ExitStatus)));
   m_MplayerExists = true;
   m_Process->start();
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

*/
