
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_GLSA_PARSER_H
#define _PERTUBIS_ENTRY_PROTECTOR_GLSA_PARSER_H 1

#include <QXmlDefaultHandler>

class QTextBrowser;

namespace pertubis
{
    /** \brief parses glsa.xml documents and transforms it to html
    *
    **/
    class GlsaParser : public QXmlDefaultHandler
    {
    public:
        GlsaParser(QTextBrowser *browser);

        bool startElement(const QString &namespaceURI,
                        const QString &localName,
                        const QString &qName,
                        const QXmlAttributes &attributes);
        bool endElement(const QString &namespaceURI,
                        const QString &localName,
                        const QString &qName);
        bool characters(const QString &str);
        bool fatalError(const QXmlParseException &exception);
        bool endDocument ();

    private:

        QString         m_unaffected;
        QString         m_vulnerable;
        QString         m_id;
        QString         m_table;
        QString         m_currentElement;
        QString         m_html;
        QTextBrowser*   m_browser;
        bool            m_putInTable;
        bool            m_dontUse;
        bool            m_isUnaffected;
        bool            m_isVulnerable;
    };
}

#endif
