#ifndef SUBTITLE_H
#define SUBTITLE_H
#include <QtCore>
#include <QStringList>
#include <QFile>
#include <QTemporaryFile>
#include <QTextCodec>
#include <QTextStream>
#include <QColor>
#include <QFont>
#include <QDebug>
#include <iostream>

class Subtitle
{

public:
   Subtitle();
   Subtitle(QString subtitleFileName);
   ~Subtitle();

   /// Get Functions:
   QString getSubtitleFileName();
   QString &text();
   const QFont &getFont();
   QFont &font();
   const QColor &getColor();
   QColor &color();
   QTime &showTime();
   QTime &hideTime();
   QTime &durationTime();
   quint64 lineCount() const;
   quint64 getCurrentLine() const;


   /// Set Functions:
   void setFile(const QFile &file);
   void setText(const QString &text);
   void setFont(const QFont &font);
   void setColor(const QColor &color);
   void setShowTime(const QTime &showTime);
   void setHideTime(const QTime &hideTime);
   void setDurationTime(const QTime &durationTime);
   void setCurrentLine(quint64 lineNumber);
   void insertLine(quint64 afterNumber, QString text = 0); // argument of value 0, means at last
   void removeLine(quint64 lineNumber);
   void clear();

   /// Manipulate Functions:
   bool load(QTextCodec *textCodec = 0);
   bool save();
   void updateLine(quint64 lineNumber = 0);

private:
   inline void loadTime(const QStringList &);
   inline void loadTextFormat(QString &);

   // Private members:
   QStringList m_SrtLines;
   QStringList m_TextList;
   QList<QTime> m_ShowTimeList;
   QList<QTime> m_HideTimeList;
   QList<QTime> m_DurationTimeList;
   QList<QFont> m_FontList;
   QList<QColor> m_ColorList;

   QFont m_DefaultFont;
   QColor m_DefaultColor;
   QFile m_File;
   QTemporaryFile m_TempFile;
   quint64 m_LineCount;

   quint64 m_CurrentLine;

   QRegExp m_LineRegex;

   QTextStream *qerr;
   QTextStream *qout;
};

#endif // SUBTITLE_H
