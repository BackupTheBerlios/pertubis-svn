
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_CATEGORY_ITEM_H
#define _PERTUBIS_ENTRY_PROTECTOR_CATEGORY_ITEM_H 1

#include <QSet>
#include <QString>

namespace pertubis
{

    /*! \brief stores category name with associated repositories
    * \ingroup ItemModelClass
    */
    class CategoryItem
    {
        public:

            ///@name Constructors
            ///@{

            /// std constructor
            CategoryItem(const QString& catname, const QSet<QString>& reps)  : m_name(catname), m_repos(reps), m_change(false) {}
            ///@}

            ///@name Content modification
            ///@{

            /// returns the category name
            const QString& name() const {return m_name;}

            /// returns the repositories in which this category exists
            const QSet<QString>& repos() const { return m_repos;}

            /// sets a flag
            void setChange(bool c) { m_change=c;}

            ///\}

            ///@name Content information
            ///@{

            /// returns the change state
            bool change() const { return m_change;}


        private:

            /// name of this category ( paludis::CategoryNamePart )
            QString         m_name;

            /// the repositories in which this category exists
            QSet<QString>   m_repos;

            /// shows if there are new packages in this category
            bool            m_change;
    };

}
#endif

