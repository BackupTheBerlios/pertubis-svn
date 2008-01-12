
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SELECTION_PAGE_H
#define _PERTUBIS_ENTRY_PROTECTOR_SELECTION_PAGE_H 1

#include "Page.hh"

class QModelIndex;
class QTreeView;
class QTextBrowser;
class QSplitter;

namespace pertubis
{
    class SelectionModel;

    class SelectionPage : public Page
    {
        Q_OBJECT

        public:

            SelectionPage(QWidget* pobj, MainWindow * mainWindow);
            virtual ~SelectionPage();

            SelectionModel*         m_selectionModel;

        public slots:

            /// the work to do when a user clicks on the selection display
            void onSelectionViewUserInteraction(const QModelIndex & mix);

            void activatePage();

             /// displayes item details
            void displayDetails(QString text);

            /// start selection tasks
            void onShowSelections();

            void onStart();

            void onRefreshPage();

        private:

            void loadSettings();
            void saveSettings();

            QTreeView*              m_selectionView;
            QSplitter*              m_hSplit;
            QTextBrowser*           m_details;
    };
}

#endif
