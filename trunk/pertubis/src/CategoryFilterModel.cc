
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

#include "CategoryFilterModel.hh"
#include "CategoryItem.hh"
#include <QSet>
#include <QModelIndex>

pertubis::CategoryFilterModel::CategoryFilterModel(QObject * pobj) : QSortFilterProxyModel(pobj)
{
}

bool pertubis::CategoryFilterModel::filterAcceptsRow(int sourceRow,
        const QModelIndex &sourceParent) const
{
    QModelIndex ix1 = sourceModel()->index(sourceRow,0,sourceParent);
    CategoryItem* p_item = static_cast<CategoryItem*>(ix1.internalPointer());
    for (QSet<QString>::const_iterator start(p_item->repos().constBegin()),end(p_item->repos().constEnd());
         start!= end;++start)
    {
        if (m_repositories.contains(*start))
            return true;
    }
    return false;
}
