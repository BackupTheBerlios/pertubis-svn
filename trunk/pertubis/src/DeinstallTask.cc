
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

#include "DeinstallTask.hh"
#include "Item.hh"
#include <QAction>

bool pertubis::DeinstallTask::changeParentStates(pertubis::Item* /*item*/, int /*mystate*/)
{
//     QList<Item*>::iterator iStart(item->childBegin());
//     QList<Item*>::iterator iEnd(item->childEnd());
//     bool inUse=false;
//     switch (mystate)
//     {
//         case Qt::Unchecked:
//             item->setTaskState(m_taskid,mystate);
//             while(iStart != iEnd)
//             {
//                 (*iStart)->setTaskState(m_taskid,Qt::Unchecked);
//             }
//             break;
//         case Qt::PartiallyChecked:
//         case Qt::Checked:
//             while(iStart != iEnd)
//             {
//                 if ( (*iStart)->data(Item::io_installed).toBool() )
//                 {
//                     (*iStart)->setTaskState(m_taskid,Qt::Checked);
//                     inUse=true;
//                     ++iStart;
//                 }
//             }
//             if (inUse)
//             {
//                 item->setTaskState(m_taskid,Qt::Checked);
//                 return true;
//             }
//             break;
//         default:
//             ;
//     }
    return false;
}

bool pertubis::DeinstallTask::available(Item* item) const
{
    return item->data(Item::io_installed).toBool();
}

bool pertubis::DeinstallTask::changeChildStates(pertubis::Item* /*item*/, int /*newState*/)
{
//     bool inUse=false;
//     QtCheckState oldState = (*iStart)->data(Item::io_selected).toList().value(m_taskid).toInt();
//     if (newState)
//     {
//         if (oldState == Qt::Unchecked)
//         {
//             item->setTaskState(m_taskid,mystate);
//             QList<Item*>::iterator iStart(item->childBegin());
//             QList<Item*>::iterator iEnd(item->childEnd());
//             while (iStart != iEnd)
//             {
//                 if ( (*iStart)->data(Item::io_selected).toList().value(m_taskid).toInt() != Qt::Unchecked )
//                 {
//                     inUse=true;
//                     break;
//                 }
//                 ++iStart;
//             }
//             if (!inUse)
//             {
//                 item->parent()->setTaskState(m_taskid,Qt::Unchecked);
//             }
//         }
//         else if (oldState == Qt::Checked)
//         {
//             if (item->data(Item::io_installed).toBool())
//             {
//                 item->setTaskState(m_taskid,Qt::Checked);
//                 item->parent()->setTaskState(m_taskid,Qt::PartiallyChecked);
//                 return true;
//             }
//         }
//     }
//     else
//     {
//
//     }
    return true;
}

void pertubis::DeinstallTask::startTask(DatabaseView* /*main*/)
{
//     paludis::DepListOptions options;
//     pertubis::Install task(m_env,options,m_env->default_destinations());
//     for (paludis::PackageIDSet::ConstIterator i(m_data.begin()), i_end(m_data.end());
//          i != i_end ; ++i)
//     {
//         task.add_target(i->get()->canonical_form(paludis::idcf_full);
//     }
//     task.execute();
}

