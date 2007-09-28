
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_BASIC_QTSTREAMBUF_H
#define _PERTUBIS_ENTRY_PROTECTOR_BASIC_QTSTREAMBUF_H 1

#include <QTextEdit>
#include <QDebug>
#include <ostream>
#include <sys/time.h>

namespace pertubis
{
    class QTOutputStreamBuf : public std::streambuf
    {
        public:
            QTOutputStreamBuf( QTextEdit& e );
            virtual ~QTOutputStreamBuf() {}
        protected:

            virtual int sync( )
            {
                m_edit.append(m_buf);
                m_buf.clear();
                return 0;
            }

            virtual int_type overflow(int_type c)
            {
                if (c != EOF)
                {
                    char z = c;
                    m_buf.append(z);
                }
                return c;
            }

            virtual std::streamsize xsputn(const char* text,
                                           std::streamsize n)
            {
                m_buf.append(QString::fromLocal8Bit(text,n));
                return n;
            }

            QString      m_buf;
            QTextEdit&   m_edit;
    };

    class QTOutputStreamBase
    {
        protected:
            QTOutputStreamBuf m_buf;
        public:

            QTOutputStreamBase( QTextEdit& buf) :
                m_buf(buf)
            {
            }
    };

    class QTOutputStream :
        protected QTOutputStreamBase,
        public std::ostream
    {
        public:

        QTOutputStream( QTextEdit& edit) :
            QTOutputStreamBase(edit),
            std::ostream(&m_buf)
        {
        }
    };

    QTOutputStreamBuf::QTOutputStreamBuf( QTextEdit& e ) :
    m_edit(e)
    {
    }
}

#endif
