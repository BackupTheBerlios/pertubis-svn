
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

#include "CategoryModel.hh"


#include <QSet>
#include <QString>

#include <paludis/environment.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/stringify.hh>
#include <paludis/package_database.hh>

void pertubis::CatFetch::run()
{
	using namespace paludis;
	QSet<QString> cats;
	for (IndirectIterator<PackageDatabase::RepositoryIterator, const Repository> r(m_env->package_database()->begin_repositories()), r_end(m_env->package_database()->end_repositories()) ;r != r_end ; ++r)
	{
		std::tr1::shared_ptr<const CategoryNamePartCollection> cat_names(r->category_names());
		for (CategoryNamePartCollection::Iterator c(cat_names->begin()), c_end(cat_names->end()); c != c_end ; ++c)
		{
			cats << stringify(*c).c_str();
		}
	}
	emit newFetchResult(cats.toList());
}

pertubis::CategoryModel::CategoryModel(QObject* parent,std::tr1::shared_ptr<paludis::Environment> env) : QAbstractListModel(parent),m_env(env)
{
	m_thread = new CatFetch(m_env,this);
	connect(m_thread,
			SIGNAL(newFetchResult(QStringList)),
			this,
    		SLOT(slotPopulateModel(QStringList)));
	refreshModel();
}

pertubis::CategoryModel::~CategoryModel()
{
	m_thread->terminate();
	m_thread->wait();
}

void pertubis::CategoryModel::refreshModel()
{
	if (!m_thread->isRunning())
		m_thread->start();
}

void pertubis::CategoryModel::setHorizontalHeaderLabels ( const QStringList & labels )
{
	m_header = labels;
}

QVariant pertubis::CategoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole || orientation == Qt::Vertical || section >= m_header.count() )
		return QVariant();
	return *(m_header.begin() + section);
}

void pertubis::CategoryModel::slotPopulateModel(QStringList cl)
{
	QString cat;
	foreach (cat,cl)
	{
		m_data << cat;
	}
	m_data.sort();
	reset();
}

int pertubis::CategoryModel::rowCount( const QModelIndex & parent ) const
{
	return parent.isValid() ? 0 : m_data.count();
}

int pertubis::CategoryModel::columnCount( const QModelIndex & parent ) const
{
	return parent.isValid() ? 0 : m_header.count();
}

QVariant pertubis::CategoryModel::data ( const QModelIndex & index, int role) const
{
	if (!index.isValid())
         return QVariant();

     if (index.row() >= m_data.size())
         return QVariant();

     if (role == Qt::DisplayRole)
         return m_data.at(index.row());
     else
         return QVariant();
}
