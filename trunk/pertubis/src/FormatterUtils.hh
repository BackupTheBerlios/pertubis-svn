
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_UTILS_H
#define _PERTUBIS_ENTRY_PROTECTOR_UTILS_H

#include <QString>
#include <string>

namespace pertubis
{
    inline std::string color(const std::string& text, const std::string& color)
    {
        return std::string("<font color=\""+ color +"\">" + text + "</font>");
    }

    inline QString color(const QString& text, const QString& color)
    {
        return QString("<font color=\"%1\">%2</font>").arg(color).arg(text);
    }

    inline QString make_row(const QString& col_1, const QString& col_2)
    {
        return QString("<tr><td>%1</td><td>%2</td></tr>").arg(col_1).arg(col_2);
    }

    inline std::string make_row(const std::string& col_1, const std::string& col_2)
    {
        return std::string("<tr><td>"+col_1 + "</td><td>" + col_2 +" </td></tr>");
    }

    inline QString link(const QString& link, const QString& text)
    {
        return QString("<a href=\"%1\">%2</a>").arg(link).arg(text);
    }

    inline std::string link(const std::string& link, const std::string& text)
    {
        return std::string("<a href=" + link + ">" + text + "</a>");
    }

    inline QString bold(const QString& text)
    {
        return QString("<b>%1</b>").arg(text);
    }

    inline std::string bold(const std::string& text)
    {
        return std::string("<b>"+text+"</b>");
    }
}


#endif
