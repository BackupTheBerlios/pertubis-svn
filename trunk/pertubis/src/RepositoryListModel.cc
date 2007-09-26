
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

#include "DatabaseView.hh"
#include "RepositoryListModel.hh"
#include <paludis/name.hh>
#include <paludis/repository.hh>
#include <libwrapiter/libwrapiter_forward_iterator.hh>
#include <paludis/package_database.hh>
#include <paludis/environment.hh>

#include <QSet>
#include <QString>
#include <QDebug>

void pertubis::RepositoryListThread::run()
{
    using namespace paludis;
    QList<QVariantList> list;
    QStringList names;
    for (IndirectIterator<PackageDatabase::RepositoryConstIterator, const Repository>
         r((*m_main->getEnv()).package_database()->begin_repositories()), r_end((*m_main->getEnv()).package_database()->end_repositories()) ;
         r != r_end ; ++r)
    {
        names << stringify(r->name()).c_str();
    }
    emit sendNames(names);
}

pertubis::RepositoryListModel::RepositoryListModel(QObject* pobj) : QAbstractListModel(pobj)
{
}

pertubis::RepositoryListModel::~RepositoryListModel()
{
}

void pertubis::RepositoryListModel::setHorizontalHeaderLabels ( const QStringList & labels )
{
    m_header = labels;
}

QVariant pertubis::RepositoryListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical || section >= m_header.count() )
        return QVariant();
    return *(m_header.begin() + section);
}

void pertubis::RepositoryListModel::slotResult(QStringList cl)
{
    QString cat;
    foreach (cat,cl)
    {
        m_data << RepositoryListItem(cat);
    }
    reset();
}

int pertubis::RepositoryListModel::rowCount( const QModelIndex & pobj ) const
{
    return pobj.isValid() ? 0 : m_data.count();
}

int pertubis::RepositoryListModel::columnCount( const QModelIndex & pobj ) const
{
    return pobj.isValid() ? 0 : m_header.count();
}

QVariant pertubis::RepositoryListModel::data ( const QModelIndex & m_index, int role) const
{
    if (!m_index.isValid())
         return QVariant();

     if (m_index.row() >= m_data.size())
         return QVariant();

     if (role == Qt::DisplayRole)
         return m_data.value(m_index.row()).name();
     else
         return QVariant();
}
