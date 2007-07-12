
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

#include "ThreadFetchPackages.hh"

#include "ThreadKeywordManager.hh"
#include "TaskBox.hh"
#include <paludis/util/log.hh>

#include <paludis/environment.hh>

#include <paludis/package_database.hh>
#include <paludis/dep_spec.hh>
#include <paludis/query.hh>
#include <QDebug>
#include <QVariant>

#include <paludis/repositories/gentoo/ebuild.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/stringify.hh>


#include "pcre_matcher.hh"
#include "PackageItem.hh"
#include "text_matcher.hh"
#include "description_extractor.hh"
#include "name_extractor.hh"

#include <set>

pertubis::ThreadFetchPackages::ThreadFetchPackages(QObject* parent,
		std::tr1::shared_ptr<paludis::Environment> env,
		ThreadKeywordManager* keywords,
		TaskBox* box) : ThreadBase(parent,env),
		m_keywords(keywords),
		m_box(box)
{
}

void pertubis::ThreadFetchPackages::searchPackages(QString str)
{
	m_query = str;
	start();
}

void pertubis::ThreadFetchPackages::run()
{
	using namespace paludis;
	qDebug() << "------- 1";
	QVariantMap tasks = m_box->tasks();
	qDebug() << "------- 2";
	Item* root = new Item(QList<QVariant>() <<
			tasks <<
			"" <<
			"" <<
			"" <<
			false,
			0,
	  		Item::it_category);
	qDebug() << "------- 3";
	RepositoryName ins("installed");
	std::tr1::shared_ptr< Repository > vdb;
	if (m_env->package_database()->has_repository_named(ins))
	{
		vdb = m_env->package_database()->fetch_repository(ins);
	}

    for (IndirectIterator<PackageDatabase::RepositoryIterator, const Repository> r(m_env->package_database()->begin_repositories()), r_end(m_env->package_database()->end_repositories()) ;r != r_end ; ++r)
    {
		if (r->format() == "vdb" || r->format() == "installed_virtuals")
			continue;
        std::tr1::shared_ptr<const CategoryNamePartCollection> cat_names(r->category_names());
		for (CategoryNamePartCollection::Iterator c(cat_names->begin()), c_end(cat_names->end()) ;c != c_end ; ++c)
        {
			CategoryNamePart cat(m_query.toLatin1().data());

			if (cat != *c )
				continue;

			std::tr1::shared_ptr<const QualifiedPackageNameCollection> pkg_names(r->package_names(*c));
			QualifiedPackageNameCollection::Iterator p(pkg_names->begin()), p_end(pkg_names->end());
			while (	p != p_end)
			{
				std::tr1::shared_ptr< const VersionSpecCollection > instVer = vdb->version_specs(*p);
				std::tr1::shared_ptr< const VersionSpecCollection > repVer = r->version_specs(*p);
				VersionSpecCollection::Iterator version( (*repVer).begin() );

				Item* pack = new Item(QList<QVariant>() <<
						tasks <<
						stringify(p->package).c_str() <<
						stringify(p->category).c_str() <<
						stringify(r->name()).c_str() <<
						Qt::Unchecked,
	  					root,
						Item::it_package);
				root->appendChild(pack);
				int mp=0;
				QVariantMap data=tasks;
				while (version != repVer->end() )
				{
					Entry entry(stringify(*p).c_str(),stringify(*version).c_str(),stringify(r->name()).c_str(),stringify(r->name()).c_str());
					QVariantMap optionsData = m_box->selectionData(entry);
					Item* ver = new Item(QList<QVariant>() <<
							optionsData	<<
							stringify(*version).c_str() <<
							"" <<
							"" <<
							( vdb->has_version(*p,*version) ? Qt::Checked : Qt::Unchecked),
							pack,
		  					Item::it_version);
					if ( m_env->mask_reasons(PackageDatabaseEntry(*p,*version,r->name())).any() )
					{
						ver->setStatus(Item::is_masked);
						++mp;
					}
					else
					{
						std::tr1::shared_ptr< const  KeywordNameCollection > keys = r->version_metadata(*p,*version)->ebuild_interface->keywords();
						ver->setStatus(static_cast<Item::ItemStatus>(m_keywords->classifyKeywords(keys)));
					}
					if (ver->status() < pack->status())
						pack->setStatus(ver->status());
					if (optionsData[tr("install")].toBool())
						data[tr("install")] = true;
					if (optionsData[tr("deinstall")].toBool())
						data[tr("deinstall")] = true;

					pack->appendChild(ver);
					++version;
				}
				if ( repVer->size() == instVer->size() )
					pack->setData(Item::io_installed,Qt::Checked);
				else if (instVer->size() > 0)
					pack->setData(Item::io_installed,Qt::Checked);
				if (mp==pack->childCount())
					pack->setStatus(Item::is_masked);
				++p;
			}
		}
	}
	emit packagesResult(root);
}
