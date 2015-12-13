#include "subtitle.h"

Subtitle::Subtitle(const QString &fileName, QTextCodec *codec)
{
   qerr = new QTextStream(stderr);
   qout = new QTextStream(stdout);
   m_DefaultFont.setPointSize(20);
   m_DefaultColor.setNamedColor("white");
   m_CurrentLine = 1;
   m_LineRegex.setCaseSensitivity(Qt::CaseInsensitive);

   m_OriginalFile = 0;
   m_AssFile = 0;

   *qerr << "Subtitle::Subtitle: " << endl;
   if(fileName == 0)
   {
      int n = 0;
      QString fileName = QDir::tempPath() + "/subtitle%1.ass";
      QFile file;
      do
      {
         file.setFileName(fileName.arg(++n));
      } while(file.exists());

      m_AssFile = new QFile();
      m_AssFile->setFileName(file.fileName());

      //creating first line:
      m_AssStyles =
            "[V4+ Styles]\r\n"
            "Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, BackColour, Bold, Italic, Underline, "
            "StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding\r\n"
            "Style: Default," + m_DefaultFont.family() + ",10,0,0,&H0,&H0,0,0,0,0,100,100,0,0,1,1,0,2,10,10,10,0\r\n\r\n";

      m_AssEvents =
            "[Events]\r\n"
            "Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text\r\n";

      m_ShowTimeList.append(QTime(0, 0, 0, 0));
      m_HideTimeList.append(QTime(0, 1, 5, 0));
      m_FontList.append(m_DefaultFont);
      m_ColorList.append(m_DefaultColor);
      m_X1List.append(-1);
      m_Y1List.append(-1);
      m_TextList.append("Text");
      m_SrtLines.append( dialogueText(1) );
      m_LineCount = 1;

      m_AssFile->open(QIODevice::WriteOnly | QIODevice::Truncate);
      m_AssTextStream = new QTextStream(m_AssFile);
      m_AssTextStream->setCodec( QTextCodec::codecForName("UTF-8") );
   }
   else
   {
      m_OriginalFile = new QFile();
      m_OriginalFile->setFileName(fileName);
   }
   if (m_AssFile == 0)
      *qerr << "Subtitle::" << "Subtitle: " << "\"" << m_OriginalFile->fileName() << "\"" << endl;
   else if(m_OriginalFile == 0)
      *qerr << "Subtitle::" << "Subtitle: " << "\"" << m_AssFile->fileName() << "\"" << endl;

   if(m_SrtLines.isEmpty())
      load(codec);
   else
      save();
}


Subtitle::~Subtitle()
{
   *qerr << "Subtitle::~Subtitle: " << endl;
   //m_File->remove();
}




/// ---------------------------------------------------- GET Functions ----------------------------------------------------





QString Subtitle::fileName()
{
   if(m_AssFile != 0)
      return m_AssFile->fileName();
   else if(m_OriginalFile != 0)
      return m_OriginalFile->fileName();
}


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


quint64 Subtitle::lineCount() const
{
   return m_LineCount;
}


quint64 Subtitle::getCurrentLine() const
{
   return m_CurrentLine;
}





/// ---------------------------------------------------- SET Functions ----------------------------------------------------




void Subtitle::setFileAddress(const QString &address)
{
   if(address == m_OriginalFile->fileName())
      return;

   removeAll();
   m_OriginalFile->setFileName(address);
   load();
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


void Subtitle::setPosition(int x1, int y1)
{
   if(x1 < -1)
      x1 = -1;
   if(y1 < -1)
      y1 = -1;
   m_X1List[m_CurrentLine-1] = x1;
   m_Y1List[m_CurrentLine-1] = y1;
}


void Subtitle::setCurrentLine(quint64 lineNumber)
{
   if( lineNumber <= m_LineCount )
      m_CurrentLine = lineNumber;
   else
      m_CurrentLine = 1;
}


void Subtitle::setDefaultFont(const QFont &font)
{
   *qerr << "Subtitle::setDefaultFont: " << font.family() << ", " << QString::number(font.pointSize()) << endl;
   m_DefaultFont = font;
}


void Subtitle::setDefaultColor(const QColor& color)
{
   *qerr << "Subtitle::setDefaultColor: " << color.name(QColor::HexRgb) << endl;
   m_DefaultColor = color;
}


void Subtitle::appendLine()
{
   QTime showTime = m_HideTimeList.last().addSecs(1);
   m_ShowTimeList.append(showTime);
   m_HideTimeList.append(showTime.addSecs(2));

   m_FontList.append( m_FontList.last() );
   m_ColorList.append( m_ColorList.last() );
   m_X1List.append( m_X1List.last() );
   m_Y1List.append( m_Y1List.last() );
   m_TextList.append("text");
   m_SrtLines.append( dialogueText( ++m_LineCount ) );
}


void Subtitle::removeLine(quint64 lineNumber)
{
   lineNumber--;
   m_ShowTimeList.removeAt(lineNumber);
   m_HideTimeList.removeAt(lineNumber);
   m_FontList.removeAt(lineNumber);
   m_ColorList.removeAt(lineNumber);
   m_TextList.removeAt(lineNumber);
   m_SrtLines.removeAt(lineNumber);
   --m_LineCount;
   save();
}


void Subtitle::removeAll()
{
   QTime showTime, hideTime;
   showTime.setHMS(0, 0, 0, 0);
   hideTime.setHMS(0, 0, 1, 0);
   QFont font(m_FontList.at(m_CurrentLine - 1));
   QColor color(m_ColorList.at(m_CurrentLine - 1));
   QString text = "text";

   m_ShowTimeList.clear();
   m_HideTimeList.clear();
   m_FontList.clear();
   m_ColorList.clear();
   m_TextList.clear();
   m_SrtLines.clear();

   m_LineCount = 1;
   m_ShowTimeList.append(showTime);
   m_HideTimeList.append(hideTime);
   m_FontList.append(font);
   m_ColorList.append(color);
   m_TextList.append(text);
   m_SrtLines.append(
            showTime.toString("h:mm:ss,zz") + " --> " + hideTime.toString("h:mm:ss,zz") + "\n" +
            "<font face=\"" + font.family() + "\" size=" + QString::number(font.pointSize()) + " color=\"" + color.name(QColor::HexRgb) +
            "\">" + text + "</font>"
            );
   save();
}



/// ---------------------------------------------------- Manipulate Functions ----------------------------------------------------





bool Subtitle::load(QTextCodec *textCodec)
{
   if(m_OriginalFile->isOpen() == false)
      m_OriginalFile->open(QIODevice::ReadOnly);
   QTextStream input(m_OriginalFile);
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
            m_ShowTimeList[i].toString("h:mm:ss,zz") + " --> " + m_HideTimeList[i].toString("h:mm:ss,zz") + "\n" +
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
   *qerr << "Subtitle::" << "saveAsASS: " << m_AssFile->fileName() << endl;
   m_AssFile->resize(0);
   *m_AssTextStream << m_AssStyles << m_AssEvents << m_SrtLines.join('\r\n') << flush;
   return true;
}


bool Subtitle::saveAsSrt()
{
   *qerr << "Subtitle::" << "saveAsSrt: " << m_OriginalFile->fileName() << endl;
   m_OriginalFile->open(QIODevice::WriteOnly | QIODevice::Truncate);
   m_OriginalFile->resize(0);
   QTextStream output(m_OriginalFile);
   output.setCodec( QTextCodec::codecForName("UTF-8") );
   QString srtText;
   for(int i = 0; i < m_LineCount; ++i)
      srtText += QString::number(i+1) + "\n" + m_SrtLines[i] + "\n\n";
   srtText.chop(2);
   output << srtText;
   *qerr << "Subtitle::" << "saveAsSrt: " << "file saved" << endl;
   return true;
}






void Subtitle::loadTime(const QStringList &timeList)
{
   QTime showTime( timeList.at(1).toInt(), timeList.at(2).toInt(), timeList.at(3).toInt(),timeList.at(4).toInt());
   QTime hideTime( timeList.at(5).toInt(), timeList.at(6).toInt(), timeList.at(7).toInt(),timeList.at(8).toInt());
   if(showTime > hideTime)
   {
      showTime = hideTime;
   }

   m_ShowTimeList.append(showTime);
   m_HideTimeList.append(hideTime);

   *qerr << "Subtitle::" << "loadTime# " << "show_time=" << showTime.toString("h:mm:ss,zz") << endl;
   *qerr << "Subtitle::" << "loadTime# " << "hide_time=" << hideTime.toString("h:mm:ss,zz") << endl;

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


QString Subtitle::dialogueText(quint64 lineNumber)
{
   QFont font = m_FontList.at(lineNumber-1);

   QString BBGGRR = m_ColorList[lineNumber-1].name(QColor::HexRgb);
   BBGGRR.remove('#');
   QRegExp rgbRegex("([a-f0-9]{2})([a-f0-9]{2})([a-f0-9]{2})", Qt::CaseInsensitive);
   if(BBGGRR.contains(rgbRegex))
      BBGGRR = rgbRegex.cap(3) + rgbRegex.cap(2) + rgbRegex.cap(1);

   QString position;
   int x1 =  m_X1List[lineNumber-1];
   int y1 = m_Y1List[lineNumber-1] ;
   if( x1 == -1 && y1==-1 )
      position = "";
   else if( 0 <= x1 && 0 <= y1 )
      position = "{\\pos(" + QString::number(x1) + "," + QString::number(y1) + ")}";

   QString dialogueString =
         "Dialogue: 0," + m_ShowTimeList[lineNumber-1].toString("h:mm:ss.zz") + "," + m_HideTimeList[lineNumber-1].toString("h:mm:ss.zz") + ",Default,,0,0,0,," +
         position +
         "{\\fn" + font.family() + "}" +
         "{\\fs" + QString::number(font.pointSize()) + "}" +
         ( font.bold() ? "{\\b1}" : "" ) +
         ( font.italic() ? "{\\i1}" : "") +
         ( font.underline() ? "{\\u1}" : "" ) +
         ( font.strikeOut() ? "{\\s1}" : "") +
         "{\\c&H" + BBGGRR + "&}" +
         m_TextList.at(lineNumber-1)
         ;
   return dialogueString;
}


void Subtitle::updateLine(quint64 lineNumber)
{
   *qerr << "Subtitle::" << "replaceLine: " << endl;
   if(lineNumber == 0)
      lineNumber = m_CurrentLine;
   QString newLine = dialogueText(lineNumber);
   m_SrtLines.replace(lineNumber-1, newLine);
   save();
}
