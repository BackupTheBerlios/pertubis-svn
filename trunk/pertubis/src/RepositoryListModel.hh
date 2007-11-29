
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
#define _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_LIST_MODEL_H 1

#include <QAbstractListModel>

#include <paludis/util/tr1_memory.hh>
#include <paludis/name-fwd.hh>
#include "ThreadBase.hh"

#include <QStringList>
#include <QList>
#include <QSet>
#include <QVariant>

namespace pertubis
{
     /*! \brief Represents a repository
     *
     * \ingroup ItemModelClass
     */
    class RepositoryListItem
    {
        public:

            RepositoryListItem();
            RepositoryListItem(const paludis::RepositoryName & name);
            QVariant data(int col) const { return m_data.value(col);}
            bool setData(int col,const QVariant& value);
        private:
            QList<QVariant>     m_data;
    };

    /*! \brief not finished
     *
     * \ingroup Thread
     * \ingroup PaludisAdapter
     */
    class RepositoryListThread : public ThreadBase
    {
        Q_OBJECT
        public:
            RepositoryListThread(QObject* pobj,
                                 paludis::tr1::shared_ptr<paludis::Environment>  env,
                                TaskBox* box) : ThreadBase(pobj,env,box) {}

            void run();

        signals:

            void sendNames(QList<RepositoryListItem*> cl);
    };

    /*! \brief This qt model class holds repositories.
     *
     * \ingroup ItemModelClass
     */
    class RepositoryListModel : public QAbstractListModel
    {
        Q_OBJECT

        public:

            typedef QList<RepositoryListItem*>::const_iterator RepositoryConstIterator;

            RepositoryListModel( QObject* parent);
            ~RepositoryListModel();

            QVariant headerData(int section, Qt::Orientation orientation, int role) const;
            void setHorizontalHeaderLabels ( const QStringList & labels );

            const QSet<QString>& activeRepositories() const;

            Qt::ItemFlags flags(const QModelIndex &mix) const;

            int rowCount( const QModelIndex & index ) const;
            int columnCount( const QModelIndex & index ) const;
            QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
            void changeActiveRepos(const QString& name);

            bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

        private:
            QList<RepositoryListItem*>   m_data;
            QStringList                 m_header;
            QSet<QString>               m_activeRepos;

        public slots:
            void slotResult(QList<RepositoryListItem*> cl);

    };
}
#endif

