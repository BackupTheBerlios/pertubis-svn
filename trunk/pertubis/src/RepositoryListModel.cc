
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

#include "RepositoryListModel.hh"
#include <paludis/environment.hh>
#include <paludis/name.hh>
#include <paludis/package_database.hh>
#include <paludis/repository.hh>
#include <paludis/util/indirect_iterator.hh>
#include <paludis/util/indirect_iterator-impl.hh>
#include <paludis/util/wrapped_forward_iterator.hh>

#include <QSet>
#include <QString>
#include <QDebug>
#include <QColor>
#include <QBrush>

pertubis::RepositoryListItem::RepositoryListItem() : m_data(QList<QVariant>() << Qt::Checked << "")
{
}

pertubis::RepositoryListItem::RepositoryListItem(const paludis::RepositoryName & name) :
        m_data(QList<QVariant>() << Qt::Checked << QString::fromStdString(paludis::stringify(name)) )
{
}

bool pertubis::RepositoryListItem::setData(int col,const QVariant& value)
{
    if (m_data.count() > col)
    {
        m_data.replace(col,value);
        return true;
    }
    return false;
}

void pertubis::RepositoryListThread::run()
{
    using namespace paludis;
    ThreadBase::lock();
    QList<QVariantList> list;
    QList<RepositoryListItem*> names;
    for (IndirectIterator<PackageDatabase::RepositoryConstIterator, const Repository>
         r((*m_env).package_database()->begin_repositories()), r_end((*m_env).package_database()->end_repositories()) ;
         r != r_end ; ++r)
    {
        names << new RepositoryListItem(r->name());
    }
    emit sendNames(names);
    ThreadBase::unlock();
}

pertubis::RepositoryListModel::RepositoryListModel(QObject* pobj) : QAbstractListModel(pobj)
{
}

pertubis::RepositoryListModel::~RepositoryListModel()
{
    qDeleteAll(m_data);
    m_data.clear();
}

Qt::ItemFlags pertubis::RepositoryListModel::flags(const QModelIndex &mix) const
{
    switch (mix.column())
    {
        case 0:
            return Qt::ItemIsSelectable | Qt::ItemIsUserCheckable;
            break;
        default:
            return 0;
    }
    return 0;
}

bool pertubis::RepositoryListModel::setData ( const QModelIndex & ix, const QVariant & value, int /*role*/ )
{
    if (!ix.isValid() || ix.column() != 0)
        return false;
    m_data.value(ix.row())->setData(0,value);
    changeActiveRepos(m_data.value(ix.row())->data(1).toString());
    emit dataChanged(ix,ix);
    return true;
}

void pertubis::RepositoryListModel::changeActiveRepos(const QString& name)
{
    QSet<QString>::iterator it(m_activeRepos.find(name));
    if (it == m_activeRepos.end())
        m_activeRepos.insert(name);
    else
        m_activeRepos.erase(it);
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

void pertubis::RepositoryListModel::slotResult(QList<RepositoryListItem*> cl)
{
    m_activeRepos.clear();
    m_data = cl;
    RepositoryListItem* item;
    foreach (item ,cl)
    {
        m_activeRepos.insert(item->data(1).toString());
    }
    reset();
}

int pertubis::RepositoryListModel::rowCount( const QModelIndex & pobj ) const
{
    return pobj.isValid() ? 0 : m_data.count();
}

// int pertubis::RepositoryListModel::columnCount( const QModelIndex & pobj ) const
// {
//     return pobj.isValid() ? 0 : m_header.count();
// }

QVariant pertubis::RepositoryListModel::data ( const QModelIndex & m_index, int role) const
{
//     qDebug() << "RepositoryListModel::data()";
    if (!m_index.isValid())
         return QVariant();

    if (role == Qt::ForegroundRole)
    {
        return QBrush(QColor(0,0,0));
    }

    if (role == Qt::CheckStateRole && m_index.column() == 0)
    {
//         qDebug() << "RepositoryListModel::data() - 0" << m_data.value(m_index.row())->data(0);
        return m_data.value(m_index.row())->data(0);
    }

    if (role == Qt::DisplayRole && m_index.column() == 0)
    {
//         qDebug() << "RepositoryListModel::data() - 1" << m_data.value(m_index.row())->data(1);
        return m_data.value(m_index.row())->data(1);
    }

    return QVariant();
}

const QSet<QString>&  pertubis::RepositoryListModel::activeRepositories() const
{
    return m_activeRepos;
}
