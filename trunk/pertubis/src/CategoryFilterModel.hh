
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
#include <QSet>
#include <QString>

class QModelIndex;

namespace pertubis
{
    /*! @brief filters categories by repository
     *
     * @ingroup ItemModelClass
     * If a category is not member of a repository, this filter rejects the category
     */
    class CategoryFilterModel : public QSortFilterProxyModel
    {
        Q_OBJECT
        public:

            ///@name Constructors
            ///@{
            /// std constructor
            CategoryFilterModel(QObject * pobj);
            ///@}

            ///@name Content modification
            ///@{

            /*! @brief sets the whitelist for the filter
             * @param repos repositoris, which will be accepted by this filter
             *
             */
            void setFilter(const QSet<QString>& repos) { m_repositories = repos;}
            ///@}

            /*! @brief filter a row
             * @param sourceRow the row ( CategoryItem ) to be filtered
             * @param sourceParent the parent row which has the sourceRow CategoryItem as child Item
             * filters a row (Item) from the source model if CategoryItem::m_repos has no repository common with CategoryFilterModel::m_repositories
             */
            bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

        private:

            /// a whitelist of repositoris to accept when filtering
            QSet<QString> m_repositories;
    };

}

#endif

