
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

#include "CategoryModel.hh"
#include "CategoryItem.hh"

#include <QString>
#include <QDebug>


bool CategoryItemSorter(pertubis::CategoryItem* a,pertubis::CategoryItem* b)
{
    return a->name() < b->name();
}

pertubis::CategoryModel::CategoryModel(QObject* pobj) : QAbstractTableModel(pobj)
{
}

pertubis::CategoryModel::~CategoryModel()
{
    qDeleteAll(m_data);
    m_data.clear();
}

void pertubis::CategoryModel::setHorizontalHeaderLabels ( const QStringList & labels )
{
    m_header = labels;
}

QVariant pertubis::CategoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical || section >= m_header.count() )
        return QVariant();
    return *(m_header.begin() + section);
}

void pertubis::CategoryModel::slotAppendCategory(QMap<QString, QSet<QString> > cats)
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

int pertubis::CategoryModel::rowCount( const QModelIndex & pobj ) const
{
    return pobj.isValid() ? 0 : m_data.count();
}

int pertubis::CategoryModel::columnCount( const QModelIndex & pobj ) const
{
    return pobj.isValid() ? 0 : m_header.count();
}

QVariant pertubis::CategoryModel::data ( const QModelIndex & mix, int role) const
{
    if (!mix.isValid())
         return QVariant();

    if (mix.row() >= m_data.size())
         return QVariant();

     if (role == Qt::DisplayRole)
         return m_data.at(mix.row())->name();
     else
         return QVariant();
}

QModelIndex pertubis::CategoryModel::index(int row, int column, const QModelIndex &parentIndex) const
{
    if (!hasIndex(row, column, parentIndex))
        return QModelIndex();

    CategoryItem *item = m_data.value(row);
    if (item)
        return QAbstractItemModel::createIndex(row, column, item);
    else
        return QModelIndex();
}
