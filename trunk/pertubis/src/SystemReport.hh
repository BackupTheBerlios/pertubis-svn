
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SYSTEM_REPORT_H
#define _PERTUBIS_ENTRY_PROTECTOR_SYSTEM_REPORT_H 1

#include <paludis/report_task.hh>
#include <paludis/package_id-fwd.hh>
#include <paludis/package_id.hh>
#include "ThreadBase.hh"
#include "SystemReport-fwd.hh"

using namespace paludis;

namespace pertubis
{

    class Selections;

    class SystemReport :
        public ThreadBase,
        public paludis::ReportTask
    {
        Q_OBJECT

        public:

            SystemReport(QObject* obj,
                        tr1::shared_ptr<Environment> e,
                        Selections* selects) :
                ThreadBase(obj,e),
                ReportTask(e.get()),
                m_dSelections(selects),
                m_env(e.get()),
                m_totalCount(0),
                m_errorCount(0)
            {
            }

            virtual void on_report_all_pre();
            virtual void on_report_check_package_pre(const QualifiedPackageName & p);
            virtual void on_report_package_success(const tr1::shared_ptr<const PackageID> & id);
            virtual void on_report_package_failure_pre(const tr1::shared_ptr<const PackageID> & id);
            virtual void on_report_package_is_masked(const tr1::shared_ptr<const PackageID> & id, const tr1::shared_ptr<const PackageID> & origin);
            virtual void on_report_package_is_vulnerable_pre(const tr1::shared_ptr<const PackageID> & id);
            virtual void on_report_package_is_vulnerable(const tr1::shared_ptr<const PackageID> & id, const GLSADepTag & tag);
            virtual void on_report_package_is_vulnerable_post(const tr1::shared_ptr<const PackageID> & id);
            virtual void on_report_package_is_missing(const tr1::shared_ptr<const PackageID> & id, const RepositoryName & repo_name);
            virtual void on_report_package_is_unused(const tr1::shared_ptr<const PackageID> & id);
            virtual void on_report_package_failure_post(const tr1::shared_ptr<const PackageID> & id);
            virtual void on_report_check_package_post(const QualifiedPackageName & p);
            virtual void on_report_all_post();

            /// overloaded from QThread
            void run();

        signals:

            /// sends a status message from paludis api to main thread
            void finished(int total,int errors);

            /// requests  to append and show this package in main thread
            void appendPackage(Package* node);

            /// sends a glsa entry
//             void notifyAboutGLSA(QString name, QString path);

        private:

            Selections*                         m_dSelections;
            const Environment * const           m_env;
            ReportPackage*                      m_node;
            int                                 m_totalCount;
            int                                 m_errorCount;
    };
}

#endif
