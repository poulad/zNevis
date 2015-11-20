#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QWindow>
#include <QColor>
#include <QFile>
#include <QFileDialog>
#include <QStandardPaths>
#include <QTextStream>




MainWindow::MainWindow(QWidget *parent) :
   QMainWindow(parent),
   ui(new Ui::MainWindow)
{
   qDebug() << "MainWindow::MainWindow: ";
   ui->setupUi(this);
   m_Dir = new QDir( QStandardPaths::writableLocation(QStandardPaths::MoviesLocation ) );
   m_Subtitle = 0;
   m_MplayerControl = 0;
   m_OpenSubtitleFileDialog = 0;
   m_OpenVideoFileDialog = 0;
   m_ColorDialog = 0;

   ///set Time Spin Boxes to change SECONDS
   ui->showTimeEdit->setCurrentSectionIndex(2);
   ui->hideTimeEdit->setCurrentSectionIndex(2);
   ui->durationTimeEdit->setCurrentSectionIndex(2);

   /// CONNECTIONS:
   ui->dockWidget->hide();
   ui->toolBar->setEnabled(true);
   setEnabled(true);




   /*
   m_MplayerControl = new MplayerControl(this);
   connect(m_MplayerControl, SIGNAL(mplayerCrashed(QString*)), this, SLOT(findMplayer(QString*)));
   connect(m_MplayerControl, SIGNAL(mplayerExists()), this, SLOT(createGui()));
   m_MplayerControl->locateMplayer(); */
}


MainWindow::~MainWindow()
{
   qDebug() << "MainWindow::~MainWindow: ";
   //m_LogDialog.close();
   delete m_Subtitle;
   delete ui;
}


void MainWindow::on_actionNewSubtitle_triggered()
{
   qDebug() << "MainWindow::on_actionNewSubtitle_triggered: ";
   if(m_Subtitle == 0) // first time
   {
      m_Subtitle = new Subtitle();
      m_SubtitleFile.setFileName(m_Subtitle->fileName());
   }
   else
   {
      disconnect(ui->tableWidget, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(changeLineProperties(int,int,int,int)));
      disconnect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(updateText()));
      disconnect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(updateFont()));
      disconnect(ui->sizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateFont()));
      disconnect(ui->boldCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
      disconnect(ui->italicCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
      disconnect(ui->strikeOutCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
      disconnect(ui->underlineCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
      disconnect(ui->showTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateShowTime(QTime)));
      disconnect(ui->hideTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateHideTime(QTime)));
      disconnect(ui->durationTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateDurationTime(QTime)));

      QFont font = ui->fontComboBox->currentFont();
      font.setPointSize(ui->sizeSpinBox->value());
      font.setBold(ui->boldCheckBox->isChecked());
      font.setItalic(ui->italicCheckBox->isChecked());
      font.setStrikeOut(ui->strikeOutCheckBox->isChecked());
      font.setUnderline(ui->underlineCheckBox->isChecked());
      m_Subtitle->setDefaultFont(font);
      m_Subtitle->setDefaultColor(ui->textEdit->textColor());

      ui->tableWidget->clear();
      m_Subtitle->removeAll();
   }
   m_MplayerControl->setSubtitleFile(m_Subtitle->fileName());
   importSubtitle();
   enableSubtitleWidgets(true);
}


void MainWindow::on_actionOpenSubtitle_triggered()
{
   qDebug() << "MainWindow::on_actionOpenSubtitle_triggered: ";
   if(m_OpenSubtitleFileDialog == 0)
   {
      m_OpenSubtitleFileDialog = new QFileDialog(this);
      m_OpenSubtitleFileDialog->setFileMode(QFileDialog::ExistingFile);
      m_OpenSubtitleFileDialog->setNameFilter("Sub-Rip subtitle files (*.srt)\nAll files(*.*)");
   }
   m_OpenSubtitleFileDialog->setDirectory(*m_Dir);
   if(m_OpenSubtitleFileDialog->exec())
   {
      qDebug() << "MainWindow::" << "openSubtitle: " << "Selected File to open:" + m_OpenSubtitleFileDialog->selectedFiles().first();
      m_SubtitleFile.setFileName( m_OpenSubtitleFileDialog->selectedFiles().first() );
      if(m_Subtitle == 0)
      {
         m_Subtitle = new Subtitle( m_SubtitleFile.fileName(), QTextCodec::codecForName("utf8") );
      }
      else
      {
         disconnect(ui->tableWidget, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(changeLineProperties(int,int,int,int)));
         disconnect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(updateText()));
         disconnect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(updateFont()));
         disconnect(ui->sizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateFont()));
         disconnect(ui->boldCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
         disconnect(ui->italicCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
         disconnect(ui->strikeOutCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
         disconnect(ui->underlineCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
         disconnect(ui->showTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateShowTime(QTime)));
         disconnect(ui->hideTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateHideTime(QTime)));
         disconnect(ui->durationTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateDurationTime(QTime)));
         ui->tableWidget->clear();

         m_Subtitle->removeAll();
         m_Subtitle->setFileAddress(m_SubtitleFile.fileName());
      }
      importSubtitle();
   }
   *m_Dir = m_OpenSubtitleFileDialog->directory();
}


void MainWindow::importSubtitle()
{
   qDebug() << "MainWindow::importSubtitle: ";
   ui->tableWidget->clearContents();
   ui->tableWidget->setRowCount( m_Subtitle->lineCount() );
   ui->tableWidget->setColumnCount( 4 );
   for(quint64 i =0; i < m_Subtitle->lineCount(); ++i)
   {
      m_Subtitle->setCurrentLine(i+1);
      ui->tableWidget->setItem(i, 0, new QTableWidgetItem(m_Subtitle->showTime().toString("hh:mm:ss,zzz")) );
      ui->tableWidget->setItem(i, 1, new QTableWidgetItem(m_Subtitle->hideTime().toString("hh:mm:ss,zzz")) );
      ui->tableWidget->setItem(i , 2, new QTableWidgetItem( m_Subtitle->durationTime().toString("hh:mm:ss,zzz")) );
      QTableWidgetItem *lineTextItem = new QTableWidgetItem(m_Subtitle->text());
      lineTextItem->setTextColor( m_Subtitle->color() );
      lineTextItem->setFont( m_Subtitle->font() );
      ui->tableWidget->setItem(i, 3, lineTextItem );
   }
   ui->tableWidget->setColumnWidth(3, 700);

   //LOADING properties of FIRST line
   m_Subtitle->setCurrentLine(1);

   m_Text = &m_Subtitle->text();
   m_Font = &m_Subtitle->font();
   m_Color = &m_Subtitle->color();
   m_ShowTime =  &m_Subtitle->showTime();
   m_HideTime =  &m_Subtitle->hideTime();
   m_DurationTime =  &m_Subtitle->durationTime();

   ui->tableWidget->setCurrentCell(0, 3);
   ui->tableWidget->currentItem()->setTextColor( *m_Color );
   ui->tableWidget->currentItem()->setFont( *m_Font );
   ui->textEdit->setTextColor(*m_Color );
   ui->textEdit->setFont( *m_Font );
   ui->textEdit->setText(*m_Text);
   ui->boldCheckBox->setChecked( m_Font->bold() );
   ui->sizeSpinBox->setValue( m_Font->pointSize());
   ui->showTimeEdit->setTime( *m_ShowTime );
   ui->hideTimeEdit->setTime( *m_HideTime );
   ui->durationTimeEdit->setTime( *m_DurationTime );
   ui->fontComboBox->setCurrentFont( *m_Font );

   connect(ui->tableWidget, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(changeLineProperties(int,int,int,int)));
   connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(updateText()));
   connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(updateFont()));
   connect(ui->sizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateFont()));
   connect(ui->boldCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
   connect(ui->italicCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
   connect(ui->strikeOutCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
   connect(ui->underlineCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
   connect(ui->showTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateShowTime(QTime)));
   connect(ui->hideTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateHideTime(QTime)));
   connect(ui->durationTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateDurationTime(QTime)));
}


void MainWindow::changeLineProperties(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
   qDebug() << "MainWindow::" << "changeLineProperties: " << "lineNumber=" << QString::number(currentRow + 1);
   if( currentRow == previousRow )
   {
      qDebug() << "item is on the same row";
      return;
   }

   ui->tableWidget->setCurrentCell(currentRow, 3); //this is for updateFont() and updateColor() and other ...

   currentColumn = previousColumn + currentColumn;// just to skip Unused warning

   qDebug() <<"changeLineProperties# " << "lineNumber=" <<  "LOADING properties of later selected line#" + QString::number( m_Subtitle->getCurrentLine() );
   disconnect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(updateText()));
   disconnect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(updateFont()));
   disconnect(ui->sizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateFont()));
   disconnect(ui->boldCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
   disconnect(ui->italicCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
   disconnect(ui->strikeOutCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
   disconnect(ui->underlineCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
   disconnect(ui->showTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateShowTime(QTime)));
   disconnect(ui->hideTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateHideTime(QTime)));
   disconnect(ui->durationTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateDurationTime(QTime)));

   m_Subtitle->setCurrentLine(currentRow+1);

   m_Color = &m_Subtitle->color();
   ui->textEdit->setTextColor( *m_Color );
   ui->tableWidget->currentItem()->setTextColor(*m_Color);

   m_Text = &m_Subtitle->text();
   ui->textEdit->setPlainText(*m_Text);
   ui->tableWidget->currentItem()->setText( *m_Text );

   m_Font = &m_Subtitle->font();
   ui->fontComboBox->setCurrentFont(*m_Font);
   ui->sizeSpinBox->setValue(m_Font->pointSize());
   ui->boldCheckBox->setChecked(m_Font->bold());
   ui->italicCheckBox->setChecked(m_Font->italic());
   ui->strikeOutCheckBox->setChecked(m_Font->strikeOut());
   ui->underlineCheckBox->setChecked(m_Font->underline());
   ui->textEdit->setFont(*m_Font);
   ui->tableWidget->currentItem()->setFont(*m_Font);

   m_ShowTime = &m_Subtitle->showTime();
   m_HideTime = &m_Subtitle->hideTime();
   m_DurationTime = &m_Subtitle->durationTime();
   ui->showTimeEdit->setTime( *m_ShowTime );
   ui->hideTimeEdit->setTime( *m_HideTime );
   ui->durationTimeEdit->setTime( *m_DurationTime );

   connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(updateText()));
   connect(ui->fontComboBox, SIGNAL(currentFontChanged(QFont)), this, SLOT(updateFont()));
   connect(ui->sizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateFont()));
   connect(ui->boldCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
   connect(ui->italicCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
   connect(ui->strikeOutCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
   connect(ui->underlineCheckBox, SIGNAL(toggled(bool)), this, SLOT(updateFont()));
   connect(ui->showTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateShowTime(QTime)));
   connect(ui->hideTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateHideTime(QTime)));
   connect(ui->durationTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateDurationTime(QTime)));
}


void MainWindow::updateText()
{
   const QString &newText = ui->textEdit->toPlainText();
   if( newText == *m_Text )
      return;
   qDebug() << "MainWindow::" << "updateText: ";
   *m_Text = ui->textEdit->toPlainText();
   ui->tableWidget->item(ui->tableWidget->currentRow(), 3)->setText( *m_Text );
   m_Subtitle->updateLine();
   m_MplayerControl->pause();
   m_MplayerControl->updateSubtitle();
   m_MplayerControl->seekTo(m_VideoPosition->msecsSinceStartOfDay());
}


void MainWindow::updateFont()
{
   qDebug() << "updateFont# ";
   m_Font->setFamily(ui->fontComboBox->currentFont().family());
   m_Font->setPointSize(ui->sizeSpinBox->value());
   m_Font->setBold(ui->boldCheckBox->isChecked());
   m_Font->setItalic(ui->italicCheckBox->isChecked());
   m_Font->setStrikeOut(ui->strikeOutCheckBox->isChecked());
   m_Font->setUnderline(ui->underlineCheckBox->isChecked());

   ui->textEdit->setFont( *m_Font );
   ui->tableWidget->currentItem()->setFont(*m_Font);
   m_Subtitle->updateLine( ui->tableWidget->currentRow()+1 );

   m_MplayerControl->pause();
   m_MplayerControl->updateSubtitle();
   m_MplayerControl->seekTo(m_VideoPosition->msecsSinceStartOfDay());
}


void MainWindow::updateShowTime(QTime newShowTime)
{
   qDebug() << "updateShowTime# ";
   if( newShowTime == *m_ShowTime )
      return;
   /// ShowTime changes
   /// HideTime does not change
   /// DurationTime changes
   if( newShowTime <= *m_HideTime)
   {
      m_ShowTime->setHMS(newShowTime.hour(), newShowTime.minute(), newShowTime.second(), newShowTime.msec());
      *m_DurationTime = QTime::fromMSecsSinceStartOfDay( m_HideTime->msecsSinceStartOfDay() - m_ShowTime->msecsSinceStartOfDay() );
   }
   else
   {
      ui->showTimeEdit->setTime(*m_HideTime);
      m_DurationTime->setHMS(0,0,0,0);
   }
   ui->durationTimeEdit->setTime(*m_DurationTime);
   ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->setText( m_ShowTime->toString("hh:mm:ss,zzz") );
   ui->tableWidget->item( ui->tableWidget->currentRow(), 2 )->setText( m_DurationTime->toString("hh:mm:ss,zzz") );
   m_Subtitle->updateLine();
   m_MplayerControl->pause();
   m_MplayerControl->updateSubtitle();
   m_MplayerControl->seekTo(m_VideoPosition->msecsSinceStartOfDay());
   qDebug() << "showTime=" << m_ShowTime->toString("hh:mm:ss,zzz");
   qDebug() << "durationTime=" << m_DurationTime->toString("hh:mm:ss,zzz");
}


void MainWindow::updateHideTime(QTime newHideTime)
{
   qDebug() << "updateHideTime# ";
   if( newHideTime == *m_HideTime )
      return;
   /// ShowTime does not change
   /// HideTime changes
   /// DurationTime changes
   if( *m_ShowTime <= newHideTime )
   {
      m_HideTime->setHMS(newHideTime.hour(), newHideTime.minute(), newHideTime.second(), newHideTime.msec());
      *m_DurationTime = QTime::fromMSecsSinceStartOfDay( m_HideTime->msecsSinceStartOfDay() - m_ShowTime->msecsSinceStartOfDay() );
   }
   else
   {
      ui->hideTimeEdit->setTime(*m_ShowTime);
      m_DurationTime->setHMS(0,0,0,0);
   }
   ui->durationTimeEdit->setTime(*m_DurationTime);
   ui->tableWidget->item(ui->tableWidget->currentRow(), 1)->setText( m_HideTime->toString("hh:mm:ss,zzz") );
   ui->tableWidget->item(ui->tableWidget->currentRow(), 2)->setText( m_DurationTime->toString("hh:mm:ss,zzz") );
   m_Subtitle->updateLine();
   m_MplayerControl->pause();
   m_MplayerControl->updateSubtitle();
   m_MplayerControl->seekTo(m_VideoPosition->msecsSinceStartOfDay());
   qDebug() << "hideTime=" << m_HideTime->toString("hh:mm:ss,zzz");
   qDebug() << "durationTime=" << m_DurationTime->toString("hh:mm:ss,zzz");
}


void MainWindow::updateDurationTime(QTime newDurationTime)
{
   qDebug() << "updateDurationTime# ";
   if ( newDurationTime == *m_DurationTime )
      return;
   /// ShowTime does not change
   /// HideTime changes
   /// DurationTime changes
   m_DurationTime->setHMS(newDurationTime.hour(), newDurationTime.minute(), newDurationTime.second(), newDurationTime.msec());
   QTime newHideTime( m_ShowTime->addMSecs( m_DurationTime->msecsSinceStartOfDay() )   );
   m_HideTime->setHMS( newHideTime.hour(), newHideTime.minute(), newHideTime.second(), newHideTime.msec() );

   ui->hideTimeEdit->setTime(*m_HideTime);
   ui->tableWidget->item(ui->tableWidget->currentRow(), 2)->setText( m_DurationTime->toString("hh:mm:ss,zzz") );
   ui->tableWidget->item( ui->tableWidget->currentRow(), 1 )->setText( m_HideTime->toString("hh:mm:ss,zzz") );
   m_Subtitle->updateLine();
   m_MplayerControl->pause();
   m_MplayerControl->updateSubtitle();
   m_MplayerControl->seekTo(m_VideoPosition->msecsSinceStartOfDay());
   qDebug() << "durationTime=" << m_DurationTime->toString("hh:mm:ss,zzz");
   qDebug() << "hideTime=" << m_HideTime->toString("hh:mm:ss,zzz");
}


void MainWindow::on_actionConvert_triggered()
{
   m_MplayerControl->pause();
   QStringList files;
   files
         << m_VideoFile.fileName()
         << m_SubtitleFile.fileName()
         << "/home/yuzer/Videos/av/yy.mp4"
            ;
   m_ConvertDialog = new ConvertDialog(files, 15, this);
   m_ConvertDialog->exec();
}


void MainWindow::playPreviousLine()
{
   ui->tableWidget->setCurrentCell(ui->tableWidget->currentRow()-1, 3);
   m_MplayerControl->seekTo(m_ShowTime->msecsSinceStartOfDay());
}


void MainWindow::playNextLine()
{
   ui->tableWidget->setCurrentCell(ui->tableWidget->currentRow()+1, 3);
   m_MplayerControl->seekTo(m_ShowTime->msecsSinceStartOfDay());
}


void MainWindow::setVideoPosition(quint64 sec, quint64 msec)
{
   qDebug() << "MainWindow::setVideoPosition: ";
   *m_VideoPosition = QTime::fromMSecsSinceStartOfDay(sec*1000 + msec );
   ui->positionSlider->setValue( sec*1000 + msec  );
   ui->positionLabel->setText( m_VideoPosition->toString("hh:mm:ss,zzz") );
   //qDebug() << "## " << m_VideoPosition->toString("hh:mm:ss,zzz") << " ##";
   ui->positionSlider->setValue( m_VideoPosition->msecsSinceStartOfDay() );
}


void MainWindow::seekSlider(int msec)
{
   msec =  ui->positionSlider->value();
   m_MplayerControl->seekTo(msec);
}




/// ------------------------------------------- Private Slots -------------------------------------------




void MainWindow::findMplayer(QString *errorLog)
{
   qDebug() << "MainWindow::findMplayer ";
   if(m_MplayerExists == false)
   {
      if (QMessageBox::critical(this, "mplayer was not found" , "Application failed to run mplayer.\nDo you want to see logs?",
                                QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
      {
         //m_LogDialog->exec();
      }
      close();
      qApp->exit(1);
   }
   else
   {
      //m_LogDialog = new LogDialog(this);
      m_MplayerFileDialog = new QFileDialog(this);
      hide();
      m_MplayerExists = false;
      setEnabled(false);
      //m_LogDialog->appendMplayerLog(*errorLog);
      //m_LogDialog->show();
      //ui->check the view menu->log
      m_MplayerFileDialog->setDirectory( QStandardPaths::writableLocation( QStandardPaths::ApplicationsLocation) );
      m_MplayerFileDialog->setFileMode(QFileDialog::ExistingFile);
      m_MplayerFileDialog->setOptions(QFileDialog::ReadOnly | QFileDialog::HideNameFilterDetails);
      m_MplayerFileDialog->setNameFilter("Any files (*)");
      if ( m_MplayerFileDialog->exec() )
      {
         m_MplayerControl->setMplayerAddress(m_MplayerFileDialog->selectedFiles().first());
      }
   }
}


void MainWindow::on_actionOpenVideo_triggered()
{
   qDebug() << "MainWindow::on_actionOpenVideo_triggered: ";
   if(m_OpenVideoFileDialog == 0)
   {
      m_OpenVideoFileDialog = new QFileDialog(this);
      m_OpenVideoFileDialog->setFileMode(QFileDialog::ExistingFile);
      m_OpenVideoFileDialog->setWindowTitle("Open video");
      m_OpenVideoFileDialog->setNameFilter("OOG, MP4, MKV (*.ogg *.mp4 *.mkv)\nAll Files (*.*)");
   }
   m_OpenVideoFileDialog->setDirectory( *m_Dir );
   if( m_OpenVideoFileDialog->exec() )
   {
      setEnabled(false);
      m_VideoFile.setFileName( m_OpenVideoFileDialog->selectedFiles().first() );
      if(m_MplayerControl == 0)
      {
         m_MplayerControl = new MplayerControl("mplayer", m_VideoFile.fileName(), this);
         m_MplayerControl->setWinId( QString::number( ui->mplayerWidget->winId() ) );
      }
      else
      {
         m_MplayerControl->setVideoFile(m_VideoFile.fileName());
      }
      //m_LogDialog.show();
      //connect(m_MplayerControl, SIGNAL(mplayerStdOutErr(const QString&)), this, SLOT(readLog(QString)));
      connect(m_MplayerControl, SIGNAL(videoIdChanged(QStringList&)), this, SLOT(importVideoId(QStringList &)));
      connect(ui->playPauseButton, SIGNAL(clicked(bool)), m_MplayerControl, SLOT(playPause()));
      connect(ui->volumeSlider, SIGNAL(valueChanged(int)), m_MplayerControl, SLOT(volume(int)));
      connect(m_MplayerControl, SIGNAL(positionChanged(quint64,quint64)), this, SLOT(setVideoPosition(quint64, quint64)));
      if(m_SubtitleFile.exists())
         m_MplayerControl->setSubtitleFile(m_Subtitle->fileName());
      m_MplayerControl->identifyVideo();
   }
   *m_Dir = m_OpenVideoFileDialog->directory();
}


void MainWindow::importVideoId(QStringList &videoIdList)
{
   qDebug() << "MainWindow::importVideoId: ";

   QString id;
   for(int i = 0; i < videoIdList.size(); i +=2)
   {
      id = videoIdList[i];
      if(id == "LENGTH")
      {
         qDebug() << videoIdList.at(i+1);
         QStringList lengthList = videoIdList[i+1].split('.');
         quint64 sec = lengthList.first().toInt() * 1000;
         bool ok = true;
         quint64 msec = lengthList.last().toInt(&ok);
         if(ok == false)
            msec = 0;
         quint64 length = sec + msec;
         qDebug() << QString::number(length);
         QTime lengthTime;
         lengthTime = QTime::fromMSecsSinceStartOfDay(length);
         ui->positionSlider->setRange(0, length);
         ui->lengthLabel->setText(lengthTime.toString("hh:mm:ss,zzz"));
         ui->videoLengthLabel->setText( lengthTime.toString("hh:mm:ss,zzz") );
      }
      else if(id == "VIDEO_FORMAT")
      {
         ui->videoFormatLabel->setText(videoIdList[i+1]);
      }
      else if(id == "VIDEO_WIDTH")
      {
         m_VideoWidth = videoIdList[i+1].toInt();
      }
      else if(id == "VIDEO_HEIGHT")
      {
         m_VideoHeight = videoIdList[i+1].toInt();
      }
      else if(id == "VIDEO_FPS")
      {
         ui->videoFpsLabel->setText(videoIdList[i+1]);
      }
      else if(id == "VIDEO_ASPECT")
      {
         ui->videoAspectLabel->setText(videoIdList[i+1]);
      }
      else if(id == "AUDIO_FORMAT")
      {
         ui->audioFormatLabel->setText(videoIdList[i+1]);
      }
      else if(id == "AUDIO_BITRATE")
      {
         ui->audioBitrateLabel->setText(videoIdList[i+1]);
      }
      else if(id == "VIDEO_CODEC")
      {
         ui->videoCodecLabel->setText(videoIdList[i+1]);
      }
      else if(id == "AUDIO_CODEC")
      {
         ui->audioCodecLabel->setText(videoIdList[i+1]);
      }
   }
   ui->videoSizeLabel->setText( QString::number(m_VideoWidth) + "x" + QString::number(m_VideoHeight) );
   m_VideoPosition = new QTime();

   ui->dockWidget->show();
   setEnabled(true);
}


void MainWindow::on_positionSlider_sliderPressed()
{
   m_MplayerControl->pause();
   disconnect(m_MplayerControl, SIGNAL(positionChanged(quint64,quint64)), this, SLOT(setVideoPosition(quint64, quint64)));
   connect(ui->positionSlider, SIGNAL(valueChanged(int)), this, SLOT(seekSlider(int)));
}


void MainWindow::on_positionSlider_sliderReleased()
{
   disconnect(ui->positionSlider, SIGNAL(valueChanged(int)), this, SLOT(seekSlider(int)));
   connect(m_MplayerControl, SIGNAL(positionChanged(quint64,quint64)), this, SLOT(setVideoPosition(quint64, quint64)));
   m_MplayerControl->playPause();
}


void MainWindow::on_colorButton_clicked()
{
   qDebug() << "MainWidnow::" << "updateColor: ";
   if(m_ColorDialog == 0)
   {
      m_ColorDialog = new QColorDialog(this);
      m_ColorDialog->setWindowTitle("Select Color");
   }
   m_ColorDialog->setCurrentColor(*m_Color);
   if(m_ColorDialog->exec() == QDialog::Accepted)
   {
      *m_Color = m_ColorDialog->selectedColor();
      ui->textEdit->setTextColor(*m_Color);
      QString text = ui->textEdit->toPlainText();
      ui->textEdit->setPlainText("Plain Text");
      ui->textEdit->setPlainText(text);
      ui->tableWidget->currentItem()->setTextColor(*m_Color);
      m_Subtitle->updateLine();
      m_MplayerControl->pause();
      m_MplayerControl->updateSubtitle();
      m_MplayerControl->seekTo(m_VideoPosition->msecsSinceStartOfDay());
   }
}


void MainWindow::readLog(QString log)
{
   //m_LogDialog.append(log);
}


void MainWindow::enableSubtitleWidgets(bool b)
{
   ui->tableWidget->setEnabled(b);
   ui->showLabel->setEnabled(b);
   ui->showTimeEdit->setEnabled(b);
   ui->hideLabel->setEnabled(b);
   ui->hideTimeEdit->setEnabled(b);
   ui->durationLabel->setEnabled(b);
   ui->durationTimeEdit->setEnabled(b);
   ui->fontLabel->setEnabled(b);
   ui->fontComboBox->setEnabled(b);
   ui->sizeLabel->setEnabled(b);
   ui->sizeSpinBox->setEnabled(b);
   ui->boldCheckBox->setEnabled(b);
   ui->italicCheckBox->setEnabled(b);
   ui->underlineCheckBox->setEnabled(b);
   ui->strikeOutCheckBox->setEnabled(b);
   ui->colorButton->setEnabled(b);
   ui->textEdit->setEnabled(b);
}





