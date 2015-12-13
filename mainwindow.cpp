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
   m_MencoderControl = 0;
   m_OpenSubtitleFileDialog = 0;
   m_SaveOutputFileDialog = 0;
   m_ColorDialog = 0;

   m_mplayerWidget = new mplayerWidget("mplayer", ui->mplayerWidget);

   m_OpenVideoFileDialog = new QFileDialog(this);
   m_OpenVideoFileDialog->setFileMode(QFileDialog::ExistingFile);
   m_OpenVideoFileDialog->setWindowTitle("Open video");
   m_OpenVideoFileDialog->setNameFilter("OOG, MP4, MKV (*.ogg *.mp4 *.mkv)\nAll Files (*.*)");

   ///set TimeEdit Boxes to change SECONDS
   ui->showTimeEdit->setCurrentSectionIndex(2);
   ui->hideTimeEdit->setCurrentSectionIndex(2);

   ui->x1Label->setHidden(true);
   ui->x1SpinBox->setHidden(true);
   ui->y1Label->setHidden(true);
   ui->y1SpinBox->setHidden(true);

   ui->toolbarSubtitle->setVisible(false);
   setEditingItemsEnabled(false);

   /// CONNECTIONS:
   //connect(m_MplayerControl, SIGNAL(mplayerStdOutErr(const QString&)), this, SLOT(readLog(QString)));
   connect(ui->volumeSlider, SIGNAL(valueChanged(int)), m_mplayerWidget, SLOT(volume(int)));
   connect(m_mplayerWidget, SIGNAL(positionChanged(int)), this, SLOT(setVideoPosition(int)));
   connect(m_mplayerWidget, SIGNAL(videoIdChanged(const MediaID*)), this, SLOT(importVideoId(const MediaID*)));
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
      disconnect(ui->showTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateShowTime(QTime)));
      disconnect(ui->hideTimeEdit, SIGNAL(timeChanged(QTime)), this, SLOT(updateHideTime(QTime)));

      QFont font = ui->fontComboBox->currentFont();
      font.setPointSize(ui->sizeSpinBox->value());
      font.setBold(ui->buttonBold->isChecked());
      font.setItalic(ui->buttonItalic->isChecked());
      font.setStrikeOut(ui->buttonStrikeout->isChecked());
      font.setUnderline(ui->buttonUnderline->isChecked());
      m_Subtitle->setDefaultFont(font);
      m_Subtitle->setDefaultColor(ui->textEdit->textColor());

      ui->tableWidget->clear();
      m_Subtitle->removeAll();
   }
   //if(m_MplayerControl != 0)
   //m_MplayerControl->setSubtitleFile(m_Subtitle->fileName());
   importSubtitle();
   //showSubtitleWidgets(true);
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
         ui->tableWidget->clear();

         m_Subtitle->removeAll();
         m_Subtitle->setFileAddress(m_SubtitleFile.fileName());
      }
      importSubtitle();
   }
   *m_Dir = m_OpenSubtitleFileDialog->directory();
}


void MainWindow::importVideoId(const MediaID *mediaID)
{
   qDebug() << "MainWindow::importVideoId ";

   ui->positionSlider->setMaximum(mediaID->length());
   m_VideoLength = QTime::fromMSecsSinceStartOfDay( mediaID->length() * 10);
   m_VideoWidth = mediaID->width();
   m_VideoHeight = mediaID->height();
   ui->lengthLabel->setText(m_VideoLength.toString("h:mm:ss,zzz"));
   ui->clipStartTimeEdit->setTime(QTime(0, 0, 0, 0));
   ui->clipDurationTimeEdit->setTime(m_VideoLength);
   ui->clipEndTimeEdit->setTime(m_VideoLength);
   setEditingItemsEnabled(true);

   ui->audioBitrateLabel->setText(QString::number(mediaID->audioBitrate()));
   ui->audioCodecLabel->setText( mediaID->audioCodec() );
   ui->audioFormatLabel->setText( mediaID->audioFormat() );

   ui->videoAspectLabel->setText( QString::number( mediaID->aspectRatio() ) );
   ui->videoCodecLabel->setText( mediaID->videoCodec() );
   ui->videoFormatLabel->setText( mediaID->videoFormat() );
   ui->videoFpsLabel->setText( QString::number( mediaID->fps() ) );
   ui->videoLengthLabel->setText( ui->lengthLabel->text() );
   ui->videoSizeLabel->setText( QString::number(m_VideoWidth) + " x " + QString::number(m_VideoHeight) );
}


void MainWindow::importSubtitle()
{
   qDebug() << "MainWindow::importSubtitle: ";
   ui->tableWidget->clearContents();
   ui->tableWidget->setRowCount( m_Subtitle->lineCount() );
   ui->tableWidget->setColumnCount( 3 );
   for(quint64 i =0; i < m_Subtitle->lineCount(); ++i)
   {
      m_Subtitle->setCurrentLine(i+1);
      ui->tableWidget->setItem(i, 0, new QTableWidgetItem(m_Subtitle->showTime().toString("h:mm:ss,zz")) );
      ui->tableWidget->setItem(i, 1, new QTableWidgetItem(m_Subtitle->hideTime().toString("h:mm:ss,zz")) );
      QTableWidgetItem *lineTextItem = new QTableWidgetItem(m_Subtitle->text());
      lineTextItem->setTextColor( m_Subtitle->color() );
      lineTextItem->setFont( m_Subtitle->font() );
      ui->tableWidget->setItem(i, 2, lineTextItem );
   }
   ui->tableWidget->setColumnWidth(2, 700);

   //LOADING properties of FIRST line
   m_Subtitle->setCurrentLine(1);

   m_Text = &m_Subtitle->text();
   m_Font = &m_Subtitle->font();
   m_Color = &m_Subtitle->color();
   m_ShowTime =  &m_Subtitle->showTime();
   m_HideTime =  &m_Subtitle->hideTime();

   ui->tableWidget->setCurrentCell(0, 2);
   ui->tableWidget->currentItem()->setTextColor( *m_Color );
   ui->tableWidget->currentItem()->setFont( *m_Font );
   ui->textEdit->setTextColor(*m_Color );
   ui->textEdit->setFont( *m_Font );
   ui->textEdit->setText(*m_Text);
   ui->buttonBold->setChecked( m_Font->bold() );
   ui->sizeSpinBox->setValue( m_Font->pointSize());
   ui->showTimeEdit->setTime( *m_ShowTime );
   ui->hideTimeEdit->setTime( *m_HideTime );
   ui->fontComboBox->setCurrentFont( *m_Font );

   connect(ui->tableWidget, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(changeLineProperties(int,int,int,int)));
   connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(updateText()));
   connect(ui->x1SpinBox, SIGNAL(valueChanged(int)), this, SLOT(updatePosition()));
   connect(ui->y1SpinBox, SIGNAL(valueChanged(int)), this, SLOT(updatePosition()));

}


void MainWindow::changeLineProperties(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
   qDebug() << "MainWindow::" << "changeLineProperties: " << "lineNumber=" << QString::number(currentRow + 1);
   if( currentRow == previousRow )
   {
      qDebug() << "item is on the same row";
      return;
   }

   ui->tableWidget->setCurrentCell(currentRow, 2); //this is for updatingFont() and updateColor() and other ...

   currentColumn = previousColumn + currentColumn;// just to skip Unused warning

   qDebug() <<"changeLineProperties# " << "lineNumber=" <<  "LOADING properties of later selected line#" + QString::number( m_Subtitle->getCurrentLine() );
   disconnect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(updateText()));

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
   ui->buttonBold->setChecked(m_Font->bold());
   ui->buttonItalic->setChecked(m_Font->italic());
   ui->buttonStrikeout->setChecked(m_Font->strikeOut());
   ui->buttonUnderline->setChecked(m_Font->underline());
   ui->textEdit->setFont(*m_Font);
   ui->tableWidget->currentItem()->setFont(*m_Font);

   m_ShowTime = &m_Subtitle->showTime();
   m_HideTime = &m_Subtitle->hideTime();
   ui->showTimeEdit->setTime( *m_ShowTime );
   ui->hideTimeEdit->setTime( *m_HideTime );

   connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(updateText()));
}


void MainWindow::updateText()
{
   const QString &newText = ui->textEdit->toPlainText();
   if( newText == *m_Text )
      return;
   qDebug() << "MainWindow::updateText: ";
   //m_MplayerControl->pause();
   *m_Text = ui->textEdit->toPlainText();
   ui->tableWidget->item(ui->tableWidget->currentRow(), 2)->setText( *m_Text );
}


void MainWindow::updatePosition()
{
   qDebug() << "MainWindow::updatePosition: ";
   m_Subtitle->setPosition(ui->x1SpinBox->value(), ui->y1SpinBox->value());
   m_Subtitle->updateLine();
   m_mplayerWidget->updateSubtitle();
}


void MainWindow::on_actionConvert_triggered()
{
   m_mplayerWidget->pause();
   if(m_MencoderControl == 0)
      m_MencoderControl = new MencoderControl(this);

   m_MencoderControl->setMencoderAddress("mencoder");
   m_MencoderControl->setInputMedia(m_VideoFile.fileName());
   m_MencoderControl->setClipTime( ui->clipStartTimeEdit->time(), ui->clipDurationTimeEdit->time() );
   if(m_SubtitleFile.exists())
      m_MencoderControl->setSubtitle(m_SubtitleFile.fileName());
   m_MencoderControl->setOutput(m_OutputFile.fileName());
   m_ConvertDialog = new ConvertDialog( m_MencoderControl, ui->clipDurationTimeEdit->time().msecsSinceStartOfDay(), this);
   m_ConvertDialog->exec();
}


void MainWindow::on_actionAddText_triggered()
{
   m_Subtitle->appendLine();
   m_Subtitle->save();
   quint64 newLineNumber = m_Subtitle->lineCount();
   ui->tableWidget->setRowCount( newLineNumber );
   m_Subtitle->setCurrentLine(newLineNumber);
   ui->tableWidget->setItem(newLineNumber-1, 0, new QTableWidgetItem(m_Subtitle->showTime().toString("hh:mm:ss,zz")) );
   ui->tableWidget->setItem(newLineNumber-1, 1, new QTableWidgetItem(m_Subtitle->hideTime().toString("hh:mm:ss,zz")) );
   QTableWidgetItem *lineTextItem = new QTableWidgetItem(m_Subtitle->text());
   lineTextItem->setTextColor( m_Subtitle->color() );
   lineTextItem->setFont( m_Subtitle->font() );
   m_Subtitle->setCurrentLine(ui->tableWidget->currentRow()+1);

   ui->tableWidget->setItem(newLineNumber-1, 2, lineTextItem );
   //m_Subtitle->setCurrentLine(currentLine);
   //ui->tableWidget->setCurrentCell( currentLine+1, 2);
}


void MainWindow::playPreviousLine()
{
   ui->tableWidget->setCurrentCell(ui->tableWidget->currentRow()-1, 2);
   m_mplayerWidget->seekTo(m_ShowTime->msecsSinceStartOfDay());
}


void MainWindow::playNextLine()
{
   ui->tableWidget->setCurrentCell(ui->tableWidget->currentRow()+1, 2);
   m_mplayerWidget->seekTo(m_ShowTime->msecsSinceStartOfDay());
}


void MainWindow::setVideoPosition(int deciSec)
{
   //qDebug() << "MainWindow::setVideoPosition: ";
   ui->positionSlider->setValue(deciSec);
   m_VideoPosition = QTime::fromMSecsSinceStartOfDay(deciSec * 10);
   ui->positionLabel->setText( m_VideoPosition.toString("h:mm:ss,zzz") );
}


void MainWindow::seekSlider(int msec)
{
   msec =  ui->positionSlider->value();
   m_mplayerWidget->seekTo(msec);
}




/// ------------------------------------------- Private Slots -------------------------------------------




void MainWindow::on_actionOpenVideo_triggered()
{
   qDebug() << "MainWindow::on_actionOpenVideo_triggered: ";
   m_OpenVideoFileDialog->setDirectory( *m_Dir );
   if( m_OpenVideoFileDialog->exec() )
   {
      setEditingItemsEnabled(false);
      m_VideoFile.setFileName( m_OpenVideoFileDialog->selectedFiles().first() );
      m_mplayerWidget->setVideoFile(m_VideoFile.fileName());

      if(m_SubtitleFile.exists())
         m_mplayerWidget->setSubtitleFile(m_Subtitle->fileName());
   }
   *m_Dir = m_OpenVideoFileDialog->directory();
}


void MainWindow::on_positionSlider_sliderPressed()
{
   m_mplayerWidget->pause();
   disconnect(m_mplayerWidget, SIGNAL(positionChanged(quint64,quint64)), this, SLOT(setVideoPosition(quint64, quint64)));
   //connect(ui->positionSlider, SIGNAL(valueChanged(int)), this, SLOT(seekSlider(int)));
}


void MainWindow::on_positionSlider_sliderReleased()
{
   //disconnect(ui->positionSlider, SIGNAL(valueChanged(int)), this, SLOT(seekSlider(int)));
   m_mplayerWidget->seekTo(ui->positionSlider->value());
   connect(m_mplayerWidget, SIGNAL(positionChanged(quint64,quint64)), this, SLOT(setVideoPosition(quint64, quint64)));
   m_mplayerWidget->playPause();
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
   m_mplayerWidget->pause();
   if(m_ColorDialog->exec() == QDialog::Accepted)
   {
      *m_Color = m_ColorDialog->selectedColor();
      ui->textEdit->setTextColor(*m_Color);
      QString text = ui->textEdit->toPlainText();
      ui->textEdit->setPlainText("Plain Text");
      ui->textEdit->setPlainText(text);
      ui->tableWidget->currentItem()->setTextColor(*m_Color);
   }
}


void MainWindow::on_positionCheckBox_toggled(bool b)
{
   if(b == true)
   {
      m_Subtitle->setPosition(-1, -1);
   }
   else
   {
      m_Subtitle->setPosition(ui->x1SpinBox->value(), ui->y1SpinBox->value());
   }
   m_Subtitle->updateLine();
   m_mplayerWidget->updateSubtitle();
}


void MainWindow::readLog(QString log)
{
   //m_LogDialog.append(log);
}


void MainWindow::showSubtitleWidgets(bool b)
{
   ui->tableWidget->setEnabled(b);
   ui->showLabel->setEnabled(b);
   ui->showTimeEdit->setEnabled(b);
   ui->hideLabel->setEnabled(b);
   ui->hideTimeEdit->setEnabled(b);
   ui->fontComboBox->setEnabled(b);
   ui->sizeSpinBox->setEnabled(b);
   ui->buttonBold->setEnabled(b);
   ui->buttonItalic->setEnabled(b);
   ui->buttonUnderline->setEnabled(b);
   ui->buttonStrikeout->setEnabled(b);
   ui->positionCheckBox->setEnabled(b);
   ui->colorButton->setEnabled(b);
   ui->textEdit->setEnabled(b);
   ui->toolbarSubtitle->setVisible(b);
}

void MainWindow::setEditingItemsEnabled(bool b)
{
   ui->dockWidget->setEnabled(b);
   ui->tabWidget->setEnabled(b);
   ui->actionConvert->setEnabled(b);
   if(ui->tabWidget->currentIndex() == 2)
      ui->toolbarSubtitle->setVisible(b);
}



void MainWindow::on_fontComboBox_currentFontChanged(const QFont &f)
{
   m_Font->setFamily(f.family());
   ui->textEdit->setFont(*m_Font);
}

void MainWindow::on_sizeSpinBox_valueChanged(int arg1)
{
   m_Font->setPointSize(arg1);
   ui->textEdit->setFontPointSize(arg1);
}

void MainWindow::on_buttonBold_toggled(bool checked)
{
   m_Font->setBold(checked);
   ui->textEdit->setFont(*m_Font);
}


void MainWindow::on_buttonItalic_toggled(bool checked)
{
   m_Font->setItalic(checked);
   ui->textEdit->setFont(*m_Font);
}


void MainWindow::on_buttonUnderline_toggled(bool checked)
{
   m_Font->setUnderline(checked);
   ui->textEdit->setFont(*m_Font);
}


void MainWindow::on_buttonStrikeout_toggled(bool checked)
{
   m_Font->setStrikeOut(checked);
   ui->textEdit->setFont(*m_Font);
}


void MainWindow::on_showTimeEdit_timeChanged(const QTime &newShowTime)
{
   m_ShowTime->setHMS(newShowTime.hour(), newShowTime.minute(), newShowTime.second(), newShowTime.msec());
   qDebug() << "MainWindow::on_showTimeEdit_timeChanged: " << m_ShowTime->toString("h:mm:ss,zz");
   ui->tableWidget->item(ui->tableWidget->currentRow(), 0)->setText( m_ShowTime->toString("h:mm:ss,zz") );
}


void MainWindow::on_hideTimeEdit_timeChanged(const QTime &newHideTime)
{
   m_HideTime->setHMS(newHideTime.hour(), newHideTime.minute(), newHideTime.second(), newHideTime.msec());
   qDebug() << "MainWindow::on_hideTimeEdit_timeChanged: " << m_HideTime->toString("h:mm:ss,zz");
   ui->tableWidget->item(ui->tableWidget->currentRow(), 1)->setText( m_HideTime->toString("h:mm:ss,zz") );
}


void MainWindow::on_actionSetLineShow_triggered()
{
   *m_ShowTime = m_VideoPosition;
   ui->showTimeEdit->setTime(m_VideoPosition);
   if(*m_HideTime < *m_ShowTime)
   {
      ui->hideTimeEdit->setTime( m_ShowTime->addSecs(1) );
   }
}


void MainWindow::on_actionSetLineHide_triggered()
{
   *m_HideTime = m_VideoPosition;
   ui->hideTimeEdit->setTime(m_VideoPosition);
   if(*m_HideTime < *m_ShowTime)
   {
      ui->showTimeEdit->setTime(m_HideTime->addSecs(-1));
   }
}


void MainWindow::on_tabWidget_currentChanged(int index)
{
   qDebug() << "MainWindow::on_tabWidget_currentChanged: " << QString::number(index) << ui->tabWidget->tabText(index);
   switch (index)
   {
   case 0:
   case 1:
      ui->toolbarSubtitle->setVisible(false);
      return;
   case 2:
      ui->toolbarSubtitle->setVisible(true);
      return;
   }
}


void MainWindow::on_buttonStartClip_clicked()
{
   ui->clipStartTimeEdit->setTime(m_VideoPosition);
   if( ui->clipEndTimeEdit->time() < ui->clipStartTimeEdit->time() )
   {
      ui->clipEndTimeEdit->setTime(m_VideoLength);
   }
   QTime clipDurationTime = QTime::fromMSecsSinceStartOfDay( ui->clipEndTimeEdit->time().msecsSinceStartOfDay() - ui->clipStartTimeEdit->time().msecsSinceStartOfDay() );
   ui->clipDurationTimeEdit->setTime(clipDurationTime);
}


void MainWindow::on_buttonEndClip_clicked()
{
   ui->clipEndTimeEdit->setTime(m_VideoPosition);
   if( ui->clipEndTimeEdit->time() < ui->clipStartTimeEdit->time() )
   {
      ui->clipStartTimeEdit->setTime(QTime(0, 0, 0));
   }
   QTime clipDurationTime = QTime::fromMSecsSinceStartOfDay( ui->clipEndTimeEdit->time().msecsSinceStartOfDay() - ui->clipStartTimeEdit->time().msecsSinceStartOfDay() );
   ui->clipDurationTimeEdit->setTime(clipDurationTime);
}


void MainWindow::on_actionOutputFile_triggered()
{
   qDebug() << "MainWindow::on_actionOutputFile_triggered: ";
   if(m_SaveOutputFileDialog == 0)
   {
      m_SaveOutputFileDialog = new QFileDialog(this);
      m_SaveOutputFileDialog->setFileMode(QFileDialog::AnyFile);
      m_SaveOutputFileDialog->setConfirmOverwrite(true);
      m_SaveOutputFileDialog->setWindowTitle("Save output to...");
      m_SaveOutputFileDialog->setNameFilter("MP4 video format (*.mp4)");
      m_SaveOutputFileDialog->setAcceptMode(QFileDialog::AcceptSave);
   }
   m_SaveOutputFileDialog->setDirectory(*m_Dir);
   if(m_SaveOutputFileDialog->exec())
   {
      m_OutputFile.setFileName( m_SaveOutputFileDialog->selectedFiles().first() );
      ui->lineEditOutput->setText(m_OutputFile.fileName());
   }
   *m_Dir = m_SaveOutputFileDialog->directory();
}

void MainWindow::on_playPauseButton_clicked()
{
   //m_Subtitle->updateLine();
   //m_mplayerWidget->updateSubtitle();
   m_mplayerWidget->playPause();
}
