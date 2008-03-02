
/* Copyright (C) 2007 Stefan Koegl
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

#include "CategoryModel.hh"
#include "CategoryItem.hh"

#include <QString>
#include <QDebug>
#include <QColor>
#include <QBrush>

using namespace pertubis;

bool
CategoryItemSorter(CategoryItem* a,CategoryItem* b)
{
    return a->name() < b->name();
}

CategoryModel::CategoryModel(QObject* pobj) : QAbstractTableModel(pobj)
{
}

CategoryModel::~CategoryModel()
{
    qDeleteAll(m_data);
    m_data.clear();
}
int
CategoryModel::indexOf (const QString & text)
{
    for (QList<CategoryItem*>::const_iterator iStart(m_data.constBegin()),iEnd(m_data.constEnd());
        iStart != iEnd;++iStart)
    {
        if (text == (*iStart)->name())
            return m_data.indexOf(*iStart);
    }
    return -1;
}

QVariant
CategoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical || section >= m_header.count())
        return QVariant();
    return *(m_header.begin() + section);
}

void
CategoryModel::appendCategory(QMap<QString, QSet<QString> > cats)
{
    for(QMap<QString,QSet<QString> >::const_iterator cStart(cats.constBegin()),
            end(cats.constEnd());
            cStart != end;
            ++cStart)
    {
        m_data.push_back(new CategoryItem(cStart.key(),cStart.value()));
    }
//     QModelIndex i(index(m_data.indexOf(cl),0,QModelIndex()));
//     qSort(m_data.begin(),m_data.end(),CategoryItemSorter);
    emit layoutChanged();
}

void
CategoryModel::appendCategory(CategoryItem* item)
{
    m_data.push_back(item);
    emit layoutChanged();
}

int
CategoryModel::rowCount(const QModelIndex & pobj) const
{
    return pobj.isValid() ? 0 : m_data.count();
}

int
CategoryModel::columnCount(const QModelIndex & pobj) const
{
    return pobj.isValid() ? 0 : m_header.count();
}

void
CategoryModel::slotChangeInCat(QString cat)
{
    CategoryItem* item;
    foreach (item,m_data)
    {
        if (item->name() == cat)
        {
            item->setChange(true);
            emit layoutChanged();
            break;
        }
    }
}

QVariant
CategoryModel::data (const QModelIndex & mix, int role) const
{
    if (!mix.isValid())
         return QVariant();

    if (mix.row() >= m_data.size())
         return QVariant();

    if (role == Qt::ForegroundRole)
    {
        return QBrush(QColor(0,0,0));
    }

    if (role == Qt::DisplayRole)
    {
        if (mix.column() == 0)
            return m_data.at(mix.row())->name();
    }
//     if (role == Qt::CheckStateRole)
//         if (mix.column() == 0)
//             return m_data.at(mix.row())->change();
    return QVariant();
}

QModelIndex
CategoryModel::index(int row, int column, const QModelIndex &parentIndex) const
{
    if (!hasIndex(row, column, parentIndex))
        return QModelIndex();

    CategoryItem *item = m_data.value(row);
    if (item)
        return QAbstractItemModel::createIndex(row, column, item);
    else
        return QModelIndex();
}

void
CategoryModel::clear()
{
    qDeleteAll(m_data);
    m_data.clear();
    emit layoutChanged();
}
