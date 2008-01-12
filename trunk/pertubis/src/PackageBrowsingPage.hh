
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_BROWSING_PAGE_H
#define _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_BROWSING_PAGE_H 1

#include "Page.hh"

#include <paludis/util/tr1_memory.hh>
#include <paludis/package_id-fwd.hh>
#include <QString>


class QModelIndex;
class QSplitter;
class QTextBrowser;
class QString;
class QTableView;
class QTreeView;


namespace pertubis
{
    class CategoryFilterModel;
    class CategoryModel;
    class PackageFilterModel;
    class Package;
    class PackagesThread;
    class CategoryItem;
    class MainWindow;
    class CategoryThread;
    class SetThread;
    class PackageModel;

    class PackageBrowsingPage : public Page
    {
        Q_OBJECT
        public:

            PackageBrowsingPage(QWidget* pobj, MainWindow * mainWindow);
            ~PackageBrowsingPage();

        public slots:

            void activatePage();

            void restartFilters(const QSet<QString> & set);

        private slots:

            void onCategoryChanged( const QModelIndex &);

            void onViewUserInteraction(const QModelIndex & index);

            void displayCategoryChanged();

            void onRefreshPage();

             /// displayes item details
            void displayDetails(QString text);

        private:
            void loadSettings();
            void saveSettings();
            void changeCategory();

            QString                 m_currentCat;
            CategoryFilterModel*    m_categoryFilterModel;
            CategoryModel*          m_catModel;
            CategoryThread*         m_categoryThread;
            PackageFilterModel*     m_packageFilterModel;
            Package*                m_current;
            PackageModel*           m_packageModel;
            PackagesThread*         m_packageViewThread;
            CategoryFilterModel*    m_setFilterModel;
            CategoryModel*          m_setModel;
            SetThread*              m_setThread;
            QTextBrowser*           m_details;
            QSplitter*              m_hSplit;
            QSplitter*              m_vSplit;
            QSplitter*              m_lSplit;
            QTableView*             m_categoryView;
            QTableView*             m_setView;
            QTreeView*              m_packageView;

    };
}

#endif
