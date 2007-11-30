
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
#include <QMap>
#include <QSet>
#include <QString>

namespace pertubis
{

    class CategoryItem;
    /*! @brief This qt model class holds all software categories of all repositories it is instructed to store.
     *
     * @ingroup ItemModelClass
     */
    class CategoryModel : public QAbstractTableModel
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            CategoryModel( QObject* parent);
            ///@}

            ~CategoryModel();

            /// @name Content modification
            ///@{

            /// sets the header data
            void setHorizontalHeaderLabels ( const QStringList & labels );

            ///@}

            /// @name Content information
            ///@{

            /// returns the column data of the header
            QVariant headerData(int section, Qt::Orientation orientation, int role) const;

            /*! @brief creates a QModelIndex for the CategoryItem descibed by the parameters
             * @param row the row of the cell seen from parentIndex' perspective
             * @param column the row of the cell seen from parentIndex' perspective
             * @param parentIndex the parent or the CategoryItems' value to descibe
             * @returns the new QModelIndex
             */
            QModelIndex index(int row, int column, const QModelIndex &parentIndex) const;

            /// returns the number of rows the CategoryItem descibed by QModelIndex has
            int rowCount( const QModelIndex & index ) const;

            /// returns the number of columns the CategoryItem descibed by QModelIndex has
            int columnCount( const QModelIndex & index ) const;

            /// returns the data in cell with QModelIndex index and Qt::DisplayRole role
            QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole) const;
            ///@}

        private:

            /// table rows / a container of CategoryItem
            QList<CategoryItem*> m_data;

            /// the header data
            QStringList        m_header;

        public slots:

            /// receives the data for a new CategoryItem to append and creates it
            void slotAppendCategory(QMap<QString, QSet<QString> > map);

            /// sets for category cat the change attribute to true
            void slotChangeInCat(QString cat);
    };
}
#endif

