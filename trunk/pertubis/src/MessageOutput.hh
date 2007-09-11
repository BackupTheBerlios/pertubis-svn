
/* Copyright (C) 2007 Stefan Koegl.
*
* This file is part of pertubis.
*
* pertubis is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* pertubis is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http:*www.gnu.org/licenses/>.
*/

#ifndef _PERTUBIS_ENTRY_PROTECTOR_MESSAGE_OUTPUT_H
#define _PERTUBIS_ENTRY_PROTECTOR_MESSAGE_OUTPUT_H

#include <QTextEdit>
#include <QThread>
#include <paludis/util/tr1_memory.hh>
#include <sstream>

class QTextEdit;

// namespace paludis
// {
//     class FDOutputStream;
// }
// #include <paludis/util/fd_output_stream.hh>

namespace pertubis
{
    /*! \brief This thread checks from time to time possible information and output from paludis and injects it into MessageOutput
    *
    */
    class Digger : public QThread
    {
        Q_OBJECT
        public:
            Digger();
            ~Digger() {}

        protected:

            void setDoIt(bool yo) { m_doIt = yo;}
            void run();

        private:
            paludis::tr1::shared_ptr<std::ostringstream>      m_ostream;
            bool                                              m_doIt;
        signals:
            void newText(QString text);
    };


    /*! \brief terminak like output window for status messages from paludis
    *
    */
    class MessageOutput : public QWidget
    {
        Q_OBJECT

        public:

            MessageOutput(QWidget* mywidget);
//             void set_capture_output_options();
            ~MessageOutput();

        private slots:

            void printText(QString text);

        private:

            QTextEdit*                                  m_output;

            paludis::tr1::shared_ptr<Digger>            m_thread;
            int                                         m_master_fd;
            int                                         m_slave_fd;
    };
}

#endif

