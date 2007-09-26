
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_LIST_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_LIST_MODEL_H

#include <QAbstractListModel>

#include <paludis/util/tr1_memory.hh>
#include "ThreadBase.hh"

#include <QStringList>

namespace pertubis
{
    class RepositoryListItem
    {
        public:

            RepositoryListItem(){}
            RepositoryListItem(QString r) : m_name(r) {}
            QVariant name() { return m_name;}
        private:
            QVariant m_name;
    };

    /*! \brief not finished
     *
     */
    class RepositoryListThread : public ThreadBase
    {
        Q_OBJECT
        public:
            RepositoryListThread(QObject* pobj,
                             DatabaseView* main) : ThreadBase(pobj,main) {}

            void run();

        signals:

            void sendNames(QStringList list);
    };

    /*! \brief This qt model class holds repositories.
     *
     */
    class RepositoryListModel : public QAbstractListModel
    {
        Q_OBJECT

        public:

            RepositoryListModel( QObject* parent);
            ~RepositoryListModel();

            QVariant headerData(int section, Qt::Orientation orientation, int role) const;
            void setHorizontalHeaderLabels ( const QStringList & labels );

            int rowCount( const QModelIndex & index ) const;
            int columnCount( const QModelIndex & index ) const;
            QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

        private:
            QList<RepositoryListItem>   m_data;
            QStringList                 m_header;

        public slots:
            void slotResult(QStringList cl);
    };
}
#endif