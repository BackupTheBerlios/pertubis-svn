
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SEARCH_PAGE_H
#define _PERTUBIS_ENTRY_PROTECTOR_SEARCH_PAGE_H 1

#include "Page.hh"
#include <QString>
#include <QSet>

class QModelIndex;
class QTreeView;
class QLineEdit;
class QProgressBar;
class QPushButton;
class QTextBrowser;
class QSplitter;

namespace pertubis
{
    class SearchWindow;
    class SearchThread;
    class PackageModel;
    class PackageFilterModel;

    class SearchPage : public Page
    {
        Q_OBJECT
        public:

            SearchPage(QWidget* pobj, MainWindow * mainWindow);
            virtual ~SearchPage();

        public slots:

            void onViewUserInteraction(const QModelIndex & index);

            /// shows a succcessful search
            void displaySearchFinished(int count);

            /// start search
            void onSearch();

            /// stopping a search
            void onSearchStopped();

            /// displayes item details
            void displayDetails(QString details);

            void activatePage();

            void onRefreshPage() {m_dirty=false;}

            void restartFilters(const QSet<QString> & set);

        private:
            void loadSettings();
            void saveSettings();

            QProgressBar*           m_bar;
            PackageModel*           m_searchModel;
            QTreeView*              m_searchView;

            QTextBrowser*           m_details;
            SearchThread*           m_searchThread;
            QLineEdit*              m_line;
            QPushButton*            m_bStart;
            QSplitter*              m_hSplit;
            PackageFilterModel*     m_searchFilterModel;
    };
}

#endif
