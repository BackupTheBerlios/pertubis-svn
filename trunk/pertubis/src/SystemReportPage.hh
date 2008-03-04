
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SYSTEM_REPORT_PAGE_H
#define _PERTUBIS_ENTRY_PROTECTOR_SYSTEM_REPORT_PAGE_H 1

#include "Page.hh"
#include <paludis/package_id-fwd.hh>
#include <paludis/util/tr1_memory.hh>

class QModelIndex;

namespace perturbis
{
    struct SystemReportPagePrivate;

    /*! \brief Displays the result of an system report
     *
     * \see SystemReport
     * \ingroup Page
     * \warning interface is not freezed
     */
    class SystemReportPage : public Page
    {
        Q_OBJECT

        public:

            /// std constructor
            SystemReportPage(MainWindow * mainWindow);

            /// destructor
            virtual ~SystemReportPage();

        public slots:

            /// start the report task
            void activatePage();

        private slots:

            /// shows the statistics for a system report in statusbar
            void displaySystemReportFinished(int total, int issues);

            /// handles user input
            void onViewUserInteraction(const QModelIndex & mix);

            /// starts a system report equal to paludis --report
            void onSystemReport();

            /// shows package metadata
            void displayDetails(QString details);

            /// redo a system report
            void refreshPage();

            /// clears all information and packages
            void clearPage();

            /// fetches package metadata
            void details(const paludis::tr1::shared_ptr<const paludis::PackageID> & id);

        private:

            void loadSettings();
            void saveSettings();

            SystemReportPagePrivate* m_imp;
    };
}

#endif
