
/* Copyright (C) 2007-2008 Stefan Koegl
*
* This file is part of perturbis
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

using namespace perturbis;

static QString
sign(QString op)
{
    if (op == "le")
        return "&lt;=";
    else if (op == "lt")
        return "&lt;";
    else if (op == "eq")
        return "=";
    else if (op == "gt")
        return "&gt;";
    else if (op == "ge")
        return "&gt;=";
    else if (op == "rlt")
        return "revision &lt;";
    else if (op == "rle")
        return "revision &lt;=";
    else if (op == "rgt")
        return "revision &gt;";
    else if (op == "rge")
        return "revision &gt;=";
    else
        return "";
}

GlsaParser::GlsaParser(QTextBrowser *browser) : m_browser(browser),m_putInTable(false), m_dontUse(false), m_isUnaffected(false),m_isVulnerable(false)
{
    m_html ="<html><head><link title=\"new\" rel=\"stylesheet\" type=\"text/css\" href=\"/usr/lib/perturbis/glsa.css\"></head><body><table border=\"0\" summary=\"\" width=\"100%\" height=\"100%\" cellpadding=\"25\"><colgroup><col width=\"100%\"></colgroup><tbody><tr><td>\n";
    m_table = "<table class=\"ntable\" border=\"0\"><tbody >\n";
}

bool
GlsaParser::startElement(const QString & /* namespaceURI */,
                              const QString & /* localName */,
                              const QString &qName,
                              const QXmlAttributes &attributes)
{
    if (qName == "glsa")
    {
        m_id = attributes.value("id");
    }
    else if (qName == "title")
    {
        m_currentElement.append("<h1>");
    }
    else if (qName == "resolution")
    {
        m_currentElement.append("<h1>resolution</h1>");
    }
    else if (qName == "product")
    {
        m_putInTable=true;
        m_currentElement.append("<tr><td class=\"infohead\">Advisory Reference</td><td class=\"tableinfo\">Glsa " + m_id + " / ");
    }
    else if (qName == "background")
    {
        m_currentElement.append("<h2>background</h2>");
    }
    else if (qName == "access")
    {
        m_putInTable=true;
        m_currentElement.append("<tr><td class=\"infohead\">Exploitale</td><td class=\"tableinfo\">");
    }
    else if (qName == "bug")
    {
        m_putInTable=true;
        m_currentElement.append("<tr><td class=\"infohead\">bug:</td><td class=\"tableinfo\">");
    }
    else if (qName == "announced")
    {
        m_putInTable=true;
        m_currentElement.append("<tr><td class=\"infohead\">Release Date</td><td class=\"tableinfo\">");
    }
    else if (qName == "metadata")
    {
        m_dontUse=true;
    }
    else if (qName == "uri")
    {
        m_currentElement.append("<p><a href=\""+ attributes.value("link") + "\">");
    }
    else if (qName == "revised")
    {
        m_putInTable=true;
        m_currentElement.append("<tr><td class=\"infohead\">Latest Revision</td><td class=\"tableinfo\">");
    }
    else if (qName == "references")
    {
        m_currentElement.append("<h2>references:</h2>");
    }
    else if (qName == "impact")
    {
        m_table.append("<tr><td class=\"infohead\">Impact</td><td class=\"tableinfo\">"+attributes.value("type") + "</td></tr>");
        m_currentElement.append("<h2>Impact:</h2>");
    }
    else if (qName == "workaround")
    {
        m_currentElement.append("<h2>workaround:</h2>");
    }
    else if (qName == "affected")
    {
        m_currentElement.append("<table class=\"ntable\"><tbody><tr><td class=\"infohead\">package</td><td class=\"infohead\">architectures</td><td class=\"infohead\">unaffected</td><td class=\"infohead\">vulnerable</td></tr>\n");
    }
    else if (qName == "package")
    {
        m_currentElement.append("<tr><td class=\"tableinfo\">"+attributes.value("name")+"</td><td class=\"tableinfo\" align=\"center\">"+attributes.value("arch")+"</td>");
    }
    else if (qName == "unaffected")
    {
        m_isUnaffected=true;
        m_unaffected.append(sign(attributes.value("range")));
    }
    else if (qName == "vulnerable")
    {
        m_isVulnerable=true;
        m_vulnerable.append(sign(attributes.value("range")));
    }
    else if (qName == "description")
    {
        m_currentElement.append("<h2>description:</h2>");
    }
    else if (qName == "code")
    {
        m_currentElement.append("<pre>");
    }
    else if (qName == "synopsis")
    {
        m_currentElement.append("<h2>synopsis: </h2><p>");
    }
    else if (qName == "p")
    {
        m_currentElement.append("<p>");
    }
    return true;
}

bool
GlsaParser::characters(const QString &str)
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

bool
GlsaParser::endElement(const QString & /* namespaceURI */,
                            const QString & /* localName */,
                            const QString &qName)
{
    if (m_dontUse)
    {
        m_dontUse = false;
        m_currentElement.clear();
        return true;
    }

    if (qName == "glsa")
    {
        m_currentElement.append("</td></tr></tbody></table></body></html>\n");
    }
    else if (qName == "p")
    {
        m_currentElement.append("</p>\n");
    }
    else if (qName == "product")
    {
        m_currentElement.append("</td></tr>\n");
    }
    else if (qName == "revised")
    {
        m_currentElement.append("</td></tr>\n");
    }
    else if (qName == "affected")
    {
        m_currentElement.append("</tbody></table>\n");
    }
    else if (qName == "package")
    {
        m_currentElement.append(QString("<td class=\"tableinfo\" >%1</td><td class=\"tableinfo\" >%2</td></tr>\n").arg(m_unaffected).arg(m_vulnerable));
    }
    else if (qName == "access")
    {
        m_currentElement.append("</td></tr>\n");
    }
    else if (qName == "bug")
    {
        m_currentElement.append("</td></tr>\n");
    }
    else if (qName == "uri")
    {
        m_currentElement.append("</a></p>\n");
    }
    else if (qName == "vulnerable")
    {
        m_vulnerable.append(" ");
    }
    else if (qName == "unaffected")
    {
        m_unaffected.append(" ");
    }
    else if (qName == "announced")
    {
        m_currentElement.append("</td></tr>");
    }
    else if (qName == "synopsis")
    {
        m_currentElement.append("</p>\n");
    }
    else if (qName == "title")
    {
        m_currentElement.append("</h1>%1\n");
    }
    else if (qName == "code")
    {
        m_currentElement.append("</pre>\n");
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

bool
GlsaParser::fatalError(const QXmlParseException &)
{
    return false;
}

bool
GlsaParser::endDocument()
{
    m_table.append("</tbody></table>");
    QString text(m_html.arg(m_table));
    m_browser->setHtml(text);
    return true;
}
