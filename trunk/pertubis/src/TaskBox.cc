
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

#include "TaskBox.hh"
#include "Task.hh"
#include "Item.hh"
#include <paludis/package_id.hh>

#include <QDebug>

QVariantList pertubis::TaskBox::selectionData(const paludis::PackageID* id)
{
    QVariantList map;
    QVector<Task*>::const_iterator mytask = m_tasks.constBegin();
    while (mytask != m_tasks.constEnd())
    {
        map.push_back( ((*mytask)->hasEntry(id ) ) ? Qt::Checked : Qt::Unchecked);
        ++mytask;
    }
//     qDebug() << "TaskBox::selectionData()" << map;
    return map;
}

int pertubis::TaskBox::addTask(Task* t)
{
    m_tasks.push_back(t);
    int id(m_tasks.indexOf(t));
    t->setTaskid(id);
    return id;
}

pertubis::Task* pertubis::TaskBox::task(int taskid) const
{
    return m_tasks.value(taskid);
}

QVariantList pertubis::TaskBox::tasks() const
{
    QVariantList list;
    int x =m_tasks.count();
    for (int i=0;i<x;i++)
    {
        list.push_back(Qt::Unchecked);
    }
    return list;
}

bool pertubis::TaskBox::hasTask(int taskid) const
{
    if (m_tasks.count() > taskid && m_tasks.value(taskid) != 0 )
        return true;
    return false;
}

void pertubis::TaskBox::slotTaskChanged(const paludis::PackageID* id,int taskid, bool mystate)
{
//     qDebug() << "TaskBox::slotTaskChanged - start" << taskid << mystate;

    Task* t = task(taskid);
    if(t == 0)
        return;
    t->changeEntry(id,mystate);

//     qDebug() << "TaskBox::slotTaskChanged - done";
}
