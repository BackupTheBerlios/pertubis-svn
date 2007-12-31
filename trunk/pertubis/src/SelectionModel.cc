
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

#include "SelectionModel.hh"
#include "Package.hh"
#include <QColor>
#include <QBrush>
#include <QVariant>

pertubis::SelectionModel::SelectionModel(QObject* pobj) : PackageModel(pobj)
{
}

Qt::ItemFlags pertubis::SelectionModel::flags(const QModelIndex &mix) const
{
    if (!mix.isValid() )
        return 0;
    switch (mix.column())
    {
        case po_install:
        case po_deinstall:
            return Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
            break;
            break;
        default:
            return 0;
    }
}

QVariant pertubis::SelectionModel::data ( const QModelIndex & ix, int role) const
{
    if (!ix.isValid())
        return QVariant();

    Package* item = static_cast<Package*>(ix.internalPointer());

    if (role == Qt::BackgroundRole)
    {
        if (0  == ix.row() % 2 )
            return QBrush(QColor(225,225,225));
        else
            return QBrush(QColor(255,255,255));
    }

    if (role == Qt::ForegroundRole)
    {
        if ( pt_child &&
             ( tr("block") == item->data(spo_package).toString()
             || tr("masked")  == item->data(spo_package).toString()) )
        {
            return QBrush(QColor(255,0,0));
        }
        return QBrush(QColor(0,0,0));
    }

    if (role == Qt::DisplayRole )
    {
        return item->data(ix.column());
    }

    return QVariant();
}
