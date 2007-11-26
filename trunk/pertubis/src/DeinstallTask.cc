
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
#include "DeinstallTask.hh"
#include "Item.hh"
#include "MessageOutput.hh"
#include <paludis/package_id.hh>
#include <paludis/util/set.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/set-impl.hh>
#include <libwrapiter/libwrapiter_forward_iterator.hh>

bool pertubis::DeinstallTask::available(Item* item) const
{
    return (item->available() && item->data(Item::io_installed).toInt() != Qt::Unchecked);
}

bool pertubis::DeinstallTask::changeStates(Item* item, int newState)
{
//     qDebug() << "DeinstallTask::changeParentStates - start" <<  newState;
    Item::Iterator iStart;
    Item::Iterator iEnd;
    Item::Iterator piStart;
    Item::Iterator piEnd;
    if (item->childCount() > 0)
    {
        iStart = item->childBegin();
        iEnd = item->childEnd();
        piStart = item->parent()->childBegin();
        piEnd = item->parent()->childEnd();
    }
    int i=0;
    switch (item->updateRange())
    {
        case Item::ur_parent:
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
            break;
        case Item::ur_child:
            if (item->data(Item::io_installed).toInt() != Qt::Unchecked)
                return false;
            switch (newState)
            {
                case Qt::Unchecked:
                    changeEntry(item->ID(),false);
                    item->setTaskState(m_taskid,Qt::Unchecked);

                    while(piStart != piEnd)
                    {
                        if ( (*piStart)->data(Item::io_selected).toList().value(m_taskid).toInt() != Qt::Unchecked )
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
                        QVariantList list((*piStart)->data(Item::io_selected).toList());
        //                 qDebug() << "DeinstallTask::changeChildStates - list" << list;
                        int mystate(list.value(m_taskid).toInt());
        //                 qDebug() << "DeinstallTask::changeChildStates - state" << mystate;
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
        case Item::ur_node:
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
                    ;
            }
        default:
            ;
    }
    return true;
}

void pertubis::DeinstallTask::startTask(const paludis::tr1::shared_ptr<paludis::Environment>& /*env*/,MessageOutput* /*output*/)
{
//     paludis::DepListOptions options;
//     if (m_task)
//         delete m_task;
//     m_task = new Install(this,main->getEnv().get(),options,main->getEnv()->default_destinations());
//     connect(m_task,
//             SIGNAL(sendMessage(QString)),
//             main->messages(),
//             SLOT(receiveMessage(QString)),
//             Qt::AutoConnection);
//
//     for (paludis::PackageIDSet::ConstIterator i(m_data.begin()), i_end(m_data.end());
//          i != i_end ; ++i)
//     {
//         m_task->add_exact_package(*i);
//     }
//     m_task->run();
//     m_data.empty();
}
