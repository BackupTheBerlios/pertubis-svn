
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_MODEL_H 1

#include <QAbstractItemModel>
#include <QStringList>
#include <QModelIndex>
#include <QVariant>

class QAction;

namespace pertubis
{
    class Package;
    class TaskBox;

    /*! \brief holds package data fetched from paludis in a tree structure
     *
     * \ingroup PackageModelClass
     */
    class PackageModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:

        PackageModel(QObject* parent=0);

        ~PackageModel();

        QVariant data ( const QModelIndex & index,
                        int role = Qt::DisplayRole ) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;

        Package* getRootPackage() const { return m_root;}

        QModelIndex index ( int row, int column,
                            const QModelIndex & parent = QModelIndex() ) const;

        QModelIndex parent ( const QModelIndex & index ) const;

        int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

        int columnCount ( const QModelIndex & parent = QModelIndex() ) const;

        QVariant headerData(int section,
                            Qt::Orientation orientation,
                            int role) const;

        void setHorizontalHeaderLabels ( const QStringList & labels );


        bool setHeaderData ( int section,
                             Qt::Orientation orientation,
                             const QVariant & value,
                             int role = Qt::EditRole );

        bool setSelectionData( const QModelIndex & ix, int taskid, bool mystate);

        void setBox(TaskBox* t);

    public slots:

        void slotAppendPackage(Package* item);
        void slotPrependPackage(Package* item);
        void slotClear();

    private:

        QModelIndex createIndex ( int row, int column, void * ptr = 0 ) const;

        Package*            m_root;
        TaskBox*         m_box;
        QStringList      m_header;

        QAction*        m_toggleInstall;
        QAction*        m_toggleDeinstall;
        QAction*        m_toggleSlot;

    };
}

#endif
