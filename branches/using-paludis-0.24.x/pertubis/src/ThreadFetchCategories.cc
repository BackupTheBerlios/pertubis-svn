
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

#include "ThreadFetchCategories.hh"

#include <paludis/environment.hh>
#include <paludis/package_database.hh>
#include <paludis/dep_spec.hh>

#include <QDebug>

#include <paludis/util/iterator.hh>
#include <paludis/util/stringify.hh>

#include "name_extractor.hh"

#include <QSet>

pertubis::ThreadFetchCategories::ThreadFetchCategories(QObject* parent, std::tr1::shared_ptr<paludis::Environment> env) : ThreadBase(parent,env)
{
}

void pertubis::ThreadFetchCategories::run()
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
	emit categoriesResult(cats.toList());
}
