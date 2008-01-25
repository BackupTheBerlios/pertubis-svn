
/* Copyright (C) 2007-2008 Stefan Koegl <hotshelf@users.berlios.de>
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

#include "PertubisDeinstallTask.hh"
#include "Package.hh"
#include "Selections.hh"
#include "SelectionModel-fwd.hh"
#include "FormatterUtils.hh"
#include <paludis/action.hh>
#include <paludis/dep_tag.hh>
#include <paludis/package_database.hh>
#include <paludis/uninstall_list.hh>
#include <paludis/util/log.hh>
#include <paludis/util/tr1_memory.hh>
#include <iostream>

using std::cout;
using std::endl;

void pertubis::PertubisDeinstallTask::on_build_unmergelist_pre()
{
    cout << ("Building unmerge list... ") << endl;
}

void pertubis::PertubisDeinstallTask::on_build_unmergelist_post()
{
    cout << ("done... ") << endl;
}

void pertubis::PertubisDeinstallTask::on_display_unmerge_list_pre()
{
    cout << "\e[32;1m" << "These packages will be uninstalled:" << "\e[0;0m" << endl;
}

void pertubis::PertubisDeinstallTask::on_display_unmerge_list_post()
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

void pertubis::PertubisDeinstallTask::on_display_unmerge_list_entry(const paludis::UninstallListEntry & e)
{
    QVector<QVariant> data(spho_last);
    QVector<QVariant> vData(spho_last);
    Package* node(0);
    unsigned count(0), max_count(3);
    switch (e.kind)
    {
        case paludis::ulk_package:
            data[spho_install] = Qt::Unchecked;
            data[spho_deinstall] = Qt::Checked;
            data[spho_installed] = Qt::Checked;
            data[spho_package] = QString::fromStdString(paludis::stringify(e.package_id->name().package));
            data[spho_category] = QString::fromStdString(paludis::stringify(e.package_id->name().category));
            data[spho_old_version] = QString::fromStdString(paludis::stringify(e.package_id->version()));
            data[spho_repository] = QString::fromStdString(paludis::stringify(e.package_id->repository()->name()));
            emit appendPackage(new Package(e.package_id,data,ps_stable,pt_parent,0));
            ++m_count;
            break;

        case paludis::ulk_virtual:
            break;

        case paludis::ulk_required:
            data[spho_install] = m_install->hasEntry(e.package_id);
            data[spho_deinstall] = m_deinstall->hasEntry(e.package_id);
            data[spho_installed] = Qt::Checked;
            data[spho_package] = QString::fromStdString(paludis::stringify(e.package_id->name().package));
            data[spho_category] = QString::fromStdString(paludis::stringify(e.package_id->name().category));
            data[spho_old_version] = QString::fromStdString(paludis::stringify(e.package_id->version()));
            data[spho_repository] = QString::fromStdString(paludis::stringify(e.package_id->repository()->name()));
            node = new Package(e.package_id,data,ps_stable,pt_parent,0);
            for (paludis::Set<paludis::tr1::shared_ptr<paludis::DepTag> >::ConstIterator
                 tag(e.tags->begin()),
                     tag_end(e.tags->end()) ;
                     tag != tag_end ; ++tag)
            {
                if ("dependency" != (*tag)->category())
                    continue;

                if (++count < max_count)
                {
                    //TODO: use a "safe" paludis cast
                    paludis::DependencyDepTag * spec(static_cast<paludis::DependencyDepTag*>((*tag).get()));
                    Q_ASSERT(spec != 0);
                    vData.clear();
                    vData.resize(spho_last);
                    vData[spho_install] = Qt::Unchecked;
                    vData[spho_deinstall] = Qt::Checked;
                    vData[spho_installed] = Qt::Checked;
                    vData[spho_package] = "requires";
                    vData[spho_category] = QString::fromStdString(spec->package_id()->canonical_form(paludis::idcf_full));
                    node->appendChild(new Package(spec->package_id(),vData,ps_masked,pt_child,node));
                    ++max_count;
                }
            }
            ++m_error_count;
            emit appendPackage(node);
            break;

        case paludis::last_ulk:
            break;
    }
}

void pertubis::PertubisDeinstallTask::on_uninstall_all_pre()
{
}

void pertubis::PertubisDeinstallTask::on_uninstall_pre(const paludis::UninstallListEntry & d)
{
    using namespace paludis;
    std::string msg("(" + stringify(++m_current_count) + " of " +
            stringify(m_count) + ") Uninstalling " + stringify(*d.package_id));

    cout << "\e[0;0m" << msg << endl;
}

void pertubis::PertubisDeinstallTask::on_uninstall_post(const paludis::UninstallListEntry &)
{
}

void pertubis::PertubisDeinstallTask::on_uninstall_all_post()
{
}

void pertubis::PertubisDeinstallTask::on_not_continuing_due_to_errors()
{
    cout << "Cannot continue with uninstall due to the errors indicated above" << "\e[0;0m" << endl;
}

void pertubis::PertubisDeinstallTask::on_update_world_pre()
{
    cout << endl << "\e[35;1m" << "Updating world file" << "\e[0;0m" << endl;
}

void pertubis::PertubisDeinstallTask::on_update_world(const paludis::PackageDepSpec & a)
{
    if (a.package_ptr())
        cout << "\e[31;1m" << "* removing " <<  paludis::stringify(*a.package_ptr()) << "\e[0;0m" << endl;
}

void pertubis::PertubisDeinstallTask::on_update_world(const paludis::SetName & a)
{
    cout << "\e[32;1m" << "* removing color(paludis::stringify(a)" << "\e[0;0m" << endl;
}

void pertubis::PertubisDeinstallTask::on_update_world_post()
{
    cout << endl;
}

void pertubis::PertubisDeinstallTask::on_preserve_world()
{
    cout << endl << "\e[32;1m" << "Updating world file\n* --preserve-world was specified, skipping world changes" << "\e[0;0m" << endl;
}

void pertubis::PertubisDeinstallTask::run()
{
    try
    {
        execute();
    }
    catch(paludis::UninstallActionError & e)
    {
        cout << endl << e.what() << endl  << e.message() << endl << e.backtrace("\n") << endl;
    }
}
