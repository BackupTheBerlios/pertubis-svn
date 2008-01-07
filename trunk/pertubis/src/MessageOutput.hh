
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

#include <QTextEdit>
#include <QThread>
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
    class MessageOutput : public Page
    {
        Q_OBJECT

        public:

            MessageOutput(QWidget* mywidget, MainWindow * );
            ~MessageOutput();

            void redirectOutput();
            void setPollingOn() { m_thread->setAtWork(true); if (!m_thread->isRunning()) m_thread->start();}
            void setPollingOff(){ m_thread->setAtWork(false);}

        public slots:

            void activatePage() {}
            void clear();
            void append(QString text);

        private:

            paludis::tr1::shared_ptr<paludis::FDOutputStream>   messages_stream;
            QTextEdit*                                          m_output;
            MessageThread*                                      m_thread;
            int                                                 m_master_fd;
            int                                                 m_slave_fd;
    };
}

#endif
