
/* Copyright (C) 2007 Stefan Koegl
*
* This file is part of perturbis
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "Settings.hh"
#include "GeneralSettings.hh"
#include "MainWindow.hh"
#include "MessageOutput.hh"

#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QFile>
#include <QCheckBox>
#include <QFontComboBox>
#include <QLayout>
#include <QSettings>
#include <QSpinBox>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>
#include <QTextEdit>
#include <QVariant>
#include <paludis/util/log.hh>
#include <paludis/util/system.hh>
#include <paludis/util/fd_output_stream.hh>
#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

typedef  void (QTextFormat::*color_func)(const QBrush &);
#define CALL_MEMBER_FN(object,ptrToMember)  ((object).*(ptrToMember))

#define CTL  1
#define CHR  2
#define CPN  4
#define DIG  8
#define SCS 16
#define GRP 32
#define CPS 64

#define lec(P,L,C) (p == (P) &&                     s[(L)]         == (C))
#define lun( ) (p ==  1  &&                       cc           >= 32 )
#define les(P,L,C) (p == (P) && s[L] < 256  && (m_tbl[s[(L)]] & (C)) == (C))
#define eec(C)     (p >=  3  &&        cc                          == (C))
#define ees(C)     (p >=  3  && cc < 256 &&    (m_tbl[  cc  ] & (C)) == (C))
#define eps(C)     (p >=  3  && s[2] != '?' && s[2] != '!' && s[2] != '>' && cc < 256 && (m_tbl[  cc  ] & (C)) == (C))
#define epp( )     (p >=  3  && s[2] == '?'                        )
#define epe( )     (p >=  3  && s[2] == '!'                        )
#define egt( ) (p >=  3  && s[2] == '>'                        )
#define Xpe        (m_ppos>=2  && m_pbuf[1] == ']'                     )
#define Xte        (Xpe                        &&     cc           ==  7 )
#define ces(C)     (        cc < 256 &&    (m_tbl[  cc  ] & (C)) == (C) && !Xte)

#define ESC 27
#define CNTL(c) ((c)-'@')

#define TY_CONSTR(T,A,N) ( ((static_cast<int>(N) & 0xffff) << 16) | ((static_cast<int>(A) & 0xff) << 8) | (static_cast<int>(T) & 0xff) )

#define TY_CHR(   )     TY_CONSTR(0,0,0)
#define TY_CTL(A  )     TY_CONSTR(1,A,0)
#define TY_ESC(A  )     TY_CONSTR(2,A,0)
#define TY_ESC_CS(A,B)  TY_CONSTR(3,A,B)
#define TY_ESC_DE(A  )  TY_CONSTR(4,A,0)
#define TY_CSI_PS(A,N)  TY_CONSTR(5,A,N)
#define TY_CSI_PN(A  )  TY_CONSTR(6,A,0)
#define TY_CSI_PR(A,N)  TY_CONSTR(7,A,N)
#define TY_VT52(A  )    TY_CONSTR(8,A,0)
#define TY_CSI_PG(A  )  TY_CONSTR(9,A,0)
#define TY_CSI_PE(A  )  TY_CONSTR(10,A,0)

using namespace perturbis;

MessageThread::MessageThread(QObject* pobj, int fd) :
        QThread(pobj),
        m_master_fd(fd),
        m_polling(false)
{
}

MessageThread::~MessageThread()
{
    paludis::Log::get_instance()->set_log_stream(&std::cerr);
}

void
MessageThread::setPolling(bool on)
{
//     qDebug() << "MessageThread::setPolling()";
    if (on)
    {
        m_polling = true;
        if (!isRunning())
            start();
    }
    else
    {
        m_polling = false;
    }
}

void
MessageThread::run()
{
    char buf[512];
    while (m_polling)
    {
        int res = read(m_master_fd,&buf,512);
        if (res > 0)
        {
            sendMessage(QString::fromLocal8Bit(buf,res));
        }
        msleep(50);
    }
}

Vt102Display::Vt102Display(MessagePage* page) :
        QTextEdit(page),
        m_page(page),
        m_parserOn(Qt::Unchecked)
{
    m_cursor = textCursor();
    m_cursor.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
    setReadOnly(true);
    document()->setMaximumBlockCount(1000);
    QPalette p(palette());
     // background color  = black
    p.setColor(QPalette::Base,QColor(0,0,0));
     // text color  = white
    p.setColor(QPalette::Text,QColor(255,255,255));
    setPalette(p);
    ensureCursorVisible();
    setAutoFillBackground(true);
    m_currentColor = tc_normal;
    m_format.setForeground(Qt::white);

    loadSettings();
    initTokenizer();

//     QFile file("output.log");
//     if (!file.open(QIODevice::ReadOnly| QIODevice::Text))
//         return;
//     appendRawText(file.readAll());
//     file.close();
}

Vt102Display::~Vt102Display()
{
    saveSettings();
//     QFile file("output.log");
//     if (!file.open(QIODevice::WriteOnly| QIODevice::Text))
//         return;
//     QTextStream out(&file);
//     out << document()->toPlainText();
}

void
Vt102Display::loadSettings()
{
    qDebug() << "MessagePage::loadSettings() - start";
    QSettings settings("/etc/perturbis/perturbis.conf",QSettings::IniFormat);
    settings.beginGroup("Vt102Display");
    m_currentFont.fromString(settings.value("font","Monospace,-1,13,5,50,0,0,0,0,0").toString());
    m_fontSize = settings.value("fontSize",13).toInt();
    m_parserOn = (Qt::CheckState) settings.value("vt100",Qt::Unchecked).toInt();
    settings.endGroup();
}

void
Vt102Display::saveSettings()
{
    qDebug() << "Vt102Display::saveSettings() - start";
    QSettings settings("/etc/perturbis/perturbis.conf",QSettings::IniFormat);
    settings.beginGroup("Vt102Display");
    settings.setValue("font", m_currentFont.toString());
    settings.setValue("vt100", m_parserOn);
    settings.setValue("fontSize", m_fontSize);
    settings.endGroup();
    qDebug() << "Vt102Display::saveSettings() - done";
}

void
Vt102Display::setFont(const QFont & myfont)
{
    m_currentFont = myfont;
    m_currentFont.setPixelSize(m_fontSize);
    qDebug() << "m_currentFont.pixelSize" << m_currentFont.pixelSize() << m_fontSize << m_currentFont.toString();
    QTextEdit::setFont(m_currentFont);
}

void
Vt102Display::setParserMode(int mode)
{
    qDebug() << "setParserMode()" << m_parserOn;
    m_parserOn = (mode == Qt::Unchecked ? Qt::Unchecked : Qt::Checked);
}

void
Vt102Display::moveUp(int value)
{
    int old(m_cursor.position());
    m_cursor.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor,value);
    int start(m_cursor.position());
    qDebug()<<  "moveUp()" << old << start;
}

void
Vt102Display::setCursorX(int value)
{
    int length(m_cursor.block().length());
    if (value > length)
    {
        qDebug() << "setCursorX()"  << m_cursor.atBlockStart() << m_cursor.atBlockEnd() << m_cursor.block().text();
        m_cursor.movePosition(QTextCursor::EndOfBlock,QTextCursor::MoveAnchor);
        m_cursor.insertText(QString(value-length,' '),m_format);
        qDebug() << "setCursorX()" << m_cursor.block().text();
    }
    else
        m_cursor.setPosition(value);
//     qDebug() << "setCursorX()" << lineBegin << end << newPos << m_cursor.position();
}

void
Vt102Display::newline()
{
    qDebug() << "newline";
    m_cursor.insertText("\n",m_format);
    ensureCursorVisible();
}

void
Vt102Display::setCursorY(int value)
{
    m_cursor.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
    m_cursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,value);
}

void
Vt102Display::setCursorXY(int vx, int vy)
{
    m_cursor.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
    moveDown(vy);
    setCursorX(vx);
}

void
Vt102Display::moveDown(int value)
{
    m_cursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,value);
}

void
Vt102Display::moveLeft(int value)
{
    m_cursor.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,value);
}

void
Vt102Display::moveRight(int value)
{
    m_cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,value);
}

void
Vt102Display::setFontSize(int mysize)
{
    m_fontSize = mysize;
    m_currentFont.setPixelSize(m_fontSize);
    QTextEdit::setFont(m_currentFont);
}

void
Vt102Display::appendRawText(QString text)
{
    // parse for terminal control sequences
    if (!m_parserOn)
    {
//         qDebug() << "normal text mode";
        m_cursor.insertText(text);
    }
    else
    {
//         qDebug() << "slow text mode";
        int len(text.length());
        for (int i=0;i<len;i++)
        {
            receiveChar(text[i].unicode());
        }
    }
    qApp->processEvents();
}

void
Vt102Display::setRendition(RENDITION r)
{
    switch (r)
    {
        case RE_BOLD:
            m_format.setFontWeight(QFont::Bold);
            break;
        case RE_UNDERLINE:
            m_format.setFontUnderline(true);
            break;
        case RE_BLINK:
            break;
        case RE_REVERSE:
            ;
    }
}

#include <cstring>

void
Vt102Display::initTokenizer()
{
    int i;
    quint8* s;

    memset(m_tbl,0,sizeof(m_tbl));
    for(i =  0; i <  32; i++)
        m_tbl[ i] |= CTL;
    for(i =  0; i <  32; i++)
        m_tbl[ i] |= CTL;
    for(i = 32; i < 256; i++)
        m_tbl[ i] |= CHR;
    for(s = (quint8*)"@ABCDGHILMPSTXZcdfry"; *s; s++)
        m_tbl[*s] |= CPN;
    for(s = (quint8*)"t"; *s; s++)
        m_tbl[*s] |= CPS;
    for(s = (quint8*)"0123456789"; *s; s++)
        m_tbl[*s] |= DIG;
    for(s = (quint8*)"()+*%"; *s; s++)
        m_tbl[*s] |= SCS;
    for(s = (quint8*)"()+*#[]%"; *s; s++)
        m_tbl[*s] |= GRP;
    resetToken();
}

void
Vt102Display::receiveChar(int cc)
{
    int i;

    if (cc == 127) return; //VT100: ignore.

    if (ces(    CTL))
    {
        if (cc == CNTL('X') || cc == CNTL('Z') || cc == ESC)
        {
            // qDebug() << "0 a" << cc;
            resetToken(); //VT100: CAN or SUB
        }
        if (cc != ESC)
        {
            // qDebug() << "0 b" << cc;
            tau( TY_CTL(cc+'@' ),   0,  0);
            return;
        }
    }

    pushToToken(cc); // advance the state

    int* s = m_pbuf;
    int  p = m_ppos;

    if (lec(1,0,ESC))
    {
        // qDebug() << 1 << cc;
        return;
    }
    if (lec(1,0,ESC+128))
    {
        // qDebug() << 2 << cc;
        s[0] = ESC;
        receiveChar('[');
        return;
    }
    if (les(2,1,GRP))
    {
        // qDebug() << 3 << cc;
        return;
    }

    if (Xte)
    {
        // qDebug() << 4 << cc;
        resetToken();
        return;
    }

    if (Xpe)
    {
        // qDebug() << 5 << cc;
        return;
    }
    if (lec(3,2,'?'))
    {
        // qDebug() << 6 << cc;
        return;
    }
    if (lec(3,2,'>'))
    {
        // qDebug() << 7 << cc;
        return;
    }
    if (lec(3,2,'!'))
    {
        // qDebug() << 8 << cc;
        return;
    }
    if (lun() )
    {
        // qDebug() << 9 << cc;
        m_cursor.insertText(QChar(cc),m_format);
        // qDebug() << "9 a";
        resetToken();
        // qDebug() << "9 b";
        return;
    }
    if (lec(2,0,ESC))
    {
        // qDebug() << 10 << cc;
        tau( TY_ESC(s[1]), 0,  0);
        resetToken();
        return;
    }
    if (les(3,1,SCS))
    {
        // qDebug() << 11 << cc;
        tau( TY_ESC_CS(s[1],s[2]), 0,  0);
        resetToken();
        return;
    }
    if (lec(3,1,'#'))
    {
        // qDebug() << 12 << cc;
        tau( TY_ESC_DE(s[2]), 0,  0);
        resetToken();
        return;
    }
    if (eps(CPN))
    {
        int res(TY_CSI_PN(cc));
        // qDebug() << 13 << res << m_argv[0] << m_argv[1];
        tau(res, m_argv[0],m_argv[1]);
        resetToken();
        return;
    }

    // resize = \e[8;<row>;<col>t
    if (eps(CPS))
    {
        // qDebug() << 14 << cc;
        tau( TY_CSI_PS(cc, m_argv[0]), m_argv[1], m_argv[2]);
        resetToken();
        return;
    }

    if (epe())
    {
        // qDebug() << 15 << cc;
        tau( TY_CSI_PE(cc),0, 0);
        resetToken();
        return;
    }

    if (ees(DIG))
    {
        // qDebug() << 16 << cc;
        addDigit(cc-'0');
        return;
    }

    if (eec(';'))
    {
        // qDebug() << 17 << cc;
        addArgument();
        return;
    }

    for (i=0;i<=m_argc;i++)
    {
        // qDebug() << "18" << cc << m_argv[i] << m_argc << i;

        if ( epp( ))
        {
            // qDebug() << "18 a";
            tau( TY_CSI_PR(cc,m_argv[i]), 0,  0);
        }
        // spec. case for ESC]>0c or ESC]>c
        else if(egt())
        {
            // qDebug() << "18 b";
            tau( TY_CSI_PG(cc     ), 0,  0);
        }
        // ESC[ ... 48;2;<red>;<green>;<blue> ... m -or- ESC[ ... 38;2;<red>;<green>;<blue> ... m
        else if (cc == 'm' && m_argc - i >= 4 && (m_argv[i] == 38 || m_argv[i] == 48) && m_argv[i+1] == 2)
        {
            // qDebug() << "18 c";
            i += 2;
            tau( TY_CSI_PS(cc, m_argv[i-2]), 4, (m_argv[i] << 16) | (m_argv[i+1] << 8) | m_argv[i+2]);
            i += 2;
        }
         // ESC[ ... 48;5;<index> ... m -or- ESC[ ... 38;5;<index> ... m
        else if (cc == 'm' && m_argc - i >= 2 && (m_argv[i] == 38 || m_argv[i] == 48) && m_argv[i+1] == 5)
        {
            // qDebug() << "18 d";
            i += 2;
            tau( TY_CSI_PS(cc, m_argv[i-2]), 3, m_argv[i]);
        }
        else
        {
            // qDebug() << "18 e";
            tau( TY_CSI_PS(cc,m_argv[i]), 0,  0);
        }
        // qDebug() << "18 f";
    }
    // qDebug() << "19";
    resetToken();
//
    // qDebug() << "20";
}

void
Vt102Display::resetToken()
{
//     qDebug() << "Vt102Display::resetToken()";
    m_ppos = 0;
    m_argc = 0;
    m_argv[0] = 0;
    m_argv[1] = 0;
}

void
Vt102Display::pushToToken(int cc)
{
//     qDebug() << "Vt102Display::pushToToken()";
    m_pbuf[m_ppos] = cc;
    m_ppos = qMin(m_ppos+1,MAXPBUF-1);
}

void
Vt102Display::addDigit(int dig)
{
    m_argv[m_argc] = 10*m_argv[m_argc] + dig;
//     qDebug() << "addDigit()" << m_argv[m_argc] << m_argc;
}

void
Vt102Display::addArgument()
{
    m_argc = qMin(m_argc+1,MAXARGS-1);
//     qDebug() << "addArgument()" << m_argv[m_argc] << m_argc;
}

void
Vt102Display::setDefaultRendition()
{
    m_format.setFontWeight(QFont::Normal);
    m_format.setForeground(Qt::white);
    m_format.setBackground(Qt::black);
}

void
Vt102Display::tau( int token, int p, int q )
{
//     qDebug() << "Vt102Display::tau() 1";
    switch (token)
    {
        case TY_CHR():
//             qDebug() << "TY_CHR()";
            m_cursor.insertText(QChar(p),m_format);
            break; //UTF16
        case TY_CTL('H'):
//             qDebug() << "TY_CTL('H')";
            m_cursor.deletePreviousChar();
            break; //VT100
        case TY_CTL('I') :
            m_cursor.insertText(QChar('\t'),m_format);
//             moveRight(4);
            break; //VT100
        case TY_CTL('J') :
            // // qDebug() << "TY_CTL('J')";
            newline();
            break; //VT100
        case TY_CTL('K') :
            // qDebug() << "TY_CTL('K')";
            newline();
            break; //VT100
        case TY_CTL('L') :
            // qDebug() << "TY_CTL('L')";
            newline();
            break; //VT100
        case TY_CTL('M') :
//             qDebug() << "TY_CTL('M')";
            newline();
            break; //VT100
        case TY_CTL('X') :
            // qDebug() << "TY_CTL('X')";
            m_cursor.insertText(QChar(0x2592),m_format);
            break; //VT100
        case TY_CTL('Z') :
            // qDebug() << "TY_CTL('Z')";
            m_cursor.insertText(QChar(0x2592),m_format);
            break; //VT100
        case TY_ESC('E') :
            // qDebug() << "TY_CTL('E')";
            moveDown(1);
            break; //VT100
        case TY_ESC('M') :
//             qDebug() << "TY_CTL('M')";
            m_cursor.movePosition(QTextCursor::StartOfLine,QTextCursor::KeepAnchor);
            m_cursor.removeSelectedText();
            break; //VT100
        case TY_ESC('n') :
            // qDebug() << "TY_ESC('n')";
            newline();
            break; //VT100
        case TY_CSI_PS('m', 0):
            setDefaultRendition();
            break;
        case TY_CSI_PS('m', 1):
            setRendition(RE_BOLD);
            break;
        case TY_CSI_PS('m', 4):
            setRendition(RE_UNDERLINE);
            break;
        case TY_CSI_PS('m', 5):
            setRendition(RE_BLINK);
            break;
        case TY_CSI_PS('m', 7):
            setRendition(RE_REVERSE);
            break;
        case TY_CSI_PS('m', 30):
            setColor(30,true);
            break;
        case TY_CSI_PS('m', 31):
            // qDebug() << "color 31";
            setColor(31,true);
            break;
        case TY_CSI_PS('m', 32):
            // qDebug() << "color 32";
            setColor(32,true);
            break;
        case TY_CSI_PS('m', 33):
            setColor(33,true);
            break;
        case TY_CSI_PS('m', 34):
            setColor(34,true);
            break;
        case TY_CSI_PS('m', 35):
            setColor(35,true);
            break;
        case TY_CSI_PS('m', 36):
            setColor(36,true);
            break;
        case TY_CSI_PS('m', 37):
            setColor(37,true);
            break;
        case TY_CSI_PS('m', 38):
            setColor(38,true);
            break;
        case TY_CSI_PS('m', 39):
            setColor(0,false);
            break;
        case TY_CSI_PS('m', 40):
            setColor(30,false);
            break;
        case TY_CSI_PS('m', 41):
            setColor(31,false);
            break;
        case TY_CSI_PS('m', 42):
            setColor(32,false);
            break;
        case TY_CSI_PS('m', 43):
            setColor(33,false);
            break;
        case TY_CSI_PS('m', 44):
            setColor(34,false);
            break;
        case TY_CSI_PS('m', 45):
            setColor(35,false);
            break;
        case TY_CSI_PS('m', 46):
            setColor(36,false);
            break;
        case TY_CSI_PS('m', 47):
            setColor(37,false);
            break;
        case TY_CSI_PS('m', 48):
            setColor(38,false);
            break;
        case TY_CSI_PS('m', 49):
            setColor(31,false);
            break;
        case TY_CSI_PS('m', 90):
            setColor(31,true);
            break;
        case TY_CSI_PS('m', 91):
            setColor(32,true);
            break;
        case TY_CSI_PS('m', 92):
            setColor(33,true);
            break;
        case TY_CSI_PS('m', 93):
            setColor(34,true);
            break;
        case TY_CSI_PS('m', 94):
            setColor(35,true);
            break;
        case TY_CSI_PS('m', 95):
            setColor(36,true);
            break;
        case TY_CSI_PS('m', 96):
            setColor(37,true);
            break;
        case TY_CSI_PS('m', 97):
            setColor(38,true);
            break;
        case TY_CSI_PS('m',  100):
            setColor(30,false);
            break;
        case TY_CSI_PS('m',  101):
            setColor(31,false);
            break;
        case TY_CSI_PS('m',  102):
            setColor(32,false);
            break;
        case TY_CSI_PS('m',  103):
            setColor(33,false);
            break;
        case TY_CSI_PS('m',  104):
            setColor(34,false);
            break;
        case TY_CSI_PS('m',  105):
            setColor(35,false);
            break;
        case TY_CSI_PS('m',  106):
            setColor(36,false);
            break;
        case TY_CSI_PS('m',  107):
            setColor(37,false);
            break;
        case TY_CSI_PN('@'):
            m_cursor.insertText(QChar(p));
            break;
        case TY_CSI_PN('A'):
            // qDebug() <<  "TY_CSI_PN('A')" << p;
//             m_cursor.joinPreviousEditBlock();
            moveUp( p ? p:1);
            break; //VT100
        case TY_CSI_PN('B'):
            moveDown(p);
            break; //VT100
        case TY_CSI_PN('C'):
        case TY_CSI_PN('I'):
            moveRight(p);
            break; //VT100
        case TY_CSI_PN('D'):
            moveLeft(p);
            break; //VT100
        case TY_CSI_PN('G'):
            setCursorX(p);
            break; //LINUX
        case TY_CSI_PN('H'):
            setCursorXY(q,p);
            break; //VT100
        default :
//             qDebug() << " unhandled char" << token << p << q;
            break;
    };
//     qDebug() << "Vt102Display::tau() 2";
}

void
Vt102Display::setColor(int termColor, bool fg)
{
    color_func func;
    QColor col;
    if (fg)
    {
        func = &QTextCharFormat::setForeground;
        col = Qt::white;
    }
    else
    {
        func = &QTextCharFormat::setBackground;
        col = Qt::black;
    }

    switch(termColor)
    {
        case tc_normal:
            CALL_MEMBER_FN(m_format,func)(col);
            break;
        case tc_red:
            CALL_MEMBER_FN(m_format,func)(Qt::red);
            break;
        case tc_green:
            CALL_MEMBER_FN(m_format,func)(Qt::green);
            break;
        case tc_yellow:
            CALL_MEMBER_FN(m_format,func)(Qt::yellow);
            break;
        case tc_blue:
            CALL_MEMBER_FN(m_format,func)(Qt::blue);
            break;
        case tc_pink:
            CALL_MEMBER_FN(m_format,func)(Qt::magenta);
            break;
        default:
            qFatal("unknown color found");
    }
}

void
Vt102Display::initSlots()
{
    QFontComboBox* box(m_page->mainWindow()->settingsPage()->m_generalView->m_fonts);
    int count(box->count());
    for (int i=0;i<count;i++)
    {
        box->setCurrentIndex(i);
        if (m_currentFont.family() == box->currentFont().family())
            break;
    }
    m_page->mainWindow()->settingsPage()->m_generalView->m_fontSize->setValue(m_fontSize);
    m_page->mainWindow()->settingsPage()->m_generalView->m_vt100Mode->setCheckState( m_parserOn);
    connect(m_page->mainWindow()->settingsPage()->m_generalView->m_fonts,
            SIGNAL(currentFontChanged(const QFont &)),
            this,
            SLOT(setFont(const QFont &)));

    connect(m_page->mainWindow()->settingsPage()->m_generalView->m_fontSize,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(setFontSize(int)));

    connect( m_page->mainWindow()->settingsPage()->m_generalView->m_vt100Mode,
            SIGNAL(stateChanged(int)),
            this,
            SLOT(setParserMode(int)));
}

MessagePage::MessagePage(MainWindow* main) :
        Page(main),
        m_thread(0)
{
    qDebug() << "MessagePage::MessagePage()";
    paludis::Log::get_instance()->set_log_level(paludis::ll_qa);
    paludis::Log::get_instance()->set_program_name("perturbis");
    m_display = new Vt102Display(this);
    QVBoxLayout* mylayout = new QVBoxLayout;
    mylayout->addWidget(m_display);
    mylayout->setMargin(0);
    setLayout(mylayout);

    m_master_fd = posix_openpt(O_RDWR | O_NOCTTY);
    grantpt(m_master_fd);
    unlockpt(m_master_fd);
    m_slave_fd = open(ptsname(m_master_fd), O_RDWR);
    m_stream.reset(new paludis::FDOutputStream(m_slave_fd));
    paludis::set_run_command_stdout_fds(m_slave_fd, m_master_fd);
    paludis::set_run_command_stderr_fds(m_slave_fd, m_master_fd);
    paludis::Log::get_instance()->set_log_stream(m_stream.get());
    fcntl(m_master_fd,F_SETFL,fcntl(m_master_fd,F_GETFL) | O_NONBLOCK);
    m_thread = new MessageThread(this, m_master_fd );

    connect(m_thread,
            SIGNAL(sendMessage(QString)),
            m_display,
            SLOT(appendRawText(QString)));

//     dup2(m_slave_fd,1);
    show();
}

MessagePage::~MessagePage()
{
    ::close(m_master_fd);
    ::close(m_slave_fd);
    if (m_thread)
    {
        m_thread->setPolling(false);
        m_thread->wait();
        delete m_thread;
    }
}

void
MessagePage::postCreate()
{
    m_display->initSlots();
}

void
MessagePage::appendRawText(QString text)
{
    m_display->appendRawText(text);
}

void
MessagePage::activatePage()
{
//     if (m_dirty)
//         onRefreshPage();
}

void
MessagePage::clearPage()
{
    m_display->clear();
}

void
MessagePage::refreshPage()
{
}

void
MessagePage::setPolling(bool on)
{
//     qDebug() << "MessagePage::setPolling()";
    m_thread->setPolling(on);
}
