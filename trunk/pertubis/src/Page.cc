
/* Copyright (C) 2007-2008 Stefan Koegl <hotshelf@users.berlios.de>
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

#include "Page.hh"
#include "MainWindow.hh"

struct PagePrivate
{
    PagePrivate(pertubis::MainWindow* main) :
        m_mainWindow(main),
        m_outOfDate(false),
        m_used(false)
    {
    }

    pertubis::MainWindow*     m_mainWindow;
    bool            m_outOfDate;

    /** \brief displays this page information to the user or not
            * set this attribute to true whenever information was fetched and could be out-of-date and so must be retrieved again
            * so it is
        */
    bool                  m_used;
};

pertubis::Page::Page(MainWindow* main) :
        QWidget( qobject_cast<QWidget*>(main) ),
        m_imp(new PagePrivate(main))
{
}

pertubis::Page::~Page()
{
    delete m_imp;
}

pertubis::MainWindow* pertubis::Page::mainWindow()
{
    return m_imp->m_mainWindow;
}

bool pertubis::Page::used()
{
    return m_imp->m_used;
}

bool pertubis::Page::outOfDate()
{
    return m_imp->m_outOfDate;
}

void pertubis::Page::setUsed(bool b)
{
    m_imp->m_used = b;
}

void pertubis::Page::setOutOfDate(bool b)
{
    m_imp->m_outOfDate = b;
}
