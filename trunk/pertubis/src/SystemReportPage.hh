
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SYSTEM_REPORT_PAGE_H
#define _PERTUBIS_ENTRY_PROTECTOR_SYSTEM_REPORT_PAGE_H 1

#include "Page.hh"
#include "SystemReport-fwd.hh"

class QModelIndex;
class QTreeView;

namespace pertubis
{
    class ReportPackageModel;

    class SystemReportPage : public Page
    {
        Q_OBJECT

        public:

            SystemReportPage(QWidget* pobj, MainWindow * mainWindow);
            virtual ~SystemReportPage();

        public slots:

            /// start the report task
            void activatePage();

        private slots:

            /// shows the statistics for a system report in statusbar
            void displaySystemReportFinished(int total, int issues);

            /// handles user input
            void onViewUserInteraction(const QModelIndex & mix);

            void onSystemReport();

        private:

            void loadSettings();
            void saveSettings();

            ReportPackageModel*     m_reportModel;
            QTreeView*              m_reportView;
            SystemReport*           m_reportThread;
    };
}

#endif
