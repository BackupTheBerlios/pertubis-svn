
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_PAGE_H
#define _PERTUBIS_ENTRY_PROTECTOR_PAGE_H 1

#include <QWidget>

namespace pertubis
{
    class MainWindow;

    class Page : public QWidget
    {
        Q_OBJECT
        public:
            Page(QWidget* pobj, MainWindow* main) :
                QWidget(pobj),
                m_mainWindow(main),
                m_dirty(false)
                {}

            virtual ~Page();

            void setDirty() { m_dirty=true;}

        signals:

            /// send this signal whenever the some tasks have finished
            void dirty();

        public slots:

            virtual void activatePage() = 0;

            virtual void onRefreshPage() = 0;

        protected:

            MainWindow*     m_mainWindow;
            bool            m_dirty;
    };
}

#endif

