
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

#include <QtGui>
#include <QDebug>

#include "GLSAParser.hh"

QString sign(QString op)
{
    if ( op == "le")
        return "&lt;=";
    else if ( op == "lt")
        return "&lt;";
    else if ( op == "eq")
        return "=";
    else if ( op == "gt")
        return "&gt;";
    else if ( op == "ge")
        return "&gt;=";
    else if ( op == "rlt")
        return "revision &lt;";
    else if ( op == "rle")
        return "revision &lt;=";
    else if ( op == "rgt")
        return "revision &gt;";
    else if ( op == "rge")
        return "revision &gt;=";
    else
        return "";
}

GLSAParser::GLSAParser(QTextBrowser *browser) : m_browser(browser),m_putInTable(false), m_dontUse(false)
{
    m_html ="<html><head><link title=\"new\" rel=\"stylesheet\" type=\"text/css\" href=\"/usr/lib/pertubis/glsa.css\"><title></title></head><body><table border=\"0\" summary=\"\" width=\"100%\" height=\"100%\" cellpadding=\"25\"><colgroup><col width=\"100%\"></colgroup><tbody><tr><td>\n";
//     m_html ="<html><head></head><body>";
    m_table = "<table class=\"ntable\" border=\"0\"><tbody >\n";



}

bool GLSAParser::startElement(const QString & /* namespaceURI */,
                              const QString & /* localName */,
                              const QString &qName,
                              const QXmlAttributes &attributes)
{
//     qDebug() << "<" << qName << ">";
    if (qName == "glsa")
    {
        m_id = attributes.value("id");
    }
    else if (qName == "title")
    {
        m_currentElement.append( "<h1>");
    }

    else if (qName == "resolution")
    {
        m_currentElement.append( "<h1>resolution</h1>");
    }
    else if (qName == "product")
    {
        m_putInTable=true;
        m_currentElement.append( "<tr><td class=\"infohead\">Advisory Reference</td><td class=\"tableinfo\">GLSA " + m_id + " / " );
    }

    else if (qName == "background")
    {
        m_currentElement.append( "<h2>background</h2>");
    }
    else if (qName == "access")
    {
        m_putInTable=true;
        m_currentElement.append( "<tr><td class=\"infohead\">Exploitale</td><td class=\"tableinfo\">");
    }
    else if (qName == "bug")
    {
        m_putInTable=true;
        m_currentElement.append( "<tr><td class=\"infohead\">bug:</td><td class=\"tableinfo\">");
    }
    else if (qName == "announced")
    {
        m_putInTable=true;
        m_currentElement.append( "<tr><td class=\"infohead\">Release Date</td><td class=\"tableinfo\">");
    }

    else if (qName == "metadata")
    {
        m_dontUse=true;
    }
    else if (qName == "uri")
    {
        m_currentElement.append( "<p><a href=\""+ attributes.value("link") + "\">");
    }
    else if (qName == "revised")
    {
        m_putInTable=true;
        m_currentElement.append( "<tr><td class=\"infohead\">Latest Revision</td><td class=\"tableinfo\">");
    }

    else if (qName == "references")
    {
        m_currentElement.append( "<h2>references:</h2>");
    }

    else if (qName == "impact")
    {
        m_table.append("<tr><td class=\"infohead\">Impact</td><td class=\"tableinfo\">"+attributes.value("type") + "</td></tr>" );
        m_currentElement.append( "<h2>Impact:</h2>");
    }
    else if (qName == "workaround")
    {
        m_currentElement.append( "<h2>workaround:</h2>");
    }
    else if (qName == "affected")
    {
        m_currentElement.append( "<table class=\"ntable\" border=\"0\"><tbody><tr><td class=\"infohead\">package</td><td class=\"infohead\">architectures</td><td class=\"infohead\">unaffected</td><td class=\"infohead\">vulnerable</td></tr>\n");
    }
    else if (qName == "package")
    {
        m_currentElement.append("<tr><td class=\"tableinfo\" align=\"center\">"+attributes.value("name")+"</td><td class=\"tableinfo\" align=\"center\">"+attributes.value("arch")+"</td>");
    }

    else if (qName == "unaffected")
    {
        m_isUnaffected=true;
        m_unaffected.append(sign(attributes.value("range")));
//         m_currentElement.append( "<td class=\"tableinfo\" align=\"center\">"+));
    }

    else if (qName == "vulnerable")
    {
        m_isVulnerable=true;
        m_vulnerable.append(sign(attributes.value("range")));
//         m_currentElement.append( "<td class=\"tableinfo\" align=\"center\">"+sign(attributes.value("range")));
    }
    else if (qName == "description")
    {
        m_currentElement.append( "<h2>description:</h2>");
    }
    else if (qName == "code")
    {
        m_currentElement.append( "<pre>");
    }

    else if (qName == "synopsis")
    {
        m_currentElement.append( "<h2>synopsis: </h2><p>");
    }
    else if (qName == "p")
    {
        m_currentElement.append( "<p>");
    }

    return true;
}

bool GLSAParser::characters(const QString &str)
{
    if (m_dontUse)
        return true;

    if (m_isUnaffected)
    {
        m_unaffected.append(str);
        m_isUnaffected=false;
    }
    else if (m_isVulnerable)
    {
        m_vulnerable.append(str);
        m_isVulnerable=false;
    }
    else
        m_currentElement += str;
    return true;
}

bool GLSAParser::endElement(const QString & /* namespaceURI */,
                            const QString & /* localName */,
                            const QString &qName)
{
    if (m_dontUse)
    {
        m_dontUse = false;
        m_currentElement.clear();
        return true;
    }
//     qDebug() << "</" << qName << ">";
    if (qName == "glsa")
    {
        m_currentElement.append("</td></tr></tbody></table></body></html>\n");
    }
    if (qName == "p")
    {
        m_currentElement.append( "</p>\n");
    }
    if (qName == "product")
    {
        m_currentElement.append( "</td></tr>\n");
    }
    if (qName == "revised")
    {
        m_currentElement.append( "</td></tr>\n");
    }
//     if (qName == "background")
//     {
//         m_currentElement.append( "</div class=\"test\">");
//     }

    if (qName == "affected")
    {
        m_currentElement.append( "</tbody></table>\n");
    }
    if (qName == "package")
    {
        m_currentElement.append( QString("<td class=\"tableinfo\" >%1</td><td class=\"tableinfo\" >%2</td></tr>\n").arg(m_unaffected).arg(m_vulnerable));
    }
    if (qName == "access")
    {
        m_currentElement.append( "</td></tr>\n");
    }
//     if (qName == "references")
//     {
//         m_currentElement.append( "</div>\n");
//     }
    if (qName == "bug")
    {
        m_currentElement.append( "</td></tr>\n");
    }
    if (qName == "uri")
    {
        m_currentElement.append( "</a></p>\n");
    }

    if (qName == "vulnerable")
    {
        m_vulnerable.append( " ");
    }
    if (qName == "unaffected")
    {
        m_unaffected.append( " ");
    }
//     if (qName == "impact")
//     {
//         m_currentElement.append( "</div>");
//     }
    if (qName == "description")
    {
//         m_currentElement.append( "</div>");
    }
//     if (qName == "resolution")
//     {
//         m_currentElement.append( "</div>");
//     }
//     if (qName == "workaround")
//     {
//         m_currentElement.append( "</div>");
//     }
    if (qName == "announced")
    {
        m_currentElement.append( "</td></tr>");
    }
    if (qName == "synopsis")
    {
        m_currentElement.append( "</p>\n");
    }
    if (qName == "title")
    {
        m_currentElement.append( "</h1>%1\n"); // later fill here in the table
    }
    if (qName == "code")
    {
        m_currentElement.append( "</pre>\n"); // later fill here in the table
    }

    if (m_putInTable)
    {
        m_table.append(m_currentElement);
    }
    else
    {
        m_html.append(m_currentElement);
    }
    m_putInTable=false;
    m_currentElement.clear();
    return true;
}

bool GLSAParser::fatalError(const QXmlParseException &exception)
{
    QMessageBox::warning(0, QObject::tr("SAX Handler"),
                         QObject::tr("Parse error at line %1, column "
                                     "%2:\n%3.")
                         .arg(exception.lineNumber())
                         .arg(exception.columnNumber())
                         .arg(exception.message()));
    return false;
}
/*
bool GLSAParser::startDocument()
{

    return true;
}*/

bool GLSAParser::endDocument()
{
    m_table.append("</tbody></table>");
    m_browser->setHtml(m_html.arg(m_table));
    return true;
}
