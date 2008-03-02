
/* Copyright (C) 2007-2008 Stefan Koegl
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_LIST_MODEL_H
#define _PERTUBIS_ENTRY_PROTECTOR_REPOSITORY_LIST_MODEL_H 1

#include <QAbstractTableModel>
#include <paludis/util/tr1_memory.hh>
#include <paludis/name-fwd.hh>
#include "ThreadBase.hh"
#include <QStringList>
#include <QList>
#include <QSet>
#include <QVariant>

/*! \file
 * declares a few classes to describe repository lists
 */

namespace pertubis
{
    enum RepositoryListModelHeaderOrder
    {
        rlmho_name=0,
        rlmho_on=1
    };

     /*! \brief Represents a repository
     *
     * \ingroup ItemModelClass
     */
    class RepositoryListItem
    {
        public:

            ///@name Constructors
            ///@{

            /// standart constructor creates empty object
            RepositoryListItem();
            /// creates an object with data
            RepositoryListItem(const QString& name, Qt::CheckState checked=Qt::Checked);
            ///@}

            ///@name Content information
            ///@{ returns data from column col
            QVariant data(int col) const { return m_data.value(col);}
            ///@}

            ///@name Content modification
            ///@{
            /// sets displayable data "value" at column "col"
            bool setData(int col,const QVariant& value);
            ///@}
        private:
            /// the data to are to be rendered (shown)
            QVariantList     m_data;
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

            /// std constructor
            RepositoryListThread(QObject* pobj,
                paludis::tr1::shared_ptr<paludis::Environment>  myenv) :
                ThreadBase(pobj,myenv)
            {
            }

            /// overloaded from QThread
            void run();

        signals:

            /// sends a list with all repositories found
            void sendNames(QStringList);
    };

    /*! \brief This qt model class holds repositories.
     *
     * \ingroup ItemModelClass
     */
    class RepositoryListModel : public QAbstractTableModel
    {
        Q_OBJECT

        public:

            /// defines a const iterator type for stored repositories
            typedef QList<RepositoryListItem*>::const_iterator RepositoryConstIterator;

            ///@name Constructors
            ///@{

            /// creates a repository container class with an QObject parent object
            RepositoryListModel( QObject* parent, bool firstrun);
            ///@}

            /// destructor
            ~RepositoryListModel();

            ///@name Content modification
            ///@{

            /// sets a list with header column QStrings
            void setHorizontalHeaderLabels ( const QStringList & labels );

            /// adds or deletes a repository name of m_activeRepos
            void changeActiveRepos(const QString& name);

            /// sets displayable column data at QModelIndex index
            bool setData ( const QModelIndex & index, const QVariant & value, int role = Qt::EditRole );

            void clear();
            ///@}

            ///@name Content information
            ///@{

            /// returns the column data of the header
            QVariant headerData(int section, Qt::Orientation orientation, int role) const;

            /// returns the set of active repositories
            const QSet<QString> activeRepositories() const;

            /// returns the display properties for the given QModelIndex
            Qt::ItemFlags flags(const QModelIndex &mix) const;

            /// returns the number of childs for the given QModelIndex
            int columnCount( const QModelIndex & index ) const;

            /// returns the number of childs for the given QModelIndex
            int rowCount( const QModelIndex & index ) const;


            /// returns the data at QModelIndex index associated with the Qt::DisplayRole role
            QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole) const;
            ///@}

            /// saves user relevant data
            void loadSettings();

            /// loads user relevant data
            void saveSettings();

        signals:

            /// sends a list with all repositories found
            void sendNames(QStringList);

        public slots:

            /// sends
            void slotResult(QStringList);

        private:

            /// Repository storage
            QList<RepositoryListItem*>  m_data;

            /// header column storage
            QStringList                 m_header;

            /// a set of selected repositories ( could be used as whitelist for filtering)
            QSet<QString>               m_activeRepos;

            bool                        m_firstrun;
    };
}

#endif
