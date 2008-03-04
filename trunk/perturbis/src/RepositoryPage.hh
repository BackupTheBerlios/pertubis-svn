
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_PAGE_H
#define _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_PAGE_H 1

#include "Page.hh"
#include <QSet>
#include <QString>

class QModelIndex;

namespace perturbis
{
    struct RepositoryPagePrivate;

    class RepositoryPage : public Page
    {
        Q_OBJECT
        public:

            RepositoryPage(MainWindow * mainWindow);
            virtual ~RepositoryPage();

        signals:

            void sendFilters(QSet<QString> repos);

        public slots:

            void activatePage();

            void clearPage();

            void refreshPage();

            void onRepositoryChanged( const QModelIndex& index );

            void displaySyncFinished();

            void restartFilters();

            void onSync();

        private:
            void loadSettings();
            void saveSettings();

            RepositoryPagePrivate* const m_imp;
    };
}

#endif
