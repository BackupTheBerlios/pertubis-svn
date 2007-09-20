
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
#include <paludis/package_id.hh>

bool pertubis::InstallTask::available(Item* item) const
{
    return item->available();
}

bool pertubis::InstallTask::changeParentStates(Item* item, bool newState)
{
//     qDebug() << "InstallTask::changeParentStates - start" <<  newState;
    QList<Item*>::iterator iStart(item->childBegin());
    QList<Item*>::iterator iEnd(item->childEnd());
    if (newState)
    {
        changeEntry(item->ID().get(),true);
        item->setTaskState(m_taskid,Qt::PartiallyChecked);
        if (item->bestChild())
            item->bestChild()->setTaskState(m_taskid,Qt::Checked);
    }
    else
    {
        item->setTaskState(m_taskid,Qt::Unchecked);
        while(iStart != iEnd)
        {
            changeEntry((*iStart)->ID().get(),false);
            (*iStart)->setTaskState(m_taskid,Qt::Unchecked);
            ++iStart;
        }
    }
//     qDebug() << "InstallTask::changeParentStates - done";
    return true;
}

bool pertubis::InstallTask::changeChildStates(Item* item, bool newState)
{
//     qDebug() << "InstallTask::changeChildStates - start" <<  newState;
    QList<Item*>::iterator iStart(item->parent()->childBegin());
    QList<Item*>::iterator iEnd(item->parent()->childEnd());
    int i=0;
    if (newState)
    {
        changeEntry(item->ID().get(),true);
        item->setTaskState(m_taskid,Qt::Checked);
        while(iStart != iEnd)
        {
            if ( (*iStart)->data(Item::io_selected).toList().value(m_taskid).toInt() != Qt::Unchecked )
                ++i;
            ++iStart;
        }
        if (i == item->parent()->childCount() )
            item->parent()->setTaskState(m_taskid,Qt::Checked);
        else
            item->parent()->setTaskState(m_taskid,Qt::PartiallyChecked);
    }
    else
    {
        changeEntry(item->ID().get(),false);
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
    }
//     qDebug() << "InstallTask::changeChildStates - done";
    return true;
}