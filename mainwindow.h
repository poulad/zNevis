#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "ui_mainwindow.h"
#include "convertdialog.h"
#include <QMainWindow>
#include "subtitle.h"
#include <QColor>
#include <QColorDialog>
#include <QFontDialog>
#include <QFileDialog>
#include <QFile>
#include <QDir>
#include "mplayercontrol.h"
#include <QMessageBox>

namespace Ui {
   class MainWindow;
}

class MainWindow : public QMainWindow

{
   Q_OBJECT

public:
   MainWindow(QWidget *parent = 0);
   ~MainWindow();

private slots:
   void findMplayer(QString*);
   void createGui();
   void importVideoId(QStringList &videoIdList);

   void changeLineProperties(int,int,int, int);

   void updateText();
   void updateFont();
   void updateShowTime(QTime);
   void updateHideTime(QTime);
   void updateDurationTime(QTime);

   void importSubtitle();

   void playPreviousLine();
   void playNextLine();

   void setVideoPosition(quint64, quint64);
   void seekSlider(int);

   void on_actionOpenSubtitle_triggered();
   void on_actionOpenVideo_triggered();
   void on_positionSlider_sliderPressed();
   void on_positionSlider_sliderReleased();
   void on_actionConvert_triggered();
   void on_colorButton_clicked();

private:
   inline void enableItems(bool);

   Ui::MainWindow *ui;
   QMainWindow *m;
   ConvertDialog *m_ConvertDialog;

   QString *m_Text;
   QFont *m_Font;
   QColor *m_Color;
   QTime *m_ShowTime;
   QTime *m_HideTime;
   QTime *m_DurationTime;

   QWidget *newWidget;
   Subtitle *subtitle;
   QColorDialog *m_ColorDialog;
   QFontDialog *fontDialog;
   QFileDialog *m_SubtitleFileDialog;
   QFileDialog *m_MplayerFileDialog;

   QFile m_SubtitleFile;
   QFile m_OutputFile;
   QDir *m_Dir;
   MplayerControl *m_MplayerControl;

   QFileDialog *m_VideoFileDialog;
   QFile m_VideoFile;
   QTime *m_VideoPosition;
   int m_VideoWidth;
   int m_VideoHeight;

   bool m_MplayerExists;
};

#endif // MAINWINDOW_H
