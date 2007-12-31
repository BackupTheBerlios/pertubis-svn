
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

#include "PackageModel.hh"
#include "Package.hh"
#include "SystemReport-fwd.hh"

#include <QColor>
#include <QBrush>
#include <QVariant>
#include <QDebug>
#include <QStringList>

pertubis::PackageModel::PackageModel(QObject* pobj) :
                                    QAbstractItemModel(pobj),
                                    m_root(makeRootPackage())
{
}

pertubis::PackageModel::~PackageModel()
{
    if (m_root)
        delete m_root;
}

Qt::ItemFlags pertubis::PackageModel::flags(const QModelIndex &mix) const
{
    if (!mix.isValid() )
        return 0;
    switch (mix.column())
    {
        case po_install:
        case po_deinstall:
            return Qt::ItemIsSelectable | Qt::ItemIsEditable |  Qt::ItemIsUserCheckable;
            break;
        case po_installed:
            return Qt::ItemIsSelectable | Qt::ItemIsEditable |  Qt::ItemIsUserCheckable;
            break;
        default:
            return 0;
    }
}

QVariant pertubis::PackageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical || section >= m_header.count() )
        return QVariant();
    return *(m_header.begin() + section);
}

QVariant pertubis::PackageModel::data ( const QModelIndex & ix, int role) const
{
    if (!ix.isValid())
        return QVariant();

    Package* item = static_cast<Package*>(ix.internalPointer());

    if (role == Qt::BackgroundRole)
    {
        if (0  == ix.row() % 2 )
            return QBrush(QColor(225,225,225));
        else
            return QBrush(QColor(255,255,255));
    }

    if (role == Qt::ForegroundRole)
    {
        switch (ix.column())
        {
            case po_install:
                return QBrush(QColor(0,255,0));
                break;
            case po_deinstall:
                return QBrush(QColor(255,0,0));
                break;
            case po_installed:
                return QBrush(QColor(0,0,255));
                break;
            case po_mask_reasons:
                return QBrush(QColor(255,0,0));
                break;
            default:
                return QBrush(QColor(0,0,0));
                break;
        }
    }

    if (role == Qt::CheckStateRole )
    {
        switch (ix.column())
        {
            case po_install:
            case po_deinstall:
            case po_installed:
                return item->data(ix.column());
            default:
                return QVariant();
        }
    }

    if (role == Qt::DisplayRole )
    {
        switch (ix.column())
        {
            case po_install:
            case po_deinstall:
            case po_installed:
                return QVariant();
            default:
                return item->data(ix.column());
        }
    }

    return QVariant();
}

QModelIndex pertubis::PackageModel::index(int row, int column, const QModelIndex &parentIndex) const
{
    if (!hasIndex(row, column, parentIndex))
        return QModelIndex();

    Package *parentItem;

    if (!parentIndex.isValid())
        parentItem = m_root;
    else
        parentItem = static_cast<Package*>(parentIndex.internalPointer());

    Package *childItem = parentItem->child(row);
    if (childItem)
        return QAbstractItemModel::createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex pertubis::PackageModel::parent(const QModelIndex &mix) const
{
    if (!mix.isValid())
        return QModelIndex();

    Package *childItem = static_cast<Package*>(mix.internalPointer());
    Package *parentItem = childItem->parent();

    if (parentItem == m_root)
        return QModelIndex();

    return QAbstractItemModel::createIndex(parentItem->row(), 0, parentItem);
}

int pertubis::PackageModel::rowCount(const QModelIndex &pmi) const
{
    Package *parentItem;
    if (pmi.column() > 0)
        return 0;

    if (!pmi.isValid())
        parentItem = m_root;
    else
        parentItem = static_cast<Package*>(pmi.internalPointer());

    return parentItem->childCount();
}

int pertubis::PackageModel::columnCount(const QModelIndex &pmi) const
{
    if (pmi.isValid())
        return static_cast<Package*>(pmi.internalPointer())->columnCount();
    else if (m_root != 0)
        return m_root->columnCount();
    else
        return 0;
}

void pertubis::PackageModel::clear()
{
    if (m_root != 0)
    {
        delete m_root;
        m_root = makeRootPackage();
        reset();
    }
}

void pertubis::PackageModel::appendPackage(Package* item)
{
    if (m_root != 0)
    {
        m_root->appendChild(item);
        layoutChanged();
    }
}

void pertubis::PackageModel::prependPackage(Package* item)
{
    if (m_root != 0)
    {
        m_root->prependChild(item);
        layoutChanged();
    }
}

void pertubis::PackageModel::setHorizontalHeaderLabels ( const QStringList & labels )
{
    qDebug() << "pertubis::PackageModel::setHorizontalHeaderLabels()" << labels;
    m_header = labels;
    emit headerDataChanged(Qt::Horizontal,0,labels.count() -1);
}

bool pertubis::PackageModel::setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role)
{
    if (section >= 0 && orientation == Qt::Horizontal && section < m_header.count() && role == Qt::EditRole)
    {
        m_header.replace(section,value.toString());
        emit headerDataChanged(orientation,section,section);
        return true;
    }
    return false;
}

pertubis::Package* pertubis::makePackage(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                                        Qt::CheckState install,
                                        Qt::CheckState deinstall,
                                        QString package,
                                        QString cat,
                                        Qt::CheckState isInstalled,
                                        PackageState mystate,
                                        Package* pitem,
                                        QString mask_reasons)
{
    Package* pack(new Package(id,QVector<QVariant>(po_last),mystate,pt_child,pitem));
    pack->setData(po_install,install);
    pack->setData(po_deinstall,deinstall);
    pack->setData(po_package,package);
    pack->setData(po_category,cat);
    pack->setData(po_repository, "");
    pack->setData(po_installed,QVariant(static_cast<int>(isInstalled)));
    pack->setData(po_mask_reasons,mask_reasons);
    return pack;
}

pertubis::Package* pertubis::makeRootPackage()
{
    return new Package(paludis::tr1::shared_ptr<const paludis::PackageID>(),
                       QVector<QVariant>(po_last),
                       ps_stable,
                       pt_parent,
                       0);
}

pertubis::Package* pertubis::makeVersionPackage(paludis::tr1::shared_ptr<const paludis::PackageID> id,
        Qt::CheckState install,
        Qt::CheckState deinstall,
        QString version,
        QString rep,
        Qt::CheckState isInstalled,
        PackageState mystate,
        Package* pitem,
        QString mask_reasons)
{
    Package* pack(new Package(id,QVector<QVariant>(po_last),mystate,pt_child,pitem));
    pack->setData(po_install,install);
    pack->setData(po_deinstall,deinstall);
    pack->setData(po_package,version);
    pack->setData(po_category,"");
    pack->setData(po_repository, rep);
    pack->setData(po_installed,QVariant(static_cast<int>(isInstalled)));
    pack->setData(po_mask_reasons,mask_reasons);
    return pack;
}


