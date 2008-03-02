
/* Copyright (C) 2007-2008 Stefan Koegl
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

#include <paludis/util/tr1_memory.hh>
#include <paludis/package_id-fwd.hh>

class QModelIndex;

namespace pertubis
{
    struct SearchPagePrivate;

    class SearchPage : public Page
    {
        Q_OBJECT
        public:

            SearchPage(MainWindow * main);
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

            void refreshPage() {setOutOfDate(false);}

            void details(const paludis::tr1::shared_ptr<const paludis::PackageID> & id);

            void clearPage();

            void restartFilters(const QSet<QString> & set);

        private:
            void loadSettings();
            void saveSettings();

            SearchPagePrivate* const m_imp;
    };
}

#endif
