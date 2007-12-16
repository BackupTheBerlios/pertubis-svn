
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_DEINSTALL_TASK_H
#define _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_DEINSTALL_TASK_H 1

#include <QThread>
#include <paludis/uninstall_task.hh>
#include <paludis/environment-fwd.hh>

namespace pertubis
{
    class PackageDeinstallTask :
            public QThread,
            public paludis::UninstallTask
    {
        Q_OBJECT


        public:
            PackageDeinstallTask(QObject* pobject,paludis::tr1::shared_ptr<paludis::Environment> e) :
                QThread(pobject),
                paludis::UninstallTask(e.get()),
                    m_count(0),
                    m_current_count(0),
                    m_error_count(0)
            {
            }

            void run();

            virtual void on_build_unmergelist_pre();

            virtual void on_build_unmergelist_post();

            virtual void on_display_unmerge_list_pre();

            virtual void on_display_unmerge_list_post();

            virtual void on_display_unmerge_list_entry(const paludis::UninstallListEntry & /*d*/);

            virtual void on_uninstall_all_pre();

            virtual void on_uninstall_pre(const paludis::UninstallListEntry & d);

            virtual void on_uninstall_post(const paludis::UninstallListEntry &);

            virtual void on_uninstall_all_post();

            virtual void on_not_continuing_due_to_errors();

            virtual void on_update_world_pre();
            virtual void on_update_world(const paludis::PackageDepSpec & a);

            virtual void on_update_world(const paludis::SetName & a);

            virtual void on_update_world_post();

            virtual void on_preserve_world();

        signals:

            void message(QString);
            void finished(int,int);

        private:
            int m_count;
            int m_current_count;
            int m_error_count;
    };
}

#endif


