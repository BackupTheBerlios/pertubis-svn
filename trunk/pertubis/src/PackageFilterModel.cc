
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
#include "RepositoryListModel.hh"
#include "Item.hh"
#include <QSet>
#include <QDebug>

pertubis::PackageFilterModel::PackageFilterModel(QObject * pobj,const RepositoryListModel& model) : QSortFilterProxyModel(pobj),m_model(model)
{
}

bool pertubis::PackageFilterModel::filterAcceptsRow(int sourceRow,
    const QModelIndex &sourceParent) const
{
//     qDebug() << "pertubis::PackageFilterModel::filterAcceptsRow()";
    QSet<QString> repos(m_model.activeRepositories());
    QModelIndex ix1 = sourceModel()->index(sourceRow,Item::io_repository,sourceParent);
    Item* parent = static_cast<Item*>(sourceParent.internalPointer());
    if (parent)
    {
        Item* child = parent->child(sourceRow);
        if (child->updateRange() == Item::ur_child)
            return true;
    }
    return repos.contains(sourceModel()->data(ix1).toString());
}