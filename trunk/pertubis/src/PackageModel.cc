
/* Copyright (C) 2007-2008 Stefan Koegl
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
#include "make_package.hh"
#include <QColor>
#include <QBrush>
#include <QVariant>
#include <QDebug>
#include <QStringList>

using namespace pertubis;

PackageModel::PackageModel(QObject* pobj) :
    QAbstractItemModel(pobj),
    m_root(makeRootPackage())
{
}

PackageModel::~PackageModel()
{
    if (m_root != 0)
        delete m_root;
}

QVariant
PackageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical || section >= m_header.count())
        return QVariant();
    return *(m_header.begin() + section);
}

QVariant
PackageModel::data (const QModelIndex & ix, int role) const
{
    if (!ix.isValid())
        return QVariant();

    Package* item = static_cast<Package*>(ix.internalPointer());

    if (role == Qt::DisplayRole)
    {
        switch (ix.column())
        {
            case pho_install:
            case pho_deinstall:
            case pho_installed:
                return QVariant();
            default:
                return item->data(ix.column());
        }
    }

    else if (role == Qt::ForegroundRole)
    {
        switch (ix.column())
        {
            case pho_mask_reasons:
                return QBrush(QColor(255,0,0));
                break;
            default:
                return QBrush(QColor(0,0,0));
                break;
        }
    }

    else if (role == Qt::CheckStateRole)
    {
        switch (ix.column())
        {
            case pho_install:
            case pho_deinstall:
            case pho_installed:
                return item->data(ix.column());
            default:
                return QVariant();
        }
    }


    return QVariant();
}

QModelIndex
PackageModel::index(int row, int column, const QModelIndex &parentIndex) const
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

QModelIndex
PackageModel::parent(const QModelIndex &mix) const
{
    if (!mix.isValid())
        return QModelIndex();

    Package *childItem = static_cast<Package*>(mix.internalPointer());
    Q_ASSERT(childItem != 0);
    Package *parentItem = childItem->parent();
    if (parentItem == 0)
        return QModelIndex();
    if (parentItem == m_root)
        return QModelIndex();

    return QAbstractItemModel::createIndex(parentItem->row(), 0, parentItem);
}

int
PackageModel::rowCount(const QModelIndex &pmi) const
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

int
PackageModel::columnCount(const QModelIndex &pmi) const
{
    if (pmi.isValid())
        return static_cast<Package*>(pmi.internalPointer())->columnCount();
    else if (m_root != 0)
        return m_root->columnCount();
    else
        return 0;
}


bool
PackageModel::setData(const QModelIndex & ix, const QVariant & value, int)
{
    qDebug() << "setData()" << ix << value;
    if (! ix.isValid())
        return false;

    Package * package(static_cast<Package*>(ix.internalPointer()));
    package->setData(ix.column(), value);
    emit layoutChanged();
    return true;
}

void
PackageModel::clear()
{
    if (m_root != 0)
    {
        delete m_root;
        m_root = makeRootPackage();
        reset();
    }
}

void
PackageModel::appendPackage(Package* package)
{
//     qDebug() << "appendPackage()";
    Q_ASSERT(m_root != 0);
    Q_ASSERT(package != 0);
    m_root->appendChild(package);
    layoutChanged();
}

void
PackageModel::prependPackage(Package* package)
{
//     qDebug() << "prependPackage()";
    Q_ASSERT(m_root != 0);
    Q_ASSERT(package != 0);
    m_root->prependChild(package);
    layoutChanged();
}

void
PackageModel::setHorizontalHeaderLabels (const QStringList & labels)
{
    qDebug() << "PackageModel::setHorizontalHeaderLabels()" << labels;
    m_header = labels;
    emit headerDataChanged(Qt::Horizontal,0,labels.count() -1);
}

bool
PackageModel::setHeaderData (int section, Qt::Orientation orientation, const QVariant & value, int role)
{
    if (section >= 0 && orientation == Qt::Horizontal && section < m_header.count() && role == Qt::EditRole)
    {
        m_header.replace(section,value.toString());
        emit headerDataChanged(orientation,section,section);
        return true;
    }
    return false;
}
