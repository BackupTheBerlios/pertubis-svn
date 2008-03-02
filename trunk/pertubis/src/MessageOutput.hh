
/* Copyright (C) 2007 Stefan Koegl
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_MESSAGE_OUTPUT_H
#define _PERTUBIS_ENTRY_PROTECTOR_MESSAGE_OUTPUT_H 1

#define MAXPBUF 80
#define MAXARGS 15

#include <QTextCharFormat>
#include <QTextCursor>
#include <QChar>
#include <QTextEdit>
#include <QThread>
#include <QColor>
#include "Page.hh"
#include <paludis/util/tr1_memory.hh>

namespace paludis
{
    class FDOutputStream;
}

namespace pertubis
{

    enum TERM_COLOR
    {
        tc_normal=0,
        tc_black=30,
        tc_red=31,
        tc_green=32,
        tc_yellow=33,
        tc_blue=34,
        tc_pink=35
    };

    /*! \brief messages transport thread
     * \ingroup Thread
     *
     * We only have to start this thread while performing tasks.
     * Otherwise we will not see any output from paludis, gcc, etc
     *
     */
    class MessageThread : public QThread
    {
        Q_OBJECT

        public:

            MessageThread(QObject* pobj, int fd);
            ~MessageThread();

            /// starts the message polling and transport
            void setPolling(bool on);
            bool polling() const { return m_polling;}

        protected:

            void run();

        signals:

            void sendMessage(QString message);

        private:

            int         m_master_fd;
            bool        m_polling;
    };

    class MessagePage;

    /** \brief a non-interactive vt102 display
     *
     * that's only an non-interactive vt102 display and shows colored paludis task output as you are used to see on your terminal.
     * It parses the raw text input from paludis and proccesses the terminal escape codes.
     *
     * We are using some modified parsing logic from kde4 konsole in this class, so thanks to the kde4 konsole from me (hotshelf),
     */
    class Vt102Display :
        public QTextEdit
    {
        Q_OBJECT

        public:

            Vt102Display(MessagePage* page);
            ~Vt102Display();

        public slots:

            void appendRawText(QString text);

            void setFont(const QFont &);
            void setFontSize(int);
            void setParserMode(int mode);
            void initSlots();

        private:

            enum RENDITION
            {
                RE_BOLD=0,
                RE_UNDERLINE,
                RE_BLINK,
                RE_REVERSE
            };

//             struct Character
//             {
//                 QTextCharFormat _f;
//                 QChar _c;
            //
//                 Character() {}
//                 Character(const QTextCharFormat & f,QChar c) : _f(f),_c(c) {}
//             };

            void loadSettings();
            void saveSettings();

            void receiveChar(int chr);

            void setRendition(RENDITION r);

            void newline();

            void setDefaultRendition();

//             void addRealText(const Character & c);

            void moveUp(int value);
            void moveDown(int value);
            void moveLeft(int value);
            void moveRight(int value);

            void tau(int, int, int);
            void addDigit(int dig);
            void addArgument();

            void addText(QString s);

            void initTokenizer();
            void resetToken();
            void pushToToken(int);

            void setColor(int color, bool fg);
            void setCursorX(int value);
            void setCursorY(int value);
            void setCursorXY(int x, int y);

//             QList<Character>                    m_cbuf;
            QFont                               m_currentFont;
            QTextCharFormat                     m_format;
            QTextCursor                         m_cursor;

            MessagePage*                        m_page;

            int                                 m_tbl[256];
            int                                 m_pbuf[MAXPBUF];
            int                                 m_argv[MAXARGS];
            int                                 m_argc;
            int                                 m_currentColor;
            int                                 m_fontSize;
            int                                 m_ppos;
            Qt::CheckState                      m_parserOn;
    };

    /*! \brief output window for messages from paludis
     *
     */
    class MessagePage : public Page
    {
        friend class Vt102Display;
        Q_OBJECT

        public:

            MessagePage(MainWindow * );
            ~MessagePage();

            void setPolling(bool on);
            void postCreate();

        public slots:

            void activatePage();
            void refreshPage();
            void clearPage();
            void appendRawText(QString text);

        private:

            paludis::tr1::shared_ptr<paludis::FDOutputStream>   m_stream;
            int                                                 m_master_fd;
            int                                                 m_slave_fd;
            Vt102Display*                                       m_display;
            MessageThread*                                      m_thread;
    };
}

#endif
