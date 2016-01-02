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
#include "mplayerwidget.h"
#include "mencodercontrol.h"
#include <QMessageBox>
#include <QInputDialog>


//#include <QTextEdit>

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
   void changeLineProperties(int,int,int, int);
   void on_tabWidget_currentChanged(int index);

   // MPlayer:
   void importVideoId(const MediaID* mediaID);
   void on_actionOpenVideo_triggered();
   void on_playPauseButton_clicked();
   void on_positionSlider_sliderPressed();
   void on_positionSlider_sliderReleased();
   void playPreviousLine();
   void playNextLine();
   void updateVideoPosition(int mSec);

   // MEncoder:
   void on_actionConvert_triggered();
   void on_actionOutputFile_triggered();
   void on_buttonStartClip_clicked();
   void on_buttonEndClip_clicked();

   // Subtitle:
   void importSubtitle();
   void on_actionNewSubtitle_triggered();
   void on_actionOpenSubtitle_triggered();
   void on_updateCheckBox_toggled(bool checked);
   void on_actionSetLineShow_triggered();
   void on_actionSetLineHide_triggered();
   void on_fontComboBox_currentFontChanged(const QFont &f);
   void on_sizeSpinBox_valueChanged(int arg1);
   void on_buttonBold_toggled(bool checked);
   void on_buttonItalic_toggled(bool checked);
   void on_buttonUnderline_toggled(bool checked);
   void on_buttonStrikeout_toggled(bool checked);
   void on_colorButton_clicked();
   void on_positionCheckBox_toggled(bool);
   void updateText();
   void updatePosition();
   void updateShowTime(QTime showTime);
   void updateHideTime(QTime hideTime);
   void on_actionAddText_triggered();


private:
   inline void showSubtitleWidgets(bool);
   inline void setEditingItemsEnabled(bool);

   Ui::MainWindow *ui;
   QMainWindow *m;
   ConvertDialog *m_ConvertDialog;
   //QTextEdit m_LogDialog;

   QString *m_Text;
   QFont *m_Font;
   QColor *m_Color;
   QTime *m_ShowTime;
   QTime *m_HideTime;

   QWidget *newWidget;
   Subtitle *m_Subtitle;
   QColorDialog *m_ColorDialog;
   QFontDialog *fontDialog;
   QFileDialog *m_OpenSubtitleFileDialog;
   QFileDialog *m_MPlayerFileDialog;

   QFile m_SubtitleFile;
   QFile m_OutputFile;
   QDir *m_Dir;
   MPlayerWidget *m_MPlayerWidget;
   MEncoderControl *m_MEncoderControl;

   QFileDialog *m_OpenVideoFileDialog;
   QFile m_VideoFile;
   QTime m_VideoPosition;
   QTime m_VideoLength;
   int m_VideoWidth;
   int m_VideoHeight;

   QFileDialog *m_SaveOutputFileDialog;

   bool m_IsPlaying;
};

#endif // MAINWINDOW_H
