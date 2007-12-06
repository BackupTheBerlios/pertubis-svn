

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

#include "Package.hh"
#include <QDebug>
#include <paludis/package_id.hh>

pertubis::Package::Package()  :
    m_data(QList<QVariant>() << QVariant(QVariantList()) << "" << "" << "" << Qt::Unchecked << "" << ""),
    m_id(paludis::tr1::shared_ptr<const paludis::PackageID>()),
    m_parent(0),
    m_bestChild(0),
    m_state(Package::ps_stable),
    m_itemType(Package::pt_nothing)
{
}

pertubis::Package::Package(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                    const QList<QVariant> &dats,
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

// pertubis::Package::Package(const QList<QVariant> &dats,
//                     PackageState mystate,
//                     UpdateRange ur,
//                     Package* pitem)  :
//                     m_data(dats),
//                     m_parent(pitem),
//                     m_bestChild(0),
//                     m_state(mystate),
//                     m_ur(ur)
// {
// }

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
    return (m_state != Package::ps_masked);
}

pertubis::Package::PackageState pertubis::Package::state() const
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

void pertubis::Package::setTaskState(int taskid, Qt::CheckState mystate)
{
    QVariantList states = data(po_selected).toList();
//     qDebug() << "Package::setTaskState() - start" << states << taskid << mystate;
    states[taskid] = mystate;
    setData(po_selected,states);
//     qDebug() << "Package::setTaskState() - done";
}

paludis::tr1::shared_ptr<const paludis::PackageID> pertubis::Package::ID() const
{
    if (m_bestChild != 0)
        return m_bestChild->ID();
    return m_id;
}



// QString pertubis::stateDescription(Package::PackageState status)
// {
//     switch (status)
//     {
//         case Package::ps_stable:
//             return "stable";
//         case Package::ps_unstable:
//             return "possibly unstable";
//         case Package::ps_masked:
//             return "not avaliable since masked";
//         default:
//             return "unknown package status";
//     }
// }

pertubis::Package* pertubis::makePackagePackage(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                             QVariantList selections,
                             QString pack,
                             QString cat,
                             Qt::CheckState isInstalled,
                             Package::PackageState mystate,
                             Package* pitem,
                             QString mask_reasons)
{
    QVariantList list;
    list <<
            QVariant(selections) <<  // po_selected
            pack << // po_package
            cat <<  // po_category
            "" << // po_repository
            QVariant(static_cast<int>(isInstalled)) <<  // po_installed
            mask_reasons << // po_mask_reasons
            "";  // po_change
    return new Package(id,list,mystate, Package::pt_parent, pitem);
}

pertubis::Package* pertubis::makeVersionPackage(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                                 QVariantList selections,
                                 QString version,
                                 QString rep,
                                 Qt::CheckState isInstalled,
                                 Package::PackageState mystate,
                                 Package* pitem,
                                 QString mask_reasons)
{
    QVariantList list;
    list <<
            QVariant(selections) << // po_selected
            version << // po_package
            "" << // po_category
            rep << // po_repository
            QVariant(static_cast<int>(isInstalled)) <<  // po_installed
            mask_reasons <<  // po_mask_reasons
            ""; // po_change
    return new Package(id,list,mystate,Package::pt_child,pitem);
}

#ifndef QT_NO_DEBUG
QDebug operator<<(QDebug dbg, const pertubis::Package &item)
{
    dbg.space() << "\n Package(" <<
            "selected" << item.data(pertubis::Package::po_selected).toList() << "\n" <<
            "package" << item.data(pertubis::Package::po_package).toString() << "\n" <<
            "cat" << item.data(pertubis::Package::po_category).toString() << "\n" <<
            "repository" << item.data(pertubis::Package::po_repository).toString() << "\n" <<
            "installed" << item.data(pertubis::Package::po_installed).toString() << "\n" <<
            "mask_reasons" << item.data(pertubis::Package::po_mask_reasons).toString() << "\n" <<
            "change" << item.data(pertubis::Package::po_change).toString() << "\n" <<
            "state" << item.state() << "\n" <<
            "itemType" << item.itemType() << ")";
    return dbg.space();
}
#endif
