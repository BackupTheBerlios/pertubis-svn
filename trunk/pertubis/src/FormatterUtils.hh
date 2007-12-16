
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_UTILS_H
#define _PERTUBIS_ENTRY_PROTECTOR_UTILS_H 1

#include <QString>
#include <QDebug>
#include <QRegExp>
#include <string>


namespace pertubis
{

    /// colorizes a text string with html colors
    inline std::string color(const std::string& text, const std::string& color)
    {
        return std::string("<font color=\""+color+"\">"+text+"</font>");
    }

    /// colorizes a text string with html colors
    inline QString color(const QString& text, const QString& color)
    {
        return QString("<font color=\"%1\">%2</font>").arg(color).arg(text);
    }

    /// creates a html row with two columns
    inline std::string make_row(const std::string& col_1, const std::string& col_2,const std::string& attr="")
    {
        return std::string("<tr><td "+attr+">"+col_1+"</td><td>"+col_2+"</td></tr>\n");
    }

    /// creates a html row with two columns
    inline QString make_row(const QString& col_1, const QString& col_2, const QString& attr="")
    {
        return QString("<tr><td %1>%2</td><td>%3</td></tr>\n").arg(attr).arg(col_1).arg(col_2);
    }

    /// creates a html link
    inline std::string link(const std::string& link, const std::string& text)
    {
        // qt is sometimes a little bit strange. We must delete possible trailing slash to be able to render this link as a "QURL"
        std::string tmp2(link);
        std::string::reverse_iterator iter2 = tmp2.rbegin();
        if (*iter2 == '/')
            tmp2.erase(tmp2.begin()+tmp2.size()-1);
        std::string res = std::string("<a href=" + tmp2 + ">" + text + "</a>");
        return res;
    }



    /// creates a html header
    inline std::string header(const std::string& text)
    {
        return std::string("<h1>"+ text + "</h1>");
    }

    /// creates a bold html text
    inline std::string bold(const std::string& text)
    {
        return std::string("<b>"+text+"</b>");
    }

    /// creates a html text formatted for tooltips
    inline QString html_tooltip(const QString& desc, const QString& name="")
    {
        return QString("<html><h1><u>%1</u></h1><p>%2</p></html>").arg(name).arg(desc);
    }

//     inline QString make_table(int padding, int width1, int width2, QString bgcolor, QString fontcolor, const QString& rows)
//     {
//         return QString("<html>\n"
// "<body>\n"
// "    <table border=\"0\" summary=\"\" width=\"100%\" height=\"100%\" cellpadding=\"%1\">\n"
// "        <colgroup>\n"
// "            <col width=\"%2\">\n"
// "            <col width=\"%3\">\n"
// "        </colgroup>\n"
// "        <tr><th bgcolor=\"#5a3aca\"></th><th bgcolor=\"%4\" align=\"left\"><font color=\"%5\">%6</font></th>\n"
// "            </tr>\n"
// "            <tbody >\n"
// "            %7</tbody>\n"
// "        </table>\n"
// " </body>\n"
// "</html>\n").arg(padding).arg(width1).arg(width2).arg(bgcolor).arg(fontcolor).arg(rows);
}

#endif
