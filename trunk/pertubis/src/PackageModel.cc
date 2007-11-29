
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
#include "TaskBox.hh"
#include "Task.hh"
#include "Item.hh"

#include <QColor>
#include <QBrush>
#include <QVariant>
#include <QDebug>
#include <QStringList>

pertubis::PackageModel::PackageModel(QObject* pobj) : QAbstractItemModel(pobj),m_root(new Item()),m_box(0)
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
        switch (ix.column())
        {
            case Item::io_installed:
                return QBrush(QColor(0,0,255));
                break;
            case Item::io_mask_reasons:
                return QBrush(QColor(255,0,0));
                break;
            case Item::io_change:
                return QBrush(QColor(0,255,0));
                break;
            default:
                return QBrush(QColor(0,0,0));
                break;

                // case Item::io_package:
                //  {
                //      if (item->state() == Item::is_stable)
                //           return QBrush(QColor(0,255,0));
                //      if (item->state() == Item::is_unstable)
                //          return QBrush(QColor(255,200,0));
                //      if (item->state() == Item::is_masked)
                //          return QBrush(QColor(255,0,0));
                //  }
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

    Item *parentItem;

    if (!parentIndex.isValid())
        parentItem = m_root;
    else
        parentItem = static_cast<Item*>(parentIndex.internalPointer());

    Item *childItem = parentItem->child(row);
    if (childItem)
        return QAbstractItemModel::createIndex(row, column, childItem);
    else
        return QModelIndex();
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

void pertubis::PackageModel::slotClear()
{
    if (m_root != 0)
    {
        delete m_root;
        m_root = new Item();
        reset();
    }
}

void pertubis::PackageModel::slotAppendPackage(Item* item)
{
    if (m_root != 0)
    {
        m_root->appendChild(item);
        layoutChanged();
    }
}

void pertubis::PackageModel::slotPrependPackage(Item* item)
{
    if (m_root != 0)
    {
        m_root->prependChild(item);
        layoutChanged();
    }
}


bool pertubis::PackageModel::setSelectionData( const QModelIndex & ix, int taskid, bool mystate)
{
    Item* item = static_cast<Item*>(ix.internalPointer());
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
    m_header = labels;
    emit headerDataChanged(Qt::Horizontal,0,labels.count()-1);
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
