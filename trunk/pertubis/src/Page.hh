
/* Copyright (C) 2007 Stefan Koegl
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_PAGE_H
#define _PERTUBIS_ENTRY_PROTECTOR_PAGE_H 1

#include <QWidget>

struct PagePrivate;

namespace perturbis
{
    class MainWindow;

    class Page : public QWidget
    {
        Q_OBJECT
        public:
            Page(MainWindow* main);

            virtual ~Page();

        signals:

            /// send this signal whenever the some tasks have finished
            void dirty();

        public slots:

            virtual void activatePage() = 0;

            /** \brief save memory when needed
             *
             * This is a prototype for a method which should actualize all temporary information stored in
             * and accociated with this page
             */
            virtual void refreshPage() = 0;

            /** \brief save memory when needed
             *
             * This is a prototype for a method which should clear all temporary information stored in
             * and accociated with this page
             */
            virtual void clearPage() = 0;

            bool used();
            bool outOfDate();

            void setUsed(bool b);
            void setOutOfDate(bool b);

        protected:

            MainWindow* mainWindow();

            PagePrivate* const    m_imp;
    };
}

#endif
