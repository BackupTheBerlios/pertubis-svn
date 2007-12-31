
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

#include "Selections.hh"
#include "Package.hh"
#include <paludis/package_id.hh>
#include <paludis/util/set.hh>
#include <paludis/util/set-impl.hh>

#include <QDebug>
#include <QAction>

pertubis::Selections::Selections(QObject* pobj,
                     QAction* myaction,
                     QString myname,
                    PackageOrder pos) :
                    QObject(pobj),
                    m_name(myname),
                    m_action(myaction),
                    m_position(pos)
{
}

void pertubis::Selections::addEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id)
{
//     qDebug() << "pertubis::Selections::addEntry()" << id->canonical_form(paludis::idcf_full).c_str();
    m_data.insert(id);
}

void pertubis::Selections::deleteEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id)
{
//     qDebug() << "pertubis::Selections::deleteEntry()" << id->canonical_form(paludis::idcf_full).c_str();
    m_data.erase(id);
}

Qt::CheckState pertubis::Selections::hasEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id) const
{
//     qDebug() << "pertubis::Selections::hasEntry()" << id->canonical_form(paludis::idcf_full).c_str();
    return m_data.find(id) != m_data.end() ? Qt::Checked : Qt::Unchecked;
}

void pertubis::Selections::changeEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id,bool mystate)
{
//     qDebug() << "pertubis::Selections::changeEntry()" << id->canonical_form(paludis::idcf_full).c_str() << mystate;
    (mystate) ? addEntry(id) : deleteEntry(id);
}

void pertubis::Selections::prepareAction(const paludis::tr1::shared_ptr<const paludis::PackageID>& id)
{
//     qDebug() << "pertubis::Selections::prepareAction()" << paludis::stringify(id).c_str();
    m_action->setChecked( hasEntry(id) ? Qt::Checked : Qt::Unchecked);
}

void pertubis::Selections::clear()
{
    for (paludis::PackageIDSet::ConstIterator i(m_data.begin()), i_end(m_data.end());
         i != i_end ; ++i)
    {
        m_data.erase(*i);
    }
    Q_ASSERT(m_data.empty());
}