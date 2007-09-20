
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

class Thread;

namespace pertubis
{
    class QTOutputStreamBuf;
    class QTOutputStream;


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
            int                                         master_fd;
            int                                         slave_fd;
            int                                         copy_fd;
    };
}

#endif
