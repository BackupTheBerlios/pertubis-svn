
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

#include <QDebug>
#include "DeinstallTask.hh"
#include "Settings.hh"
#include "InstallSettings.hh"
#include "UninstallSettings.hh"
#include "PackageDeinstallTask.hh"
#include "Package.hh"
#include "MessageOutput.hh"
#include <paludis/package_id.hh>
#include <paludis/util/set.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/set-impl.hh>
#include <libwrapiter/libwrapiter_forward_iterator.hh>

bool pertubis::DeinstallTask::available(Package* item) const
{
    return (item->available() && item->data(Package::po_installed).toInt() != Qt::Unchecked);
}

bool pertubis::DeinstallTask::changeStates(Package* item, int newState)
{
    Package::PackageIterator iStart;
    Package::PackageIterator iEnd;
    Package::PackageIterator piStart;
    Package::PackageIterator piEnd;
    int i=0;
    switch (item->itemType())
    {
        case Package::pt_parent:
            iStart = item->childBegin();
            iEnd = item->childEnd();
            switch (newState)
            {
                case Qt::Unchecked:
                    item->setTaskState(m_taskid,Qt::Unchecked);
                    while(iStart != iEnd)
                    {
                        changeEntry((*iStart)->ID(),false);
                        (*iStart)->setTaskState(m_taskid,Qt::Unchecked);
                        ++iStart;
                    }
                    break;
                case Qt::PartiallyChecked:
                case Qt::Checked:
                    changeEntry(item->ID(),true);
                    item->setTaskState(m_taskid,Qt::PartiallyChecked);
                    if (item->bestChild() != 0)
                        item->bestChild()->setTaskState(m_taskid,Qt::Checked);
                    break;
                default:
                    break;
            }
            break;
        case Package::pt_child:
            if (item->data(Package::po_installed).toInt() == Qt::Unchecked)
                return false;
            piStart = item->parent()->childBegin();
            piEnd = item->parent()->childEnd();
            switch (newState)
            {
                case Qt::Unchecked:
                    changeEntry(item->ID(),false);
                    item->setTaskState(m_taskid,Qt::Unchecked);
                    while(piStart != piEnd)
                    {
                        if ( Qt::Checked == (*piStart)->data(Package::po_selected).toList().value(m_taskid).toInt() )
                        {
                            ++i;
                            break;
                        }
                        ++piStart;
                    }
                    if (i  == 0)
                        item->parent()->setTaskState(m_taskid,Qt::Unchecked);
                    else
                        item->parent()->setTaskState(m_taskid,Qt::PartiallyChecked);
                    break;
                case Qt::PartiallyChecked:
                case Qt::Checked:
                    changeEntry(item->ID(),true);
                    item->setTaskState(m_taskid,Qt::Checked);
                    while(piStart != piEnd)
                    {
                        QVariantList list((*piStart)->data(Package::po_selected).toList());
                        if ( Qt::Unchecked != list.value(m_taskid).toInt() )
                            ++i;
                        ++piStart;
                    }
                    if (i == item->parent()->childCount() )
                        item->parent()->setTaskState(m_taskid,Qt::Checked);
                    else
                        item->parent()->setTaskState(m_taskid,Qt::PartiallyChecked);
                    break;
                default:
                    break;
            }
            break;
        case Package::pt_node_only:
            switch (newState)
            {
                case Qt::Unchecked:
                    changeEntry(item->ID(),false);
                    item->setTaskState(m_taskid,Qt::Unchecked);
                    break;
                case Qt::Checked:
                    changeEntry(item->ID(),true);
                    item->setTaskState(m_taskid,Qt::Checked);
                    break;
                default:
                    break;
            }
        default:
            break;
    }
    return true;
}

void pertubis::DeinstallTask::startTask(const paludis::tr1::shared_ptr<paludis::Environment>& env,Settings* settings,MessageOutput* output)
{
    qDebug() << "pertubis::DeinstallTask::startTask() - start";
    if (m_data.size() > 0)
    {
        if (m_task)
            delete m_task;
        m_task = new PackageDeinstallTask(this,env);
        m_task->set_pretend(settings->m_installView->m_model->install_args.a_pretend.specified());
        m_task->set_no_config_protect(settings->m_installView->m_model->install_args.a_no_config_protection.specified());
        m_task->set_preserve_world(settings->m_installView->m_model->install_args.a_preserve_world.specified());
        m_task->set_with_unused_dependencies(settings->m_deinstallView->m_model->m_unusedDeps);
        m_task->set_with_dependencies(settings->m_deinstallView->m_model->m_deps);
        m_task->set_check_safety(! settings->m_deinstallView->m_model->m_unsafeUninstall);
        m_task->set_all_versions( settings->m_deinstallView->m_model->m_allVersions);

        connect(m_task,
                SIGNAL(message(QString)),
                output,
                SLOT(append(QString)));

        connect(m_task,
                SIGNAL(finished()),
                this,
                SIGNAL(finished()));

        for (paludis::PackageIDSet::ConstIterator i(m_data.begin()), i_end(m_data.end());
            i != i_end ; ++i)
        {
            std::string target("=" + stringify((*i)->name()) + "-" + (*i)->canonical_form(paludis::idcf_version));
            m_task->add_target(target);
        }
    }
    qDebug() << "pertubis::DeinstallTask::startTask() - done";
    m_task->run();
}
