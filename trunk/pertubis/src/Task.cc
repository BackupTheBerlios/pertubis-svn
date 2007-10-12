
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
#include "Item.hh"
#include <paludis/package_id.hh>
#include <paludis/util/set.hh>
#include <paludis/util/set-impl.hh>

#include <libwrapiter/libwrapiter_forward_iterator.hh>
#include <libwrapiter/libwrapiter_output_iterator.hh>
#include <QDebug>
#include <QAction>

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

void pertubis::Task::addEntry(paludis::tr1::shared_ptr<const paludis::PackageID> id)
{
    if (!hasEntry(id))
        m_data.insert(id);
}

void pertubis::Task::deleteEntry(paludis::tr1::shared_ptr<const paludis::PackageID> id)
{
    paludis::PackageIDSet::ConstIterator iter = m_data.find(id);
    if (iter != m_data.end())
        m_data.erase(*iter);
}

bool pertubis::Task::hasEntry(paludis::tr1::shared_ptr<const paludis::PackageID> id) const
{
    Iterator iter = m_data.find(id);
    if (iter != m_data.end())
        return true;
    return false;
}

void pertubis::Task::fillAction(Item* item)
{
    if (item->ID().get() != 0)
        m_action->setChecked( (hasEntry(item->ID())) ? Qt::Checked : Qt::Unchecked);
    else
        m_action->setChecked( Qt::Unchecked);
}

void pertubis::Task::changeEntry(paludis::tr1::shared_ptr<const paludis::PackageID> id,bool mystate)
{
    (mystate) ? addEntry(id) : deleteEntry(id);
}
