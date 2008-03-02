
/* Copyright (C) 2007 Stefan Koegl
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
#include <paludis/util/tr1_memory.hh>
#include <paludis/package_id-fwd.hh>


class QModelIndex;



namespace pertubis
{
    struct SelectionPagePrivate;

    /*! \brief Displays user selections and handles tasks
     *
     * \see InstallSelections, DeinstallSelections, PertubisInstallTask, PertubisDeinstallTask
     * \ingroup Selection
     * \ingroup Page
     * \warning interface is not freezed
     */
    class SelectionPage : public Page
    {
        Q_OBJECT

        public:

            /// std constructor
            SelectionPage(MainWindow * mainWindow);

            /// std destructor
            virtual ~SelectionPage();

        public slots:

            /// do something when the user switches to this paage
            void activatePage();

            /// refreshes the information displayed by this page
            void refreshPage();

            /// clears all information, saves memory
            void clearPage();

        private slots:

            /// clears all user selections
            void unselectAll();

            /// handles internal tasks after install tasks
            void installTaskFinished();

            /// handles internal tasks after deinstall tasks
            void deinstallTaskFinished();

            /// provide all neccessary information the install task needs
            void setupInstallTask(bool pretend, QString target ="");

            /// provide all neccessary information the deinstall task needs
            void setupDeinstallTask(bool pretend);

             /// displays item details
            void displayDetails(QString text);

            /// show the packages and acute issues for packages a user selected for all tasks
            void startPretend();

            void details(const paludis::tr1::shared_ptr<const paludis::PackageID> & id);

            /** \brief starts silently all tasks.
             *
             * \todo Users must be warned about what there will happen to their system, but provide some logic
             */
            void start();

            /// the work to do when a user clicks on the selection display
            void selectionViewUserInteraction(const QModelIndex & mix);

        private:

            /// loads user settings
            void loadSettings();

            /// saves user settings
            void saveSettings();

            /// implementation ptr
            SelectionPagePrivate* const     m_imp;
    };
}

#endif
