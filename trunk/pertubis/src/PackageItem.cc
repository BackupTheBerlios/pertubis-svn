
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


#include <QStringList>
#include <QDebug>
#include "PackageItem.hh"

namespace pertubis
{


}

pertubis::PackageItem::PackageItem( const QList<QVariant> &dats) :
                                    Item(dats,is_stable)
{
}

pertubis::PackageItem::PackageItem( const paludis::tr1::shared_ptr<const paludis::PackageID>& id,
                                    const QList<QVariant> &dats) :
                                    Item(dats,Item::is_stable),
                                    m_id(id)
{
}

paludis::tr1::shared_ptr<const paludis::PackageID> pertubis::PackageItem::ID() const
{
    return m_bestChild->ID();
}

pertubis::PackageItem::~PackageItem()
{
}

bool pertubis::PackageItem::available() const
{
    return (m_state != Item::is_masked);
}

pertubis::Item::UpdateRange pertubis::PackageItem::updateRange() const
{
    return ur_parent;
}

QString pertubis::stateDescription(Item::ItemState status)
{
    switch (status)
    {
        case Item::is_stable:
            return "stable";
        case Item::is_unstable:
            return "possibly unstable";
        case Item::is_masked:
            return "not avaliable since masked";
        default:
            return "unknown package status";
    }
}
