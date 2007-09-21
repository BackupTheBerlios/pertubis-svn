
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_ITEM_H
#define _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_ITEM_H

#include "Item.hh"
#include <QList>
#include <QVariant>
#include <paludis/util/tr1_memory.hh>

namespace paludis
{
    class PackageID;
}

namespace pertubis
{
    class PackageItem : public Item
    {
        Q_OBJECT

        public:

            PackageItem() {}

            PackageItem(const QList<QVariant> &data);

            PackageItem(const paludis::tr1::shared_ptr<const paludis::PackageID>& id,
                        const QList<QVariant> &data);

            PackageItem(const paludis::tr1::shared_ptr<const paludis::PackageID>& id,
                        QString    catname,
                          QString    packname,
                        QVariantList selections,
                        const QList<QVariant> &data);

            ~PackageItem();

            bool available() const;

            UpdateRange updateRange() const;

            paludis::tr1::shared_ptr<const paludis::PackageID> ID();

        private:

            paludis::tr1::shared_ptr<const paludis::PackageID> m_id;

    };

    QString stateDescription(Item::ItemState status);

    /*! \brief helps us creating correctly a PackageItem
    *
    */
    Item* makePackageItem(QVariantList selections,
                                 QString pack,
                                 QString cat,
                                 QString rep,
                                 int mystate=Qt::Unchecked);
}


#endif
