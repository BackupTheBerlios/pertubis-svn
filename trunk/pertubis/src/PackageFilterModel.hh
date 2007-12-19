
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_FILTER_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_FILTER_MODEL_H 1

#include <QSortFilterProxyModel>
#include <QSet>
#include <QString>

class QModelIndex;

namespace pertubis
{
    /*! \brief filters packages by repository
     *
     * \ingroup ItemModelClass
     * If any repository of m_repositories does not contain this package, the filter rejects the package
     */
    class PackageFilterModel : public QSortFilterProxyModel
    {
        Q_OBJECT
        public:

            PackageFilterModel(QObject * pobj);

            void setFilter(const QSet<QString>& repos) { m_repositories = repos;}
            bool filterAcceptsRow(int sourceRow,
                             const QModelIndex &sourceParent) const;

            void setFilterOff(bool setOff) { m_filterOff=setOff;}

        private:

            QSet<QString> m_repositories;
            bool    m_filterOff;
    };

}

#endif

