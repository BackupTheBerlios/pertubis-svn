
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

#include "RepositoryInfoModel.hh"
#include "DatabaseView.hh"
#include <QHeaderView>
#include <QDebug>
#include <paludis/repository.hh>
#include <paludis/repository_info.hh>
#include <paludis/util/indirect_iterator.hh>
#include <paludis/util/indirect_iterator-impl.hh>
#include <paludis/package_database.hh>
#include <paludis/util/wrapped_forward_iterator.hh>
#include <paludis/environment.hh>

void pertubis::RepositoryInfoThread::getInfo(const QString& name)
{
    qDebug() << "RepositoryInfoThread::getInfo()" << name;
    m_repName = name;
    start();
}

pertubis::RepositoryInfoModel::RepositoryInfoModel(QObject* pobj) : QAbstractTableModel(pobj)
{

}

QVariant pertubis::RepositoryInfoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation == Qt::Vertical || section >= m_header.count() )
        return QVariant();
    return *(m_header.begin() + section);
}

void pertubis::RepositoryInfoThread ::run()
{
    using namespace paludis;
    QList<QVariantList> list;
    QStringList names;
    RepositoryName repoName(m_repName.toStdString());
    tr1::shared_ptr<Repository> repo = m_main->getEnv()->package_database()->fetch_repository(repoName);
    std::tr1::shared_ptr<const RepositoryInfo> info = repo->info(true);
    for (IndirectIterator<RepositoryInfo::SectionConstIterator>
            s(indirect_iterator(info->begin_sections())), s_end(indirect_iterator(info->end_sections())) ;
            s != s_end ; ++s)
    {
        for (RepositoryInfoSection::KeyValueConstIterator k(s->begin_kvs()), k_end(s->end_kvs()) ;
                k != k_end ; ++k)
        {
            list.push_back(QVariantList() << k->first.c_str() << k->second.c_str());
        }
    }
    emit sendResult(list);
}

void pertubis::RepositoryInfoModel::slotResult(QList<QVariantList> list)
{
    m_data = list;
    reset();
}

QVariant pertubis::RepositoryInfoModel::data ( const QModelIndex & ix, int role) const
{
    if (!ix.isValid())
        return QVariant();

    if (role == Qt::DisplayRole )
    {
        return m_data.value(ix.row()).value(ix.column());
    }

    return QVariant();
}

void pertubis::RepositoryInfoModel::setHorizontalHeaderLabels ( const QStringList & labels )
{
    m_header = labels;
}

int pertubis::RepositoryInfoModel::rowCount(const QModelIndex& /*pmi*/) const
{
    return m_data.count();
}

int pertubis::RepositoryInfoModel::columnCount ( const QModelIndex& /*parent*/ ) const
{
    return 2;
}
