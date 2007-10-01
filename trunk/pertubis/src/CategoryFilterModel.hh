
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_CATEGORY_FILTER_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_CATEGORY_FILTER_MODEL_H 1

#include <QSortFilterProxyModel>
#include <QVariant>
#include <QModelIndex>

namespace pertubis
{
    class RepositoryListModel;

    class CategoryFilterModel : public QSortFilterProxyModel
    {
        Q_OBJECT
        public:

            CategoryFilterModel(QObject * pobj,const RepositoryListModel& main);

            bool filterAcceptsRow(int sourceRow,
                                  const QModelIndex &sourceParent) const;

        private:

            const RepositoryListModel& m_model;


    };

}

#endif

