
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SETTINGS_H
#define _PERTUBIS_ENTRY_PROTECTOR_SETTINGS_H

class QStackedWidget;
class QListWidget;
class QListWidgetItem;

#include "Page.hh"

namespace pertubis
{
    class DepListSettingsView;
    class GeneralSettingsView;
    class InstallSettingsView;
    class QuerySettingsView;
    class DeinstallSettingsView;

    /*! \brief Presents a settings window with different configuration groups.
    *
    */
    class SettingsPage : public Page
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            SettingsPage(QWidget* parent,MainWindow* main);
            ///@}

            ~SettingsPage();

            ///@name Content information
            ///@{

             /// loads all settings for the gui
            void loadSettings();

            /// saves all settings for the gui
            void saveSettings();

            /// the language settings page
            GeneralSettingsView*   m_generalView;

            /// the install settings page
            InstallSettingsView*    m_installView;

            /// the deinstall settings page
            DeinstallSettingsView*    m_deinstallView;

            /// the language settings page
            QuerySettingsView*    m_queryView;

            /// the deplist settings page
            DepListSettingsView*    m_depListView;

        public slots:

            /// changes the actual settings page to display to current
            void changePage(QListWidgetItem *current, QListWidgetItem *previous);

            void activatePage();

            void onRefreshPage() {}

        private:

            /// a list widget
            QListWidget*        m_pagesView;

            /// stacked pages
            QStackedWidget*     m_pagesStore;

    };
}

#endif
