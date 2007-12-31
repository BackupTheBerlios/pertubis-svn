

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

#include "ReportPackage.hh"
#include <QDebug>
#include <paludis/package_id.hh>

pertubis::ReportPackage::ReportPackage()  :
        m_data(QList<QVariant>() << Qt::Checked<< "" << "" << ""),
        m_id(paludis::tr1::shared_ptr<const paludis::PackageID>()),
        m_parent(0)
{
}

pertubis::ReportPackage::ReportPackage(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                    const QList<QVariant> &dats,
                    bool istag)  :
                    m_data(dats),
                    m_id(id),
                    m_isTag(istag)
{
}

pertubis::ReportPackage::~ReportPackage()
{
    m_parent=0;
}

int pertubis::ReportPackage::columnCount() const
{
    return m_data.count();
}

void pertubis::ReportPackage::addTag(ReportPackage *item)
{
    m_tags.append(item);
    item->setParent(this);
}

QVariant pertubis::ReportPackage::data(int column) const
{
    return m_data.value(column);
}

pertubis::ReportPackage* pertubis::ReportPackage::parent() const
{
    return m_parent;
}

void pertubis::ReportPackage::setData(int column, QVariant dats)
{
    if (column < m_data.count() )
    {
        m_data.replace(column, dats);
    }
}

void pertubis::ReportPackage::setParent(ReportPackage* pitem)
{
    m_parent=pitem;
}

paludis::tr1::shared_ptr<const paludis::PackageID> pertubis::ReportPackage::ID() const
{
    return m_id;
}
