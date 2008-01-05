
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

#include "ReportPackageModel.hh"
#include "ReportPackage.hh"

#include <QColor>
#include <QBrush>
#include <QVariant>
#include <QDebug>
#include <QStringList>

pertubis::ReportPackageModel::ReportPackageModel(QObject* pobj) : PackageModel(pobj)
{
    clear();
}

pertubis::ReportPackageModel::~ReportPackageModel()
{
    if (m_root)
        delete m_root;
}

QVariant pertubis::ReportPackageModel::data ( const QModelIndex & ix, int role) const
{
    if (!ix.isValid())
        return QVariant();
    if (rpho_last <= ix.column() )
        return QVariant();
    ReportPackage* item = static_cast<ReportPackage*>(ix.internalPointer());
    if (role == Qt::BackgroundRole)
    {
        if (tk_normal != item->tag())
            return QBrush(QColor(255,200,200));
    }
    else if (role == Qt::ForegroundRole)
    {
        if (tk_normal != item->tag()
            || rpho_deinstall == ix.column() )
            return QBrush(QColor(255,0,0));
        else
            return QBrush(QColor(0,0,0));
    }
    else if (role == Qt::CheckStateRole &&
             tk_normal == item->tag() &&
             rpho_deinstall == ix.column())
    {
        return item->data(ix.column());
    }
    else if (role == Qt::DisplayRole &&
             rpho_deinstall != ix.column())
    {
        return item->data(ix.column());
    }

    return QVariant();
}

void pertubis::ReportPackageModel::unselectAll()
{
    qDebug() << "pertubis::ReportPackageModel::unselectAll()";
    for (ReportPackage::PackageIterator iStart(m_root->childBegin()),
         iEnd(m_root->childEnd());
         iStart != iEnd;
         ++iStart)
    {
        if (*iStart != 0)
        {
            (*iStart)->setData(rpho_deinstall,false);
        }
    }
    emit layoutChanged();
}

void pertubis::ReportPackageModel::clear()
{
    qDebug() << "pertubis::ReportPackageModel::clear() start";
    m_root = new ReportPackage(paludis::tr1::shared_ptr<const paludis::PackageID>(),
                                   QVector<QVariant>(rpho_last), tk_normal);
    qDebug() << "pertubis::ReportPackageModel::clear() done";
}
