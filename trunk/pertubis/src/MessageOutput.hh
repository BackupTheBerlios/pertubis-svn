
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
*
* This file is part of the pertubis frontend for paludis package manager.
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
#define _PERTUBIS_ENTRY_PROTECTOR_MESSAGE_OUTPUT_H

#include <QTextEdit>
#include <QThread>
#include <paludis/util/tr1_memory.hh>

class QTextEdit;

namespace paludis
{
    class FDOutputStream;
}



namespace pertubis
{
    class QTOutputStreamBuf;
    class QTOutputStream;

    class Thread : public QThread
    {
        Q_OBJECT
        public:
            Thread(QObject* pobj,  QTextEdit* output,int fd) : QThread(pobj), m_output(output), m_fd(fd),m_atwork(true)
            {
            }

            void setAtWork(bool t) { m_atwork = t;}
            bool atWork() const { return m_atwork;}

        protected:

            void run()
            {
                while (m_atwork)
                {
                    static char buf[256];
                    int res = read(m_fd,&buf,256);
                    m_output->append(QString::fromLocal8Bit(buf,res));
                    msleep(50);
                }
            }

        private:

            QTextEdit*  m_output;
            int         m_fd;
            bool        m_atwork;
    };


    /*! \brief output window for messages from paludis
    *
    */
    class MessageOutput : public QWidget
    {
        Q_OBJECT

        public:

            MessageOutput(QWidget* mywidget);
            ~MessageOutput();

            void append(QString text) { m_output->append(text);}
            QTextEdit* output() const { return m_output;}
            void redirectOutput_Simple();
            void redirectOutput_Paludis();
            void redirectOutput_Combined();

        private:

            QTextEdit*                                  m_output;
            Thread*                                     m_thread;
            paludis::tr1::shared_ptr<QTOutputStreamBuf> m_cout;
            paludis::tr1::shared_ptr<QTOutputStream>    m_input;
            paludis::tr1::shared_ptr<paludis::FDOutputStream> messages_stream;
            int                                         m_master_fd;
            int                                         m_slave_fd;
            int                                         m_copy_fd;
    };
}

#endif
