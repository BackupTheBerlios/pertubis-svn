
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_MESSAGE_OUTPUT_H
#define _PERTUBIS_ENTRY_PROTECTOR_MESSAGE_OUTPUT_H 1

#define MAXPBUF 80
#define MAXARGS 15

#include <QTextCharFormat>
#include <QTextCursor>
#include <QThread>
#include <QColor>
#include "Page.hh"
#include <paludis/util/tr1_memory.hh>

class QTextEdit;


namespace paludis
{
    class FDOutputStream;
}

namespace pertubis
{
    /*! \brief transfering status messages from paludis api to pertubis
     * \ingroup Thread
     */

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

    class MessageThread : public QThread
    {
        Q_OBJECT
        public:
            MessageThread(QObject* pobj,  int fd) :
                QThread(pobj),
                        m_fd(fd),
                             m_atwork(false)
                             {
                             }

                             void setAtWork(bool t) { m_atwork = t;}
                             bool atWork() const { return m_atwork;}

        protected:

            void run();

        signals:

            void sendMessage(QString message);

        private:

            int         m_fd;
            bool        m_atwork;
    };

    /*! \brief output window for messages from paludis
     *
     */
    class MessagePage : public Page
    {
        Q_OBJECT

        public:

            MessagePage(QWidget* mywidget, MainWindow * );
            ~MessagePage();

            void redirectOutput();
            void setPollingOn();
            void setPollingOff();

            paludis::tr1::shared_ptr<paludis::FDOutputStream>   messages_stream;

        public slots:

            void activatePage() {}
            void clear();
            void append(QString text);
            void appendRawText(QString text);

            void onRefreshPage() {};

        private:

            enum RENDITION
            {
                RE_BOLD=0,
                RE_UNDERLINE,
                RE_BLINK,
                RE_REVERSE
            };

            void setRendition(RENDITION r);

            void setDefaultRendition();

            void moveUp(int value);
            void moveDown(int value);
            void moveLeft(int value);
            void moveRight(int value);

            void receiveChar(int chr);
            void tau(int, int, int);
            void addDigit(int dig);
            void addArgument();
            void loadSettings();
            void saveSettings();
            void initTokenizer();
            void resetToken();
            void pushToToken(int);

            void setColor(int color, bool fg);
            void setCursorX(int value);
            void setCursorY(int value);
            void setCursorXY(int x, int y);
            void setFont(const QFont &);
            void setFontSize(int);

            QFont                                               m_currentFont;
            QTextCharFormat                                     m_format;
            QTextCursor                                         m_cursor;

            QTextEdit*                                          m_output;
            MessageThread*                                      m_thread;
            int                                                 m_tbl[256];
            int                                                 m_argv[MAXARGS];
            int                                                 m_pbuf[MAXPBUF];
            int                                                 m_argc;
            int                                                 m_master_fd;
            int                                                 m_slave_fd;
            int                                                 m_currentColor;
            int                                                 m_newColor;
            int                                                 m_fontSize;
            int                                                 m_ppos;
    };
}

#endif
