
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

#include "PackageFilterModel.hh"
#include "Item.hh"

#include <QDebug>

pertubis::PackageFilterModel::PackageFilterModel(QObject * pobj) : QSortFilterProxyModel(pobj)
{
}

bool pertubis::PackageFilterModel::filterAcceptsRow(int sourceRow,
    const QModelIndex &sourceParent) const
{
//     qDebug() << m_repositories << sourceRow << sourceParent;
//     if (sourceParent == QModelIndex() )
//     {
//         QModelIndex pmi = sourceModel()->index(sourceRow,0,sourceParent);
//         for (uint i=0,iEnd=sourceModel()->rowCount(pmi);i<iEnd;i++)
//         {
//             QModelIndex vx = sourceModel()->index(i,Item::io_repository,pmi);
//             if (m_repositories.contains(sourceModel()->data(vx).toString()))
//                 return true;
//         }
//         return false;
        return true;
//     }
//     QModelIndex ix1 = sourceModel()->index(sourceRow,Item::io_repository,sourceParent);
//     return m_repositories.contains(sourceModel()->data(ix1).toString());
}
