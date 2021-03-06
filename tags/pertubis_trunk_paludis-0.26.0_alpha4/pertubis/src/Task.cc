
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

#include "Task.hh"
#include "Package.hh"
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

void pertubis::Task::addEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id)
{
//     qDebug() << "pertubis::Task::addEntry()" << id->canonical_form(paludis::idcf_full).c_str();
    if (!hasEntry(id))
        m_data.insert(id);
}

void pertubis::Task::deleteEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id)
{
//     qDebug() << "pertubis::Task::deleteEntry()" << id->canonical_form(paludis::idcf_full).c_str();
    paludis::PackageIDSet::ConstIterator iter = m_data.find(id);
    if (iter != m_data.end())
        m_data.erase(*iter);
}

bool pertubis::Task::hasEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id) const
{
//     qDebug() << "pertubis::Task::hasEntry()" << id->canonical_form(paludis::idcf_full).c_str();
    Iterator iter = m_data.find(id);
    if (iter != m_data.end())
        return true;
    return false;
}

void pertubis::Task::fillAction(Package* item)
{
//     qDebug() << "pertubis::Task::fillAction()" << *item;
    if (item->ID().get() != 0)
        m_action->setChecked( (hasEntry(item->ID())) ? Qt::Checked : Qt::Unchecked);
    else
        m_action->setChecked( Qt::Unchecked);
}

void pertubis::Task::changeEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id,bool mystate)
{
//     qDebug() << "pertubis::Task::changeEntry()" << id->canonical_form(paludis::idcf_full).c_str() << mystate;
    (mystate) ? addEntry(id) : deleteEntry(id);
}

void pertubis::Task::clear()
{
    for (paludis::PackageIDSet::ConstIterator i(m_data.begin()), i_end(m_data.end());
         i != i_end ; ++i)
    {
        m_data.erase(*i);
    }
    Q_ASSERT(m_data.empty());
}
