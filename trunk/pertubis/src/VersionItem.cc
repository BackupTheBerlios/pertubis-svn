
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

#include "VersionItem.hh"
#include <QDebug>

pertubis::VersionItem::VersionItem( const QList<QVariant> &dats) :
                                    Item(dats,is_stable)
{
}

pertubis::VersionItem::VersionItem( const paludis::tr1::shared_ptr<const paludis::PackageID>& id,
                                    const QList<QVariant> &dats) :
                                    Item(id,dats,Item::is_stable)
{
    // VersionItem has no childs in the moment!
//     setBestChild(this);
}

bool pertubis::VersionItem::available() const
{
    return (m_state != Item::is_masked);
}

pertubis::Item::UpdateRange pertubis::VersionItem::updateRange() const
{
    return Item::ur_child;
}

