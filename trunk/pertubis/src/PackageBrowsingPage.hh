
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

#include <QString>
#include "Page.hh"

class QModelIndex;
class QSplitter;
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

            void onPackageViewUserInteraction(const QModelIndex & index);

            void displayCategoryChanged();

        private:
            void loadSettings();
            void saveSettings();

            QString                 m_currentCat;
            CategoryFilterModel*    m_categoryFilterModel;
            CategoryModel*          m_catModel;
            CategoryThread*         m_categoryThread;
            PackageFilterModel*     m_packageFilterModel;
            Package*                m_current;
            PackageModel*           m_packageModel;
            PackagesThread*         m_packageViewThread;
            QSplitter*              m_hSplit;
            QTableView*             m_categoryView;
            QTreeView*              m_packageView;

    };
}

#endif
