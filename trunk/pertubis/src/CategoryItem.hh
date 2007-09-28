
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_CATEGORY_ITEM_H
#define _PERTUBIS_ENTRY_PROTECTOR_CATEGORY_ITEM_H 1

#include <QStringList>

namespace pertubis
{

    /*! \brief stores category name with associated repositories
    *
    */
        class CategoryItem
    {
        public:

            CategoryItem(const QString& name,const QStringList& repos);

            QString name() const;
            QStringList data() const;

        private:

            QString     m_name;
            QStringList m_repos;
    };

}
#endif