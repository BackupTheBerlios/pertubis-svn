
/* Copyright (C) 2007 Stefan Koegl.
*
* This file is part of pertubis.
*
* pertubis is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* pertubis is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http:*www.gnu.org/licenses/>.
*/

#include <QDebug>
#include "InstallTask.hh"
#include "Item.hh"
#include "DatabaseView.hh"
#include "ItemInstallTask.hh"
#include <paludis/package_id.hh>
#include <paludis/util/set.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/set-impl.hh>
#include <libwrapiter/libwrapiter_forward_iterator.hh>

bool pertubis::InstallTask::available(Item* item) const
{
    return item->available();
}

bool pertubis::InstallTask::changeParentStates(Item* item, int newState)
{
//     qDebug() << "InstallTask::changeParentStates - start" <<  newState;
    QList<Item*>::iterator iStart(item->childBegin());
    QList<Item*>::iterator iEnd(item->childEnd());
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
            while(iStart != iEnd)
            {
                changeEntry((*iStart)->ID(),false);
                (*iStart)->setTaskState(m_taskid,Qt::Unchecked);
                ++iStart;
            }
            break;
        default:
            ;
    }
    return true;
}

bool pertubis::InstallTask::changeChildStates(Item* item, int newState)
{
//     qDebug() << "InstallTask::changeChildStates - start" <<  newState;
    QList<Item*>::iterator iStart(item->parent()->childBegin());
    QList<Item*>::iterator iEnd(item->parent()->childEnd());

    int i=0;
    switch (newState)
    {
        case Qt::Unchecked:
            changeEntry(item->ID(),false);
            item->setTaskState(m_taskid,Qt::Unchecked);

            while(iStart != iEnd)
            {
                if ( (*iStart)->data(Item::io_selected).toList().value(m_taskid).toInt() != Qt::Unchecked )
                    ++i;
                ++iStart;
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
            while(iStart != iEnd)
            {
                QVariantList list((*iStart)->data(Item::io_selected).toList());
//                 qDebug() << "InstallTask::changeChildStates - list" << list;
                int mystate(list.value(m_taskid).toInt());
//                 qDebug() << "InstallTask::changeChildStates - state" << mystate;
                if ( mystate != Qt::Unchecked )
                    ++i;
                ++iStart;
            }
            if (i == item->parent()->childCount() )
                item->parent()->setTaskState(m_taskid,Qt::Checked);
            else
                item->parent()->setTaskState(m_taskid,Qt::PartiallyChecked);
            break;
        default:
            ;
    }
    return true;
}

void pertubis::InstallTask::startTask(DatabaseView* main)
{
    paludis::DepListOptions options;
    if (m_task)
        delete m_task;
    m_task = new Install(this,main,options,main->getEnv()->default_destinations());
    for (paludis::PackageIDSet::ConstIterator i(m_data.begin()), i_end(m_data.end());
         i != i_end ; ++i)
    {
        m_task->add_target("="+paludis::stringify(*i->get()));
    }
    m_task->run();
    m_data.empty();
}
