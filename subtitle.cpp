#include "subtitle.h"

Subtitle::Subtitle()
{
   qerr = new QTextStream(stderr);
   qout = new QTextStream(stdout);
   *qerr << "Subtitle::" << "Subtitle# " << endl;
   m_DefaultFont.setPointSize(16);
   m_DefaultColor.setNamedColor("red");
   m_CurrentLine = 1;
   m_LineRegex.setCaseSensitivity(Qt::CaseInsensitive);
}


Subtitle::Subtitle(QString fileName)
{
   qerr = new QTextStream(stderr);
   qout = new QTextStream(stdout);
   *qerr << "Subtitle::" << "Subtitle: " << "\"" << fileName << "\"" << endl;
   m_DefaultFont.setPointSize(16);
   m_DefaultColor.setNamedColor("red");
   m_CurrentLine = 1;
   m_LineRegex.setCaseSensitivity(Qt::CaseInsensitive);

   m_File.setFileName(fileName);
   if( m_File.exists() )
   {
      m_File.open(QIODevice::ReadWrite);
   }
   else
   {
      *qerr << "Subtitle::" << "Subtitle# " << "Warning: file doesn't exist or cannot be accessed." << endl;
   }
}


Subtitle::~Subtitle()
{
   m_File.remove();
}



/// ---------------------------------------------------- GET Functions ----------------------------------------------------





QString &Subtitle::text()
{
   return m_TextList[m_CurrentLine-1];
}


const QFont &Subtitle::getFont()
{
   return m_FontList[m_CurrentLine-1];
}


QFont &Subtitle::font()
{
   return m_FontList[m_CurrentLine-1];
}


const QColor &Subtitle::getColor()
{
   return m_ColorList[m_CurrentLine-1];
}

QColor &Subtitle::color()
{
   return m_ColorList[m_CurrentLine-1];
}


QTime &Subtitle::showTime()
{
   return m_ShowTimeList[m_CurrentLine-1];
}


QTime &Subtitle::hideTime()
{
   return m_HideTimeList[m_CurrentLine-1];
}


QTime &Subtitle::durationTime()
{
   return m_DurationTimeList[m_CurrentLine-1];
}


quint64 Subtitle::lineCount() const
{
   return m_LineCount;
}


quint64 Subtitle::getCurrentLine() const
{
   return m_CurrentLine;
}





/// ---------------------------------------------------- SET Functions ----------------------------------------------------




void Subtitle::setFile(const QFile &file)
{
   m_File.setFileName(file.fileName());
}


void Subtitle::setText(const QString &text)
{
   m_TextList[m_CurrentLine-1] = text;
}


void Subtitle::setFont(const QFont &font)
{
   m_FontList[m_CurrentLine-1] = font;
}


void Subtitle::setColor(const QColor &color)
{
   m_ColorList[m_CurrentLine-1] = color;
}


void Subtitle::setShowTime(const QTime &showTime)
{
   m_ShowTimeList[m_CurrentLine-1] = showTime;
}


void Subtitle::setHideTime(const QTime &hideTime)
{
   m_HideTimeList[m_CurrentLine-1] = hideTime;
}


void Subtitle::setDurationTime(const QTime &durationTime)
{
   m_DurationTimeList[m_CurrentLine-1] = durationTime;
}


void Subtitle::setCurrentLine(quint64 lineNumber)
{
   if( lineNumber <= m_LineCount )
      m_CurrentLine = lineNumber;
   else
      m_CurrentLine = 1;
}


void Subtitle::insertLine(quint64 afterNumber, QString text)
{/*
   m_CurrentLine = afterNumber+1;
   SubtitleLine subLine;
   if(!text.isEmpty())
      subLine.text = text;
   subLine.font = m_DefaultFont;
   subLine.color = m_DefaultColor;
   m_SubtitleLinesList.insert(afterNumber-1, subLine);
}


void Subtitle::removeLine(quint64 lineNumber)
{
   m_CurrentLine = lineNumber;
   m_SubtitleLinesList.removeAt(lineNumber-1);
}


void Subtitle::clear()
{
   m_CurrentLine = 1;
   m_LineCount = 0;
   m_DefaultColor = QColor();
   m_DefaultFont = QFont();
   m_SubtitleFile.setFileName("");
   m_SubtitleLinesList.clear();
   */
}



/// ---------------------------------------------------- Manipulate Functions ----------------------------------------------------





bool Subtitle::load(QTextCodec *textCodec)
{
   QTextStream input(&m_File);
   if(textCodec != 0)
   {
      input.setCodec(textCodec);
   }
   else
   {
      input.setAutoDetectUnicode(true);
      *qerr << "Subtitle::" << "loadSubtitle# " << "Text Codec automatically set to \"" << input.codec() << "\"" << endl;
   }
   QString loadedText = input.readAll();
   QRegExp m_redundantRegex("\r?\n\r?\n\\d+\r?\n");
   if(loadedText.at(0) == '1')
   {
      loadedText.prepend("\n\n"); // to be compatible with m_redundantRegex
   }
   m_TextList = loadedText.split(m_redundantRegex);
   m_TextList.removeFirst();

   QRegExp m_TimeRegex( "^(\\d{2}):(\\d{2}):(\\d{2})[,.](\\d{3})[ \t]+-->[ \t]+(\\d{2}):(\\d{2}):(\\d{2})[,.](\\d{3})[ \t]?[\r]?\n" );
   m_LineCount = 0;
   do
   {
      ++m_LineCount;
      *qerr << "Subtitle::" << "loadSubtitle# " << "loading line #" << QString::number(m_LineCount) << endl;
      m_TextList[m_LineCount-1].contains(m_TimeRegex);
      m_TextList[m_LineCount-1].remove(m_TimeRegex);
      loadTime(m_TimeRegex.capturedTexts());
      loadTextFormat(m_TextList[m_LineCount-1]);
      //while(text.endsWith("\n" || text.endsWith("\r")))
      //   text.chop(1);
      *qerr << "Subtitle::" << "loadSubtitle: " << "line#" << QString::number(m_LineCount) << m_TextList.at(m_LineCount-1) << endl;
      qDebug() << m_TextList[m_LineCount-1];
   } while(m_LineCount < m_TextList.size());

   *qerr << "Subtitle::" << "load: " << "appending lines." << endl;
   QString line;
   for(int i = 0; i < m_LineCount; ++i)
   {
      line =
            m_ShowTimeList[i].toString("hh:mm:ss,zzz") + " --> " + m_HideTimeList[i].toString("hh:mm:ss,zzz") + "\n" +
            "<font face=\"" + m_FontList[i].family() + "\" size=" + QString::number( m_FontList[i].pointSize() ) + " color=" +
            m_ColorList[i].name(QColor::HexRgb) + ">" +(m_FontList[i].bold() ? "<b>" : "") + (m_FontList[i].italic() ? "<i>" : "")+
            (m_FontList[i].strikeOut() ? "<s>" : "") + (m_FontList[i].underline() ? "<u>" : "") + m_TextList[i] +
            ( m_FontList[i].underline() ? "</u>" : "") + ( m_FontList[i].strikeOut() ? "</s>" : "") + (m_FontList[i].italic() ? "</i>" : "") +
            (m_FontList[i].bold() ? "</b>" : "") + "</font>";
      m_SrtLines.append(line);
   }
   save();
   return true;
}


bool Subtitle::save()
{
   *qerr << "Subtitle::" << "saveSubtitle: " << m_File.fileName() << endl;
   m_File.open(QIODevice::WriteOnly | QIODevice::Truncate);
   m_File.resize(0);
   QTextStream output(&m_File);
   output.setCodec( QTextCodec::codecForName("UTF-8") );
   QString srtText;
   for(int i = 0; i < m_LineCount; ++i)
      srtText += QString::number(i+1) + "\n" + m_SrtLines[i] + "\n\n";
   srtText.chop(2);
   output << srtText;
   *qerr << "Subtitle::" << "saveSubtitle# " << "file saved" << endl;
   return true;
}


void Subtitle::loadTime(const QStringList &timeList)
{
   QTime showTime( timeList.at(1).toInt(), timeList.at(2).toInt(), timeList.at(3).toInt(),timeList.at(4).toInt());
   QTime hideTime( timeList.at(5).toInt(), timeList.at(6).toInt(), timeList.at(7).toInt(),timeList.at(8).toInt());
   QTime durationTime;
   if(showTime > hideTime)
   {
      showTime = hideTime;
      durationTime.setHMS(0,0,0,0);
   }
   else
      durationTime = QTime::fromMSecsSinceStartOfDay( hideTime.msecsSinceStartOfDay() - showTime.msecsSinceStartOfDay() );

   m_ShowTimeList.append(showTime);
   m_HideTimeList.append(hideTime);
   m_DurationTimeList.append(durationTime);

   *qerr << "Subtitle::" << "loadTime# " << "show_time=" << showTime.toString("hh:mm:ss,zzz") << endl;
   *qerr << "Subtitle::" << "loadTime# " << "hide_time=" << hideTime.toString("hh:mm:ss,zzz") << endl;
   *qerr << "Subtitle::" << "loadTime# " << "duration_time=" << durationTime.toString("hh:mm:ss,zzz") << endl;

}


void Subtitle::loadTextFormat(QString &lineText)
{
   QFont font = m_DefaultFont;
   QColor color = m_DefaultColor;
   QRegExp regexFontTag("<font\\sface=[\'\"]?([a-zA-Z\\d ]*)[\'\"]?\\ssize=(\\d+)\\s(?:colour|color)=[\'\"]?((?:#[a-fA-F\\d]{6,8}|[a-zA-Z]+))[\'\"]?>(?:.|\\r|\\n)*</font>", Qt::CaseInsensitive);
   QRegExp regexBoldTag("<b>(?:.|\\r|\\n)*</b>", Qt::CaseInsensitive);
   QRegExp regexItalicTag("<i>(?:.|\\r|\\n)*</i>", Qt::CaseInsensitive);
   QRegExp regexStrikeOutTag("<s>(?:.|\\r|\\n)*</s>", Qt::CaseInsensitive);
   QRegExp regexUnderlineTag("<u>(?:.|\\r|\\n)*</u>", Qt::CaseInsensitive);
   QRegExp regexTagsToRemove("<(?:[/]?(?:b|i|s|u)|/font)>", Qt::CaseInsensitive);
   QRegExp regexFontTagToRemove("<font\\sface=[\'\"]?([a-zA-Z\\d ]*)[\'\"]?\\ssize=(\\d+)\\s(?:colour|color)=[\'\"]?((?:#[a-fA-F\\d]{6,8}|[a-zA-Z]+))[\'\"]?>", Qt::CaseInsensitive);

   if(lineText.contains( regexFontTag ))
   {
      /// 0: Whole regex
      /// 1:face value
      /// 2:size value
      /// 3:color value
      font.setFamily( regexFontTag.cap(1) );
      font.setPointSize( regexFontTag.cap(2).toInt() );
      if( QColor::isValidColor( regexFontTag.cap(3) ) )
         color.setNamedColor( regexFontTag.cap(3) );
   }
   if(lineText.contains(regexBoldTag))
      font.setBold(true);
   if(lineText.contains(regexItalicTag))
      font.setItalic(true);
   if(lineText.contains(regexStrikeOutTag))
      font.setStrikeOut(true);
   if(lineText.contains(regexUnderlineTag))
      font.setUnderline(true);

   lineText.remove( regexTagsToRemove );
   lineText.remove(regexFontTagToRemove);

   m_FontList.append(font);
   m_ColorList.append(color);
   *qerr << "Subtitle::" << "loadTextFormat# " << "face:" +  font.family() << endl;
   *qerr << "Subtitle::" << "loadTextFormat# " << "size:" +  QString::number( font.pointSize() ) << endl;
   *qerr << "Subtitle::" << "loadTextFormat# " << "color:" +  color.name() << endl;
   *qerr << "Subtitle::" << "loadTextFormat# " << "bold:" <<  font.bold() << endl;
   *qerr << "Subtitle::" << "loadTextFormat# " << "italic:" <<  font.italic() << endl;
   *qerr << "Subtitle::" << "loadTextFormat# " << "strikeout:" <<  font.strikeOut() << endl;
   *qerr << "Subtitle::" << "loadTextFormat# " << "underline:" <<  font.underline() << endl;
}


void Subtitle::updateLine(quint64 lineNumber)
{
   *qerr << "Subtitle::" << "replaceLine: " << endl;
   if(lineNumber == 0)  lineNumber = m_CurrentLine;
   QString newLine =
         m_ShowTimeList[lineNumber-1].toString("hh:mm:ss,zzz") + " --> " + m_HideTimeList[lineNumber-1].toString("hh:mm:ss,zzz") + "\n" +
         "<font face=\"" + m_FontList[lineNumber-1].family() + "\" size=" + QString::number( m_FontList[lineNumber-1].pointSize() ) + " color=" +
         m_ColorList[lineNumber-1].name(QColor::HexRgb) + ">" +(m_FontList[lineNumber-1].bold() ? "<b>" : "") + (m_FontList[lineNumber-1].italic() ? "<i>" : "") +
         (m_FontList[lineNumber-1].strikeOut() ? "<s>" : "") + (m_FontList[lineNumber-1].underline() ? "<u>" : "") + m_TextList[lineNumber-1] +
         ( m_FontList[lineNumber-1].underline() ? "</u>" : "") + ( m_FontList[lineNumber-1].strikeOut() ? "</s>" : "") + (m_FontList[lineNumber-1].italic() ? "</i>" : "") +
         (m_FontList[lineNumber-1].bold() ? "</b>" : "") + "</font>\n";
   m_SrtLines.replace(lineNumber-1, newLine);
   save();
}
