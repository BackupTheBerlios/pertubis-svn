
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

#include "TaskBox.hh"
#include "Task.hh"
#include "Package.hh"
#include <paludis/package_id.hh>
#include <QDebug>

QVariantList pertubis::TaskBox::selectionData(paludis::tr1::shared_ptr<const paludis::PackageID> id) const
{
    QVariantList list;
    for (ConstIterator mytask(constTaskBegin()), taskend(constTaskEnd());
         mytask != taskend;
         ++mytask)
    {
        list.push_back( ((*mytask)->hasEntry(id ) ) ? Qt::Checked : Qt::Unchecked);
    }
    return list;
}

int pertubis::TaskBox::addTask(Task* t)
{
    m_tasks.push_back(t);
    int id(m_tasks.indexOf(t));
    m_index.insert(t->name(),t);
    t->setTaskid(id);
    return id;
}

pertubis::Task* pertubis::TaskBox::task(QString name) const
{
//     qDebug() << "pertubis::TaskBox::task()" << taskid;
    if (!m_index.contains(name))
        return 0;
    return m_index.value(name);
}

pertubis::Task* pertubis::TaskBox::task(int taskid) const
{
//     qDebug() << "pertubis::TaskBox::task()" << taskid;
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
//     qDebug() << "pertubis::TaskBox::tasks()" << list;
    return list;
}

bool pertubis::TaskBox::hasTask(int taskid) const
{
//     qDebug() << "pertubis::TaskBox::hasTask()" << taskid;
    if (m_tasks.count() > taskid && m_tasks.value(taskid) != 0 )
        return true;
    return false;
}

void pertubis::TaskBox::slotTaskChanged(paludis::tr1::shared_ptr<const paludis::PackageID> id,int taskid, bool mystate)
{
//     qDebug() << "pertubis::TaskBox::slotTaskChanged()" << taskid << mystate;
    Task* t = task(taskid);
    if(t == 0)
        return;
    if (mystate)
        t->addEntry(id);
    else
        t->deleteEntry(id);
}

void pertubis::TaskBox::setTasksInPackage(Package* item)
{
//     qDebug() << "pertubis::TaskBox::setTasksInItem()" << *item;
    for (Iterator mytask(taskBegin()), taskend(taskEnd());
        mytask != taskend;
        ++mytask)
    {
        Qt::CheckState mystate( ((*mytask)->hasEntry(item->ID() ) ) ? Qt::Checked : Qt::Unchecked);
        (*mytask)->changeStates(item, mystate);
    }
//     qDebug() << "pertubis::TaskBoxTest::setTasksInItem() - done";
}

void pertubis::TaskBox::startAllTasks(const paludis::tr1::shared_ptr<paludis::Environment>& env,Settings* settings, MessageOutput* output)
{
    for (Iterator i(m_tasks.begin()),i_end(m_tasks.end());i!= i_end;++i)
    {
        (*i)->startTask(env,settings, output);
    }
}

void pertubis::TaskBox::slotClear()
{
    qDebug() << "pertubis::TaskBox::slotClear()";
    for (Iterator i(m_tasks.begin()),i_end(m_tasks.end());i!= i_end;++i)
    {
        (*i)->clear();
    }
}
