
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_VERSION_ITEM_H
#define _PERTUBIS_ENTRY_PROTECTOR_VERSION_ITEM_H

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
    class VersionItem : public Item
    {
        Q_OBJECT

        public:

            VersionItem() {}

            VersionItem(const QList<QVariant> &data);

            VersionItem(const paludis::tr1::shared_ptr<const paludis::PackageID>& id,
                        const QList<QVariant> &data);

            ~VersionItem() {}

            bool available() const;

            UpdateRange updateRange() const;

            paludis::tr1::shared_ptr<const paludis::PackageID> ID() const;

        private:
            paludis::tr1::shared_ptr<const paludis::PackageID> m_id;
    };
}

#endif
