
/* Copyright (C) 2007-2008 Stefan Koegl
*
* This file is part of perturbis
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

#include "SelectionModel.hh"
#include "Package.hh"
#include <QColor>
#include <QBrush>
#include <QVariant>

using namespace perturbis;

SelectionModel::SelectionModel(QObject* pobj) : PackageModel(pobj)
{
    clear();
}

QVariant
SelectionModel::data (const QModelIndex & ix, int role) const
{
    if (!ix.isValid())
        return QVariant();

    Package* item = static_cast<Package*>(ix.internalPointer());

    if (role == Qt::BackgroundRole)
    {
        if (pt_child == item->itemType() ||
             0 != item->childCount())
            return QBrush(QColor(255,200,200));
    }

    if (role == Qt::ForegroundRole)
    {
        if (pho_installed == ix.column())
            return QBrush(QColor(0,0,255));
        if (pt_child == item->itemType())
        {
            return QBrush(QColor(255,0,0));
        }
        return QBrush(QColor(0,0,0));
    }

    if (role == Qt::CheckStateRole)
    {
        switch (ix.column())
        {
            case spho_install:
            case spho_deinstall:
            case spho_installed:
                return item->data(ix.column());
            default:
                return QVariant();
        }
    }

    if (role == Qt::DisplayRole)
    {
        switch (ix.column())
        {
            case spho_install:
            case spho_deinstall:
            case spho_installed:
                return QVariant();
            default:
                return item->data(ix.column());
        }
    }

    return QVariant();
}

void
SelectionModel::clear()
{
    if (m_root != 0)
    {
        delete m_root;
        m_root = new Package(paludis::tr1::shared_ptr<const paludis::PackageID>(),
            QVector<QVariant>(spho_last),
            ps_stable,
            pt_parent,
            0);
        reset();
    }
}
