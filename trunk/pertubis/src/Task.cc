
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

#include "Task.hh"
#include "PackageItem.hh"
#include <paludis/package_id.hh>


#include <QDebug>

pertubis::Task::Task(QObject* pobj,
                     QAction* myaction,
                     QString myname) :
                    QObject(pobj),
                    m_action(myaction),
                    m_name(myname),
                    m_taskid(-1)
{
}

void pertubis::Task::setTaskid(int id)
{
    m_taskid = id;
}

void pertubis::Task::addEntry(const paludis::PackageID* id)
{
    m_data.insert(id );
}

void pertubis::Task::deleteEntry(const paludis::PackageID* id)
{
    std::set<const paludis::PackageID* >::iterator iter = m_data.find(id);
    if (iter != m_data.end() )
        m_data.erase(iter);
}

bool pertubis::Task::hasEntry(const paludis::PackageID* id) const
{
    return (m_data.find(id ) != m_data.end());
}

void pertubis::Task::fillAction(Item* item)
{
    qDebug() << "Task::fillAction() - start" << m_action->text();
    m_action->setChecked( (hasEntry(item->ID().get())) ? Qt::Checked : Qt::Unchecked);
    qDebug() << "Task::fillAction() - done";
}

void pertubis::Task::changeEntry(const paludis::PackageID* id,bool mystate)
{
    qDebug() << "Task::changeEntry() - start" << mystate;

    if (mystate)
        addEntry(id);
    else
        deleteEntry(id);
    qDebug() << "Task::changeEntry() - done";
}
