
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_PAGE_H
#define _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_PAGE_H 1

#include "Page.hh"
#include <QSet>
#include <QString>

class QModelIndex;

class QTableView;
class QSplitter;

namespace pertubis
{
    class RepositoryListThread;
    class RepositoryInfoThread;
    class RepositoryListModel;
    class RepositoryInfoModel;
    class PertubisSyncTask;

    class RepositoryPage : public Page
    {
        Q_OBJECT
        public:

            RepositoryPage(QWidget* pobj, MainWindow * mainWindow);
            virtual ~RepositoryPage();

        signals:

            void sendFilters(QSet<QString> repos);

        public slots:

            /// shows a succcessful search
            void onRepositoryChanged( const QModelIndex& index );

            void displaySyncFinished();

            void activatePage();

            void restartFilters();

            void onRefreshPage();

            void onSync();

        private:
            void loadSettings();
            void saveSettings();

            QTableView*             m_repoListView;
            QTableView*             m_repoInfoView;
            RepositoryListThread*   m_repoListThread;
            RepositoryInfoThread*   m_repoInfoThread;
            RepositoryListModel*    m_repoListModel;
            RepositoryInfoModel*    m_repoInfoModel;
            PertubisSyncTask*       m_syncTask;
            QSplitter*              m_hSplit;
    };
}

#endif
