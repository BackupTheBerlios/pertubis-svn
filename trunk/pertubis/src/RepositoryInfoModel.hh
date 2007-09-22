
/* Copyright (C) 2007 Stefan Koegl.
*
* This file is part of pertubis.
*
* pertubis is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* pertubis is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http:*www.gnu.org/licenses/>.
*/

#ifndef _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_INFO_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_INFO_MODEL_H

#include <QList>
#include <QPair>
#include <QStringList>
#include "ThreadBase.hh"
#include <QAbstractTableModel>
#include <paludis/util/tr1_memory.hh>

namespace paludis
{
    class Environment;
}

namespace pertubis
{

    /*! \brief not finished
     *
     */
    class RepositoryInfoItem
    {
        public:

            typedef QStringList Line;
            RepositoryInfoItem(QString name) : m_name(name) {}

            void add(QString key,QString value) { m_data.push_back(Line() << key << value );}
            QList<Line>          m_data;
            QString              m_name;
    };

    /*! \brief not finished
     *
     */
    class ThreadRepository : public ThreadBase
    {
        Q_OBJECT
        public:
            ThreadRepository(QObject* pobj,
                             DatabaseView* main) : ThreadBase(pobj,main) {}

            void run();

        signals:

            void sendResult(RepositoryInfoItem list);
    };

    /*! \brief not finished
    *
    */
    class RepositoryInfoModel : public QAbstractTableModel
    {
        Q_OBJECT

    public:

        RepositoryInfoModel(QObject* parent);

        int columnCount ( const QModelIndex & parent = QModelIndex() ) const;

        QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

        QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;

        void setHorizontalHeaderLabels ( const QStringList & labels );

    public slots:

        void slotResult(RepositoryInfoItem list);


    private:
        RepositoryInfoItem m_data;
        QStringList        m_header;
    };

}

#endif



