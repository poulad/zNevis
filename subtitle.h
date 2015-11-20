#ifndef SUBTITLE_H
#define SUBTITLE_H
#include <QtCore>
#include <QStringList>
#include <QFile>
#include <QString>
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
   Subtitle(const QString &subtitleFileName = 0, QTextCodec *codec = 0);
   ~Subtitle();


   /// Get Functions:
   QString fileName();
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
   void setFileAddress(const QString &address);
   void setText(const QString &text);
   void setFont(const QFont &font);
   void setColor(const QColor &color);
   void setShowTime(const QTime &showTime);
   void setHideTime(const QTime &hideTime);
   void setDurationTime(const QTime &durationTime);

   void setCurrentLine(quint64 lineNumber);
   void setDefaultFont(const QFont& font);
   void setDefaultColor(const QColor& color);
   void insertLine(quint64 afterNumber, QString text = 0); // argument of value 0, means append
   void removeLine(quint64 lineNumber);
   void removeAll();

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
   QFile *m_File;
   quint64 m_LineCount;

   quint64 m_CurrentLine;

   QRegExp m_LineRegex;

   QTextStream *qerr;
   QTextStream *qout;
};

#endif // SUBTITLE_H
