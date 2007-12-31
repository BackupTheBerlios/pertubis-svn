
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SELECTION_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_SELECTION_MODEL_H 1

#include "PackageModel.hh"
#include <QStringList>

namespace pertubis
{
    class Package;
    class TaskBox;

    /*! \brief holds package data fetched from paludis in a tree structure
     *
     * \ingroup ModelClass
     */
    class SelectionModel : public PackageModel
    {
        Q_OBJECT

    public:

        enum SelectionPackageOrder
        {
            spo_selected,
            spo_package,
            spo_category,
            spo_version,
            spo_repository,
            spo_last
        };

        SelectionModel(QObject* parent);

        virtual ~SelectionModel() {};

        ///@name Content information
        ///@{

        QVariant data ( const QModelIndex & index,
                        int role = Qt::DisplayRole ) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;
    };
}

#endif