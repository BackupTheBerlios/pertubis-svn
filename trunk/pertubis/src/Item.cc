
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
*
* This file is part of the pertubis frontend for paludis package manager.
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

#include "Item.hh"
#include <QDebug>
#include <paludis/package_id.hh>

pertubis::Item::Item()  :
    m_data(QList<QVariant>() << QVariant(QVariantList()) << "" << "" << "" << Qt::Unchecked << "" << ""),
    m_parent(0),
    m_bestChild(0),
    m_state(Item::is_stable),
    m_ur(Item::ur_nothing)
{
}

pertubis::Item::Item(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                    const QList<QVariant> &dats,
                    ItemState mystate,
                    UpdateRange ur,
                    Item* pitem)  :
                    m_data(dats),
                    m_id(id),
                    m_parent(pitem),
                    m_bestChild(0),
                    m_state(mystate),
                    m_ur(ur)
{
}

pertubis::Item::Item(const QList<QVariant> &dats,
                    ItemState mystate,
                    UpdateRange ur,
                    Item* pitem)  :
                    m_data(dats),
                    m_parent(pitem),
                    m_bestChild(0),
                    m_state(mystate),
                    m_ur(ur)
{
}

pertubis::Item::~Item()
{
    m_parent=0;
    qDeleteAll(m_children);
    m_children.clear();
}

pertubis::Item* pertubis::Item::child(int rindex) const
{
    return m_children.value(rindex);
}

int pertubis::Item::childCount() const
{
    return m_children.count();
}

int pertubis::Item::columnCount() const
{
    return m_data.count();
}

QVariant pertubis::Item::data(int column) const
{
    return m_data.value(column);
}

pertubis::Item* pertubis::Item::parent() const
{
    return m_parent;
}

int pertubis::Item::indexOf(Item* item) const
{
    return m_children.indexOf(item);
}

bool pertubis::Item::available() const
{
    return (m_state != Item::is_masked);
}

pertubis::Item::ItemState pertubis::Item::state() const
{
    return m_state;
}

void pertubis::Item::appendChild(Item *item)
{
    m_children.append(item);
    item->setParent(this);
}

void pertubis::Item::prependChild(Item *item)
{
    m_children.prepend(item);
    item->setParent(this);
}

int pertubis::Item::row() const
{
    if (m_parent)
        return m_parent->indexOf( const_cast< Item* >(this) );
    return 0;
}

void pertubis::Item::setBestChild(Item *item)
{
    m_bestChild = item;
}

void pertubis::Item::setData(int column, QVariant dats)
{
    if (column < m_data.count() )
    {
        m_data.replace(column, dats);
    }
}

pertubis::Item::ConstIterator pertubis::Item::constChildBegin()
{
    return m_children.begin();
}

pertubis::Item::Iterator pertubis::Item::childBegin()
{
    return m_children.begin();
}

pertubis::Item::ConstIterator pertubis::Item::constChildEnd()
{
    return m_children.end();
}

pertubis::Item::Iterator pertubis::Item::childEnd()
{
    return m_children.end();
}

void pertubis::Item::setParent(Item* pitem)
{
    m_parent=pitem;
}

void pertubis::Item::setState(ItemState s)
{
     m_state = s;
}

void pertubis::Item::setTaskState(int taskid, Qt::CheckState mystate)
{
    QVariantList states = data(io_selected).toList();
//     qDebug() << "Item::setTaskState() - start" << states << taskid << mystate;
    states[taskid] = mystate;
    setData(io_selected,states);
//     qDebug() << "Item::setTaskState() - done";
}

paludis::tr1::shared_ptr<const paludis::PackageID> pertubis::Item::ID()
{
    if (m_bestChild != 0)
        return m_bestChild->ID();
    return m_id;
}

pertubis::Item::UpdateRange pertubis::Item::updateRange() const
{
    return m_ur;
}

pertubis::Item* pertubis::Item::bestChild() const
{
    return m_bestChild;
}

QString pertubis::stateDescription(Item::ItemState status)
{
    switch (status)
    {
        case Item::is_stable:
            return "stable";
        case Item::is_unstable:
            return "possibly unstable";
        case Item::is_masked:
            return "not avaliable since masked";
        default:
            return "unknown package status";
    }
}

pertubis::Item* pertubis::makePackageItem(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                             QVariantList selections,
                             QString pack,
                             QString cat,
                             Qt::CheckState isInstalled,
                             Item::ItemState mystate,
                             Item::UpdateRange ur,
                             Item* pitem,
                             QString mask_reasons)
{
    QVariantList list;
    list <<
            QVariant(selections) <<  // io_selected
            pack << // io_package
            cat <<  // io_category
            "" << // io_repository
            QVariant(static_cast<int>(isInstalled)) <<  // io_installed
            mask_reasons << // io_mask_reasons
            "";  // io_change
    return new Item(id,list,mystate,ur,pitem);
}

pertubis::Item* pertubis::makeVersionItem(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                                 QVariantList selections,
                                 QString version,
                                 QString rep,
                                 Qt::CheckState isInstalled,
                                 Item::ItemState mystate,
                                 Item::UpdateRange ur,
                                 Item* pitem,
                                 QString mask_reasons)
{
    QVariantList list;
    list <<
            QVariant(selections) << // io_selected
            version << // io_package
            "" << // io_category
            rep << // io_repository
            QVariant(static_cast<int>(isInstalled)) <<  // io_installed
            mask_reasons <<  // io_mask_reasons
            ""; // io_change
    return new Item(id,list,mystate,ur,pitem);
}

#ifndef QT_NO_DEBUG
QDebug operator<<(QDebug dbg, const pertubis::Item &item)
{
    dbg.nospace() <<
            item.data(pertubis::Item::io_selected).toList() << ", " <<
            item.data(pertubis::Item::io_package).toString() << ", " <<
            item.data(pertubis::Item::io_category).toString() << ", " <<
            item.data(pertubis::Item::io_repository).toString() << ", " <<
            item.data(pertubis::Item::io_installed).toString() << ", " <<
            item.data(pertubis::Item::io_change).toString() << ", " <<
            item.childCount() << ", " <<
            item.columnCount() << ")";
    return dbg.space();
}
#endif
