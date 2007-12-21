
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

#include "PackageDeinstallTask.hh"
#include "FormatterUtils.hh"
#include <paludis/package_database.hh>
#include <paludis/uninstall_list.hh>
#include <paludis/util/tr1_memory.hh>

void pertubis::PackageDeinstallTask::run()
{
    execute();
}

void pertubis::PackageDeinstallTask::on_build_unmergelist_pre()
{
    emit message("Building unmerge list... ");
}

void pertubis::PackageDeinstallTask::on_build_unmergelist_post()
{
    emit message("done... ");
}

void pertubis::PackageDeinstallTask::on_display_unmerge_list_pre()
{
    emit message(color( QString("These packages will be uninstalled:"),QString("green")));
}

void pertubis::PackageDeinstallTask::on_display_unmerge_list_post()
{
    if (m_error_count)
    {
        emit finished(m_count,m_error_count);
    }
    else
    {
        emit finished(m_count,0);
    }
}

void pertubis::PackageDeinstallTask::on_display_unmerge_list_entry(const paludis::UninstallListEntry & d)
{
    switch (d.kind)
    {
        case paludis::ulk_package:
            emit message(bold(paludis::stringify(*d.package_id)).c_str());
            ++m_count;
            break;

        case paludis::ulk_virtual:
//             cout << "* " << colour(cl_unimportant, stringify(*d.package_id));
            break;

        case paludis::ulk_required:
            emit message(color(paludis::stringify(*d.package_id),std::string("red")).c_str());
            ++m_error_count;
            break;

        case paludis::last_ulk:
            break;
    }
}

void pertubis::PackageDeinstallTask::on_uninstall_all_pre()
{
}

void pertubis::PackageDeinstallTask::on_uninstall_pre(const paludis::UninstallListEntry & d)
{
    using namespace paludis;
    std::string msg("(" + stringify(++m_current_count) + " of " +
            stringify(m_count) + ") Uninstalling " + stringify(*d.package_id));

    emit message(QString::fromStdString(color(msg,"green")));
}

void pertubis::PackageDeinstallTask::on_uninstall_post(const paludis::UninstallListEntry &)
{
}

void pertubis::PackageDeinstallTask::on_uninstall_all_post()
{
}

void pertubis::PackageDeinstallTask::on_not_continuing_due_to_errors()
{
    using namespace paludis;
    emit message( color(QString("Cannot continue with uninstall due to the errors indicated above"),QString("red")));
}

void pertubis::PackageDeinstallTask::on_update_world_pre()
{
    emit message( color(QString("Updating world file"),QString("green")));
}

void pertubis::PackageDeinstallTask::on_update_world(const paludis::PackageDepSpec & a)
{
    if (a.package_ptr())
        emit message( QString("* removing %1\n\n").arg( QString::fromStdString(color( paludis::stringify(*a.package_ptr()),std::string("green") ))));
}

void pertubis::PackageDeinstallTask::on_update_world(const paludis::SetName & a)
{
    emit message( QString("* removing %1\n\n").arg(color(paludis::stringify(a),std::string("green")).c_str() ));
}

void pertubis::PackageDeinstallTask::on_update_world_post()
{
    emit message("\n\n");
}

void pertubis::PackageDeinstallTask::on_preserve_world()
{
    emit message(color(QString("Updating world file\n* --preserve-world was specified, skipping world changes"),QString("green") ));
}
