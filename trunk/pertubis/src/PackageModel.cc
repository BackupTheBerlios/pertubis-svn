
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
#include "TaskBox.hh"
#include "Task.hh"
#include "Package.hh"
#include "SystemReport-fwd.hh"

#include <QColor>
#include <QBrush>
#include <QVariant>
#include <QDebug>
#include <QStringList>

pertubis::PackageModel::PackageModel(QObject* pobj) : QAbstractItemModel(pobj),m_root(new Package()),m_box(0),m_reportMode(false)
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
        case Package::po_selected:
            return Qt::ItemIsEditable;
            break;
        case Package::po_installed:
            return Qt::ItemIsSelectable;
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
    if (!m_reportMode)
    {

        if (role == Qt::ForegroundRole)
        {
            switch (ix.column())
            {
                case Package::po_installed:
                    return QBrush(QColor(0,0,255));
                    break;
                case Package::po_mask_reasons:
                    return QBrush(QColor(255,0,0));
                    break;
                case Package::po_change:
                    return QBrush(QColor(0,255,0));
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
                case Package::po_installed:
                    return item->data(ix.column());
                default:
                    return QVariant();
            }
        }

        if (role == Qt::DisplayRole )
        {
            switch (ix.column())
            {
                case Package::po_installed:
                    return QVariant();
                default:
                    return item->data(ix.column());
            }
        }
    }
    else
    {
        if (role == Qt::ForegroundRole)
        {
            if (ix.column() == rho_reasons)
                    return QBrush(QColor(255,0,0));
            return QBrush(QColor(0,0,0));
        }
        if (role == Qt::DisplayRole )
        {
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

void pertubis::PackageModel::clear(int columns)
{
    if (m_root != 0)
    {
        delete m_root;
        m_root = makeRootPackage(columns);
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


bool pertubis::PackageModel::setSelectionData( const QModelIndex & ix, int taskid, bool mystate)
{
    Package* item = static_cast<Package*>(ix.internalPointer());
    if (item != 0 && m_box->task(taskid)->changeStates(item,mystate))
    {
        emit layoutChanged();
        return true;
    }
    return false;
}

void pertubis::PackageModel::setBox(TaskBox* t)
{
    m_box = t;
}

void pertubis::PackageModel::setHorizontalHeaderLabels ( const QStringList & labels )
{
    qDebug() << "pertubis::PackageModel::setHorizontalHeaderLabels()" << labels;
    m_header = labels;
    qDebug() << "pertubis::PackageModel::setHorizontalHeaderLabels() 1";
    emit headerDataChanged(Qt::Horizontal,0,labels.count() -1);
    qDebug() << "pertubis::PackageModel::setHorizontalHeaderLabels() 2";
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
