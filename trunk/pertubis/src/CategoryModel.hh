
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_CATEGORY_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_CATEGORY_MODEL_H 1

#include <QAbstractTableModel>
#include <QStringList>

namespace pertubis
{

    class CategoryItem;
    /*! \brief This qt model class holds all software categories of all repositories it is instructed to store.
    *
    */
    class CategoryModel : public QAbstractTableModel
    {
        Q_OBJECT

        public:

            CategoryModel( QObject* parent);
            ~CategoryModel();

            QVariant headerData(int section, Qt::Orientation orientation, int role) const;
            void setHorizontalHeaderLabels ( const QStringList & labels );

            QModelIndex index(int row, int column, const QModelIndex &parentIndex) const;
            int rowCount( const QModelIndex & index ) const;
            int columnCount( const QModelIndex & index ) const;
            QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

        private:
            QList<CategoryItem*> m_data;
            QStringList        m_header;

    public slots:
        void slotPopulateModel(QList<CategoryItem*> list);
    };
}
#endif

