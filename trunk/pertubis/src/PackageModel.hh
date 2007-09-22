
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_PACKAGE_MODEL_H

#include <QAbstractItemModel>
#include <QList>
#include <QStringList>
#include <QVariant>
#include <paludis/util/tr1_memory.hh>

class QAction;

namespace pertubis
{
    class Item;
    class PackageItem;
    class VersionItem;
    class TaskBox;

    /*! \brief holds package data fetched from paludis in a tree structure
    *
    */
    class PackageModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:

        PackageModel(QObject* parent);

        ~PackageModel();

        int columnCount ( const QModelIndex & parent = QModelIndex() ) const;

        QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

        Qt::ItemFlags flags(const QModelIndex &index) const;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        bool setHeaderData ( int section, Qt::Orientation orientation, const QVariant & value, int role = Qt::EditRole );

        QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;

        QModelIndex parent ( const QModelIndex & index ) const;

        int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

        bool setSelectionData( const QModelIndex & ix, int taskid, bool mystate);

        void setHorizontalHeaderLabels ( const QStringList & labels );

        const Item* root() const { return m_root;}

        void setBox(TaskBox* t);

    public slots:

        void slotAppendPackage(Item* item);
        void slotSetRoot(Item* item);

    private:

        QModelIndex createIndex ( int row, int column, void * ptr = 0 ) const;

        Item*            m_root;
        QStringList        m_header;

        QAction*        m_toggleInstall;
        QAction*        m_toggleDeinstall;
        QAction*        m_toggleSlot;
        TaskBox*        m_box;

    };
}

#endif
