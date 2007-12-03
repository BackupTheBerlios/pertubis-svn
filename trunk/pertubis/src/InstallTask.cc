
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

#include "InstallTask.hh"
#include "Item.hh"
#include "ItemInstallTask.hh"
#include "MessageOutput.hh"
#include <paludis/package_id.hh>
#include <paludis/util/set.hh>
#include <paludis/util/set-impl.hh>
#include <paludis/util/stringify.hh>
#include <QDebug>

pertubis::InstallTask::InstallTask(QObject* pobject,
        QAction* myaction,
        QString tname) : Task(pobject,myaction,tname),m_task(0)
{
}

bool pertubis::InstallTask::available(Item* item) const
{
    return item->available();
}

bool pertubis::InstallTask::changeStates(Item* item, int newState)
{
    Item::Iterator iStart;
    Item::Iterator iEnd;
    Item::Iterator piStart;
    Item::Iterator piEnd;

    int i=0;
    switch (item->itemType())
    {
        case Item::it_parent:
            iStart = item->childBegin();
            iEnd = item->childEnd();
            switch (newState)
            {
                case Qt::PartiallyChecked:
                case Qt::Checked:
                    changeEntry(item->ID(),true);
                    item->setTaskState(m_taskid,Qt::PartiallyChecked);
                    if (item->bestChild() != 0)
                        item->bestChild()->setTaskState(m_taskid,Qt::Checked);
                    break;
                case Qt::Unchecked:
                    item->setTaskState(m_taskid,Qt::Unchecked);
                    while (iStart!= iEnd)
                    {
                        changeEntry((*iStart)->ID(),false);
                        (*iStart)->setTaskState(m_taskid,Qt::Unchecked);
                        ++iStart;
                    }
                    break;
                default:
                    ;
            }
            break;
        case Item::it_child:
            piStart = item->parent()->childBegin();
            piEnd = item->parent()->childEnd();
            switch (newState)
            {
                case Qt::Unchecked:
                    changeEntry(item->ID(),false);
                    item->setTaskState(m_taskid,Qt::Unchecked);
                    while(piStart != piEnd)
                    {
                        if ( (*piStart)->data(Item::io_selected).toList().value(m_taskid).toInt() != Qt::Unchecked )
                            ++i;
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
                        QVariantList list((*piStart)->data(Item::io_selected).toList());
        //                 qDebug() << "InstallTask::changeChildStates - list" << list;
                        int mystate(list.value(m_taskid).toInt());
        //                 qDebug() << "InstallTask::changeChildStates - state" << mystate;
                        if ( mystate != Qt::Unchecked )
                            ++i;
                        ++piStart;
                    }
                    if (i == item->parent()->childCount() )
                        item->parent()->setTaskState(m_taskid,Qt::Checked);
                    else
                        item->parent()->setTaskState(m_taskid,Qt::PartiallyChecked);
                    break;
                default:
                    ;
            }
            break;
        case Item::it_node_only:
            switch (newState)
            {
                case Qt::Unchecked:
                    changeEntry(item->ID(),false);
                    item->setTaskState(m_taskid,Qt::Unchecked);
                    break;
                case Qt::Checked:
                    changeEntry(item->ID(),true);
                    qDebug() << "InstallTask::changeStates - n 2";
                    item->setTaskState(m_taskid,Qt::Checked);
                    qDebug() << "InstallTask::changeStates - n 3";
                    break;
                default:
                    ;
            }
            break;
        default:
            ;
    }
    return true;
}

void pertubis::InstallTask::startTask(const paludis::tr1::shared_ptr<paludis::Environment>& env, MessageOutput* output)
{
    qDebug() << "pertubis::InstallTask::startTask()";
    paludis::DepListOptions options;
    if (m_task)
        delete m_task;
    m_task = new Install(this,env.get(),options,env->default_destinations());
    connect(m_task,
            SIGNAL(sendMessage(QString)),
            output,
            SLOT(append(QString)));

    connect(m_task,
            SIGNAL(finished()),
            this,
            SLOT(slotFinished()));

    for (paludis::PackageIDSet::ConstIterator i(m_data.begin()), i_end(m_data.end());
         i != i_end ; ++i)
    {
        m_task->add_exact_package(*i);
    }
    m_task->run();
}

void pertubis::InstallTask::slotFinished()
{
    qDebug() << "pertubis::InstallTask::slotFinished()";
    emit finished();
}
