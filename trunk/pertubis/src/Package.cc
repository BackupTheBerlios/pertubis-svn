

/* Copyright (C) 2007-2008 Stefan Koegl <hotshelf@users.berlios.de>
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

#include "Package.hh"
#include <QDebug>
#include <paludis/package_id.hh>

pertubis::Package::Package(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                    const QVector<QVariant> & dats,
                    PackageState mystate,
                    PackageType itype,
                    Package* pitem)  :
                    m_data(dats),
                    m_id(id),
                    m_parent(pitem),
                    m_bestChild(0),
                    m_state(mystate),
                    m_itemType(itype)
{
}

pertubis::Package::~Package()
{
    m_parent=0;
    qDeleteAll(m_children);
    m_children.clear();
}

pertubis::Package* pertubis::Package::child(int rindex) const
{
    return m_children.value(rindex,0);
}

int pertubis::Package::childCount() const
{
    return m_children.count();
}

int pertubis::Package::columnCount() const
{
    return m_data.count();
}

QVariant pertubis::Package::data(int column) const
{
    return m_data.value(column);
}

pertubis::Package* pertubis::Package::parent() const
{
    return m_parent;
}

bool pertubis::Package::available() const
{
    return (m_state != ps_masked);
}

pertubis::PackageState pertubis::Package::state() const
{
    return m_state;
}

void pertubis::Package::appendChild(Package *item)
{
    m_children.append(item);
    item->setParent(this);
}

void pertubis::Package::prependChild(Package *item)
{
    m_children.prepend(item);
    item->setParent(this);
}

void pertubis::Package::setBestChild(Package *item)
{
    m_bestChild = item;
}

void pertubis::Package::setData(int column, QVariant dats)
{
    if (column < m_data.count() )
    {
        m_data.replace(column, dats);
    }
}

void pertubis::Package::setParent(Package* pitem)
{
    m_parent=pitem;
}

void pertubis::Package::setPackageState(PackageState s)
{
     m_state = s;
}

paludis::tr1::shared_ptr<const paludis::PackageID> pertubis::Package::ID() const
{
    if (m_bestChild != 0)
        return m_bestChild->ID();
    return m_id;
}


bool pertubis::Package::bestChildTest()
{
    return bestChild() != 0;
}



#ifndef QT_NO_DEBUG
QDebug operator<<(QDebug dbg, const pertubis::Package &item)
{
    dbg.space() << "\n Package(" <<
            "install" << item.data(pertubis::pho_install).toInt() << "\n" <<
            "deinstall" << item.data(pertubis::pho_deinstall).toInt() << "\n" <<
            "package" << item.data(pertubis::pho_package).toString() << "\n" <<
            "cat" << item.data(pertubis::pho_category).toString() << "\n" <<
            "repository" << item.data(pertubis::pho_repository).toString() << "\n" <<
            "installed" << item.data(pertubis::pho_installed).toString() << "\n" <<
            "mask_reasons" << item.data(pertubis::pho_mask_reasons).toString() << "\n" <<
            "state" << item.state() << "\n" <<
            "itemType" << item.itemType() << ")";
    return dbg.space();
}
#endif
