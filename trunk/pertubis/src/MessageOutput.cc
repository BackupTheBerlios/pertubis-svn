
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
*
* This file is part of pertubis
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
#include <QColor>
#include <QDebug>
#include <QFile>
#include <QFontComboBox>
#include <QLayout>
#include <QSettings>
#include <QSpinBox>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextEdit>
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

void pertubis::MessageThread::run()
{
    char buf[512];
    while (m_atwork)
    {
        int res = read(m_fd,&buf,512);
        if (res > 0)
        {
            sendMessage(QString::fromLocal8Bit(buf,res));
        }
        msleep(100);
    }
}

pertubis::MessagePage::MessagePage(QWidget* pobj, MainWindow* main) :
        Page(pobj,main),
        m_thread(0),
        m_master_fd(-1),
        m_slave_fd(-1)
{
    m_output = new QTextEdit(pobj);
    m_cursor = m_output->textCursor();
    redirectOutput();
    paludis::Log::get_instance()->set_log_level(paludis::ll_qa);
    paludis::Log::get_instance()->set_program_name("pertubis");

//     m_output->setReadOnly(true);
    m_output->document()->setMaximumBlockCount(1000);
    QPalette p(palette());
    p.setColor(QPalette::Base,QColor(0,0,0)); // background color  = black
    p.setColor(QPalette::Text,QColor(255,255,255)); // text color  = white
    m_output->setPalette(p);
    m_output->ensureCursorVisible();
    m_output->setAutoFillBackground(true);
    show();
    m_currentColor = tc_normal;
    m_format.setForeground(Qt::white);
    QVBoxLayout* mylayout = new QVBoxLayout;
    mylayout->addWidget(m_output);
    mylayout->setMargin(0);
    setLayout(mylayout);

    connect(m_mainWindow->m_settingsPage->m_generalView->m_fonts,
            SIGNAL(currentFontChanged(const QFont &)),
            this,
            SLOT(setFont(const QFont &)));

    connect(m_mainWindow->m_settingsPage->m_generalView->m_fontSize,
            SIGNAL(valueChanged(int)),
            this,
            SLOT(setFontSize(int)));

    loadSettings();
    initTokenizer();

//     QFile file("fd_output_stream.txt");
//     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
//         return;
//     QString text(file.readAll());
//     file.close();
//     appendRawText(text);
}

pertubis::MessagePage::~MessagePage()
{
    if (m_thread)
    {
        m_thread->setAtWork(false);
        m_thread->wait();
        ::close(m_master_fd);
        ::close(m_slave_fd);
    }
    paludis::Log::get_instance()->set_log_stream(&std::cerr);
    saveSettings();
}

void pertubis::MessagePage::loadSettings()
{
    qDebug() << "pertubis::MessagePage::loadSettings() - start";
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "MessagePage" );
    m_currentFont = settings.value("font","Monospace,-1,12,5,50,0,0,0,0,0").value<QFont>();
    QFont test (settings.value("font").toString());
    m_fontSize = settings.value("fontSize",12).toInt();
    settings.endGroup();
    m_currentFont.setPixelSize(m_fontSize);
    m_output->setFont(m_currentFont);
    m_mainWindow->m_settingsPage->m_generalView->m_fonts->setCurrentFont(m_currentFont);
    m_mainWindow->m_settingsPage->m_generalView->m_fontSize->setValue(m_fontSize);
}

void pertubis::MessagePage::saveSettings()
{
    qDebug() << "pertubis::MessagePage::saveSettings() - start";
    QSettings settings("/etc/pertubis/pertubis.conf",QSettings::IniFormat);
    settings.beginGroup( "MessagePage" );
    settings.setValue("font", m_currentFont);
    settings.setValue("fontSize", m_fontSize);
    settings.endGroup();
    qDebug() << "pertubis::SearchPage::saveSettings() - done";
}

void pertubis::MessagePage::setFont(const QFont & myfont)
{
    m_currentFont = myfont;
    m_currentFont.setPixelSize(m_fontSize);
    qDebug() << "m_currentFont.pixelSize" << m_currentFont.pixelSize() << m_fontSize << m_currentFont.toString();
    m_output->setFont(m_currentFont);
}

void pertubis::MessagePage::moveUp(int value)
{
    int old(m_cursor.position());
    Q_ASSERT(m_cursor.movePosition(QTextCursor::Up,QTextCursor::MoveAnchor,value));
    int start(m_cursor.position());
    qDebug()<<  "moveUp()" << old << start;
}

void pertubis::MessagePage::setCursorX(int value)
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

void pertubis::MessagePage::setCursorY(int value)
{
    m_cursor.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
    m_cursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,value);
}

void pertubis::MessagePage::setCursorXY(int vx, int vy)
{
    m_cursor.movePosition(QTextCursor::Start,QTextCursor::MoveAnchor);
    moveDown(vy);
    setCursorX(vx);
}

void pertubis::MessagePage::moveDown(int value)
{
    m_cursor.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor,value);
}

void pertubis::MessagePage::moveLeft(int value)
{
    m_cursor.movePosition(QTextCursor::Left,QTextCursor::MoveAnchor,value);
}

void pertubis::MessagePage::moveRight(int value)
{
    m_cursor.movePosition(QTextCursor::Right,QTextCursor::MoveAnchor,value);
}

void pertubis::MessagePage::setFontSize(int mysize)
{
    m_fontSize = mysize;
    m_currentFont.setPixelSize(m_fontSize);
    m_output->setFont(m_currentFont);
}

void pertubis::MessagePage::appendRawText(QString text)
{
    // parse for terminal control sequences
    for (int i=0;i<text.length();i++)
    {
        receiveChar(text[i].unicode());
    }
}

void pertubis::MessagePage::setRendition(RENDITION r)
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

void pertubis::MessagePage::initTokenizer()
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

void pertubis::MessagePage::receiveChar(int cc)
{
    int i;

    if (cc == 127) return; //VT100: ignore.

    if (ces(    CTL))
    {
        if (cc == CNTL('X') || cc == CNTL('Z') || cc == ESC)
        {
            qDebug() << "0 a";
            resetToken(); //VT100: CAN or SUB
        }
        if (cc != ESC)
        {
            qDebug() << "0 b";
            tau( TY_CTL(cc+'@' ),   0,  0);
            return;
        }
    }

    pushToToken(cc); // advance the state

    int* s = m_pbuf;
    int  p = m_ppos;

    if (lec(1,0,ESC))
    {
        qDebug() << 1 << cc;
        return;
    }
    if (lec(1,0,ESC+128))
    {
        qDebug() << 2 << cc;
        s[0] = ESC;
        receiveChar('[');
        return;
    }
    if (les(2,1,GRP))
    {
        qDebug() << 3 << cc;
        return;
    }

    if (Xte)
    {
        qDebug() << 4 << cc;
        resetToken();
        return;
    }

    if (Xpe)
    {
        qDebug() << 5 << cc;
        return;
    }
    if (lec(3,2,'?'))
    {
        qDebug() << 6 << cc;
        return;
    }
    if (lec(3,2,'>'))
    {
        qDebug() << 7 << cc;
        return;
    }
    if (lec(3,2,'!'))
    {
        qDebug() << 8 << cc;
        return;
    }
    if (lun(   ))
    {
        qDebug() << 9 << cc;
        m_cursor.insertText(QChar(cc),m_format);
        resetToken();
        return;
    }
    if (lec(2,0,ESC))
    {
        qDebug() << 10 << cc;
        tau( TY_ESC(s[1]), 0,  0);
        resetToken();
        return;
    }
    if (les(3,1,SCS))
    {
        qDebug() << 11 << cc;
        tau( TY_ESC_CS(s[1],s[2]), 0,  0);
        resetToken();
        return;
    }
    if (lec(3,1,'#'))
    {
        qDebug() << 12 << cc;
        tau( TY_ESC_DE(s[2]), 0,  0);
        resetToken();
        return;
    }
    if (eps(CPN))
    {
        int res(TY_CSI_PN(cc));
        qDebug() << 13 << res << m_argv[0] << m_argv[1];
        tau(res, m_argv[0],m_argv[1]);
        resetToken();
        return;
    }

    // resize = \e[8;<row>;<col>t
    if (eps(CPS))
    {
        qDebug() << 14 << cc;
        tau( TY_CSI_PS(cc, m_argv[0]), m_argv[1], m_argv[2]);
        resetToken();
        return;
    }

    if (epe())
    {
        qDebug() << 15 << cc;
        tau( TY_CSI_PE(cc),0, 0);
        resetToken();
        return;
    }

    if (ees(DIG))
    {
        qDebug() << 16 << cc;
        addDigit(cc-'0');
        return;
    }

    if (eec(';'))
    {
        qDebug() << 17 << cc;
        addArgument();
        return;
    }

    for (i=0;i<=m_argc;i++)
    {
        if ( epp( ))
        {
            qDebug() << "18 a" << cc;
            tau( TY_CSI_PR(cc,m_argv[i]), 0,  0);
        }
        // spec. case for ESC]>0c or ESC]>c
        else if(egt())
        {
            qDebug() << "18 b" << cc;
            tau( TY_CSI_PG(cc     ), 0,  0);
        }
        // ESC[ ... 48;2;<red>;<green>;<blue> ... m -or- ESC[ ... 38;2;<red>;<green>;<blue> ... m
        else if (cc == 'm' && m_argc - i >= 4 && (m_argv[i] == 38 || m_argv[i] == 48) && m_argv[i+1] == 2)
        {
            qDebug() << "18 c" << cc;
            i += 2;
            tau( TY_CSI_PS(cc, m_argv[i-2]), 4, (m_argv[i] << 16) | (m_argv[i+1] << 8) | m_argv[i+2]);
            i += 2;
        }
         // ESC[ ... 48;5;<index> ... m -or- ESC[ ... 38;5;<index> ... m
        else if (cc == 'm' && m_argc - i >= 2 && (m_argv[i] == 38 || m_argv[i] == 48) && m_argv[i+1] == 5)
        {
            qDebug() << "18 d" << cc;
            i += 2;
            tau( TY_CSI_PS(cc, m_argv[i-2]), 3, m_argv[i]);
        }
        else
        {
            qDebug() << "18 e" << cc << m_argv[i] << (i < m_argc-1?m_argv[i+1] : -9999) << (i > 0 ?m_argv[i-1] : 9999);
            tau( TY_CSI_PS(cc,m_argv[i]), 0,  0);
        }
        resetToken();
    }
    m_output->ensureCursorVisible();
}

void pertubis::MessagePage::resetToken()
{
    m_ppos = 0;
    m_argc = 0;
    m_argv[0] = 0;
    m_argv[1] = 0;
}

void pertubis::MessagePage::pushToToken(int cc)
{
    m_pbuf[m_ppos] = cc;
    m_ppos = qMin(m_ppos+1,MAXPBUF-1);
}

void pertubis::MessagePage::addDigit(int dig)
{
    m_argv[m_argc] = 10*m_argv[m_argc] + dig;
}

void pertubis::MessagePage::addArgument()
{
    m_argc = qMin(m_argc+1,MAXARGS-1);
    m_argv[m_argc] = 0;
}

void pertubis::MessagePage::setPollingOn()
{
    m_thread->setAtWork(true);
    if (!m_thread->isRunning())
        m_thread->start();
}

void pertubis::MessagePage::setPollingOff()
{
    m_thread->setAtWork(false);
}

void pertubis::MessagePage::append(QString text)
{
    m_output->append(text);
}

void pertubis::MessagePage::setDefaultRendition()
{
    m_format.setFontWeight(QFont::Normal);
    m_format.setForeground(Qt::white);
    m_format.setBackground(Qt::black);
}

void pertubis::MessagePage::tau( int token, int p, int q )
{
    switch (token)
    {
        case TY_CHR():
//             qDebug() << "TY_CHR()";
            m_cursor.insertText(QString(p),m_format);
            break; //UTF16
        case TY_CTL('H'):
//             qDebug() << "TY_CTL('H')";
            m_cursor.deletePreviousChar();
            break; //VT100
        case TY_CTL('I') :
            m_cursor.insertText(QString("\t"));
//             moveRight(4);
            break; //VT100
        case TY_CTL('J') :
            // // qDebug() << "TY_CTL('J')";
            m_cursor.insertText("\n",m_format);
            break; //VT100
        case TY_CTL('K') :
            // qDebug() << "TY_CTL('K')";
            m_cursor.insertText("\n",m_format);
            break; //VT100
        case TY_CTL('L') :
            // qDebug() << "TY_CTL('L')";
            m_cursor.insertText("\n",m_format);
            break; //VT100
        case TY_CTL('M') :
            // qDebug() << "TY_CTL('M')";
//             m_cursor.insertText("\n",m_format);
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
        case TY_ESC('n') :
            // qDebug() << "TY_ESC('n')";
            m_cursor.insertText(QString(p));
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
            qDebug() << " unhandled char" << token << p << q;
            break;
    };
}

void pertubis::MessagePage::setColor(int termColor, bool fg)
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

void pertubis::MessagePage::redirectOutput()
{
    m_master_fd = posix_openpt(O_RDWR | O_NOCTTY);
    grantpt(m_master_fd);
    unlockpt(m_master_fd);
    m_slave_fd = open(ptsname(m_master_fd), O_RDWR);
    messages_stream.reset(new paludis::FDOutputStream(m_slave_fd));
    paludis::set_run_command_stdout_fds(m_slave_fd, m_master_fd);
    paludis::set_run_command_stderr_fds(m_slave_fd, m_master_fd);

    fcntl(m_master_fd,F_SETFL,fcntl(m_master_fd,F_GETFL) | O_NONBLOCK);
    paludis::Log::get_instance()->set_log_stream(messages_stream.get());
    m_thread = new MessageThread(this,m_master_fd);
    connect(m_thread,
            SIGNAL(sendMessage(QString)),
            this,
            SLOT(appendRawText(QString)));
}

void pertubis::MessagePage::clear()
{
    m_output->clear();
}

