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
   quint64 lineCount() const;
   quint64 getCurrentLine() const;


   /// Set Functions:
   void setFileAddress(const QString &address);

   void setShowTime(const QTime &showTime);
   void setHideTime(const QTime &hideTime);
   void setFont(const QFont &font);
   void setColor(const QColor &color);
   void setText(const QString &text);
   void setPosition(int x1, int y1);

   void setCurrentLine(quint64 lineNumber);
   void setDefaultFont(const QFont& font);
   void setDefaultColor(const QColor& color);
   void appendLine();
   void removeLine(quint64 lineNumber);
   void removeAll();

   /// Manipulate Functions:
   bool load(QTextCodec *textCodec = 0);
   bool save();
   bool saveAsSrt();
   void updateLine(quint64 lineNumber = 0);

public slots:


private:
   inline void loadTime(const QStringList &);
   inline void loadTextFormat(QString &);
   inline QString dialogueText(quint64 lineNumber);

   // Private members:
   QStringList m_SrtLines;
   QStringList m_TextList;
   QList<QTime> m_ShowTimeList;
   QList<QTime> m_HideTimeList;
   QList<QFont> m_FontList;
   QList<QColor> m_ColorList;
   QList<int> m_X1List;
   QList<int> m_Y1List;

   QString m_AssStyles;
   QString m_AssEvents;

   QFont m_DefaultFont;
   QColor m_DefaultColor;

   QFile *m_OriginalFile;
   QFile *m_AssFile;

   int m_LineCount;

   quint64 m_CurrentLine;

   QRegExp m_LineRegex;

   QTextStream *qerr;
   QTextStream *qout;
   QTextStream *m_AssTextStream;
};

#endif // SUBTITLE_H
