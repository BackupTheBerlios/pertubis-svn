
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

#include "ReportPackageModel.hh"
#include "ReportPackage.hh"

#include <QColor>
#include <QBrush>
#include <QVariant>
#include <QDebug>
#include <QStringList>

pertubis::ReportPackageModel::ReportPackageModel(QObject* pobj) : QAbstractItemModel(pobj), m_root(new ReportPackage())
{
}

pertubis::ReportPackageModel::~ReportPackageModel()
{
    if (m_root)
        delete m_root;
}

Qt::ItemFlags pertubis::ReportPackageModel::flags(const QModelIndex &mix) const
{
    if (!mix.isValid() )
        return 0;
    switch (mix.column())
    {
        case ReportPackage::ro_deinstall:
            return Qt::ItemIsEditable;
            break;
        default:
            return 0;
    }
}

QVariant pertubis::ReportPackageModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical || section >= m_header.count() )
        return QVariant();
    return *(m_header.begin() + section);
}

QVariant pertubis::ReportPackageModel::data ( const QModelIndex & ix, int role) const
{
    if (!ix.isValid())
        return QVariant();
    if (ReportPackage::ro_last <= ix.column() )
        return QVariant();
    ReportPackage* item = static_cast<ReportPackage*>(ix.internalPointer());
    if (role == Qt::BackgroundRole)
    {
        if (0  == ix.row() % 2 )
            return QBrush(QColor(225,225,225));
        else
            return QBrush(QColor(255,255,255));
    }
    else if (role == Qt::ForegroundRole)
    {
        if (item->isTag() || ReportPackage::ro_deinstall == ix.column() )
            return QBrush(QColor(255,0,0));
        else
            return QBrush(QColor(0,0,0));
    }
    else if (role == Qt::CheckStateRole &&
             ! item->isTag() &&
             ReportPackage::ro_deinstall == ix.column())
    {
        return item->data(ix.column());
    }
    else if (role == Qt::DisplayRole &&
             ReportPackage::ro_deinstall != ix.column())
    {
        return item->data(ix.column());
    }
    else
        return QVariant();
}

QModelIndex pertubis::ReportPackageModel::index(int row, int column, const QModelIndex &parentIndex) const
{
    if (!hasIndex(row, column, parentIndex))
        return QModelIndex();

    ReportPackage *parentItem;

    if (!parentIndex.isValid())
        parentItem = m_root;
    else
        parentItem = static_cast<ReportPackage*>(parentIndex.internalPointer());

    ReportPackage *childItem = parentItem->child(row);
    if (childItem)
        return QAbstractItemModel::createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex pertubis::ReportPackageModel::parent(const QModelIndex &mix) const
{
    if (!mix.isValid())
        return QModelIndex();

    ReportPackage *childItem = static_cast<ReportPackage*>(mix.internalPointer());
    ReportPackage *parentItem = childItem->parent();

    if (parentItem == m_root)
        return QModelIndex();

    return QAbstractItemModel::createIndex(parentItem->row(), 0, parentItem);
}

int pertubis::ReportPackageModel::rowCount(const QModelIndex &pmi) const
{
    ReportPackage *parentItem;
    if (pmi.column() > 0)
        return 0;

    if (!pmi.isValid())
        parentItem = m_root;
    else
        parentItem = static_cast<ReportPackage*>(pmi.internalPointer());

    return parentItem->tagCount();
}

int pertubis::ReportPackageModel::columnCount(const QModelIndex &pmi) const
{
    if (pmi.isValid())
        return static_cast<ReportPackage*>(pmi.internalPointer())->columnCount();
    else if (m_root != 0)
        return m_root->columnCount();
    else
        return 0;
}

void pertubis::ReportPackageModel::clear()
{
    if (m_root != 0)
    {
        delete m_root;
        m_root = new ReportPackage();
        reset();
    }
}

void pertubis::ReportPackageModel::appendPackage(ReportPackage* item)
{
    if (m_root != 0)
    {
        m_root->addTag(item);
        layoutChanged();
    }
}

void pertubis::ReportPackageModel::setHorizontalHeaderLabels ( const QStringList & labels )
{
    qDebug() << "pertubis::ReportPackageModel::setHorizontalHeaderLabels()" << labels;
    m_header = labels;
    emit headerDataChanged(Qt::Horizontal,0,labels.count() -1);
}


void pertubis::ReportPackageModel::unselectAll()
{
    qDebug() << "pertubis::ReportPackageModel::unselectAll()";
    for (ReportPackage::ReportPackageIterator iStart(m_root->childBegin()),
         iEnd(m_root->childEnd());
         iStart != iEnd;
         ++iStart)
    {
        if (*iStart != 0)
        {
            (*iStart)->setData(ReportPackage::ro_deinstall,false);
        }
    }
    emit layoutChanged();
}

bool pertubis::ReportPackageModel::setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role)
{
    if (section >= 0 && orientation == Qt::Horizontal && section < m_header.count() && role == Qt::EditRole)
    {
        m_header.replace(section,value.toString());
        emit headerDataChanged(orientation,section,section);
        return true;
    }
    return false;
}
