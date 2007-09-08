
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

#include "PackageModel.hh"
#include "PackageItem.hh"

#include <QDebug>
#include <QBrush>
#include <QApplication>
#include <QColor>

#include "Task.hh"
#include "TaskBox.hh"
#include <QStringList>

bool ItemLessThan(const pertubis::Item* a,const pertubis::Item* b)
{
    return (a->data(0).toString() < b->data(0).toString() );
}

pertubis::PackageModel::PackageModel(QObject* pobj) : QAbstractItemModel(pobj),m_root(0)
{
    m_root = new RootItem();
}

pertubis::PackageModel::~PackageModel()
{
}

void pertubis::PackageModel::setBox(TaskBox* t)
{
    m_box = t;
}

bool pertubis::PackageModel::setSelectionData( const QModelIndex & ix, int taskid, bool mystate)
{
    qDebug() << "PackageModel::setSelectionData() - start" << ix << taskid << mystate;
    Item* item = static_cast<Item*>(ix.internalPointer());
    Item::UpdateRange range = item->updateRange();
    switch (range)
    {
        case Item::ur_parent:
               if (m_box->task(taskid)->changeParentStates(item, mystate) )
                emit dataChanged(ix, index(item->childCount()-1,0,ix));
            return true;
        case Item::ur_child:
            if (m_box->task(taskid)->changeChildStates(item,mystate) )
            {
                QModelIndex p = parent(ix);
                QModelIndex last = index(rowCount(p)-1,0,p);
                emit dataChanged(p,last);
            }
            return true;
        default:
            return true;
    }
    return false;
}

bool pertubis::PackageModel::setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role)
{
    if (orientation == Qt::Horizontal && section < m_header.count() && role == Qt::EditRole)
    {
        m_header.replace(section,value.toString());
        emit headerDataChanged(orientation,section,section);
        return true;
    }
    return false;
}

Qt::ItemFlags pertubis::PackageModel::flags(const QModelIndex &mix) const
{
    switch (mix.column())
    {
        case Item::io_selected:
            return Qt::ItemIsEditable;
            break;
        case Item::io_installed:
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

    Item* item = static_cast<Item*>(ix.internalPointer());

    if (role == Qt::ForegroundRole)
    {
        if (ix.column() == Item::io_installed)
            return QBrush(QColor(0,0,255));
        if (ix.column() == Item::io_selected)
            return QBrush(QColor(0,255,0));
        if (ix.column() == Item::io_package)
        {
            if (item->state() == Item::is_stable)
                return QBrush(QColor(0,255,0));
            if (item->state() == Item::is_unstable)
                return QBrush(QColor(255,200,0));
            if (item->state() == Item::is_masked)
                return QBrush(QColor(255,0,0));
        }
    }

    if (role == Qt::CheckStateRole )
    {
        switch (ix.column())
        {
            case Item::io_installed:
                return item->data(ix.column());
            default:
                return QVariant();
        }
    }

    if (role == Qt::DisplayRole )
    {
        switch (ix.column())
        {
            case Item::io_installed:
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

    Item *pmiItem;

    if (!parentIndex.isValid())
        pmiItem = m_root;
    else
        pmiItem = static_cast<Item*>(parentIndex.internalPointer());

    Item *childItem = pmiItem->child(row);
    if (childItem)
        return QAbstractItemModel::createIndex(row, column, childItem);
    else
        return QModelIndex();
}

void pertubis::PackageModel::setHorizontalHeaderLabels ( const QStringList & labels )
{
    m_header = labels;
}

QModelIndex pertubis::PackageModel::parent(const QModelIndex &mix) const
{
    if (!mix.isValid())
        return QModelIndex();

    Item *childItem = static_cast<Item*>(mix.internalPointer());
    Item *parentItem = childItem->parent();

    if (parentItem == m_root)
        return QModelIndex();

    return QAbstractItemModel::createIndex(parentItem->row(), 0, parentItem);
}

int pertubis::PackageModel::rowCount(const QModelIndex &pmi) const
{
    Item *parentItem;
    if (pmi.column() > 0)
        return 0;

    if (!pmi.isValid())
        parentItem = m_root;
    else
        parentItem = static_cast<Item*>(pmi.internalPointer());

    return parentItem->childCount();
}

int pertubis::PackageModel::columnCount(const QModelIndex &pmi) const
{
    if (pmi.isValid())
         return static_cast<Item*>(pmi.internalPointer())->columnCount();
    else if (m_root != 0)
        return m_root->columnCount();
    else
        return 0;
}

void pertubis::PackageModel::slotSetRoot(Item* item)
{
    if (m_root != 0)
        delete m_root;
    m_root = item;
    reset();
}

void pertubis::PackageModel::slotAppendPackage(Item* item)
{
    if (m_root != 0)
    {
        m_root->appendChild(item);
        item->setParent(m_root);
        reset();
    }
}
