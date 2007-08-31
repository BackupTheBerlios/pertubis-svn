
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

#include "ThreadFetchItem.hh"
#include "PackageItem.hh"
#include "TaskBox.hh"

#include <paludis/util/log.hh>

#include <paludis/environments/environment_maker.hh>
#include <paludis/environment.hh>
#include <paludis/package_database.hh>
#include <paludis/dep_spec.hh>
#include <paludis/query.hh>
#include <QDebug>
#include <QVariant>

// #include <paludis/repositories/gentoo/ebuild.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/stringify.hh>

#include "pcre_matcher.hh"
// #include "ThreadKeywordManager.hh"
#include "text_matcher.hh"
#include "description_extractor.hh"
#include "name_extractor.hh"

#include <set>

pertubis::ThreadFetchItem::ThreadFetchItem(QObject* pobj,
		paludis::tr1::shared_ptr<paludis::Environment> env,
		ThreadKeywordManager* keywords,
		TaskBox* box) :
		ThreadBase(pobj,env),
		m_keywords(keywords),
		m_box(box)
{
}

void pertubis::ThreadFetchItem::search(QString str,bool name,bool desc)
{
	m_query = str;
	m_optName = name;
	m_optDesc = desc;
 	start();
}

void pertubis::ThreadFetchItem::run()
{
	using namespace paludis;
	std::list<std::tr1::shared_ptr<Matcher> > matchers;
	std::list<std::tr1::shared_ptr<Extractor> > extractors;
	qDebug() << "ThreadFetchItem::run() - query string:" << m_query;
	qDebug() << "ThreadFetchItem::run() - searching for name:" << m_optName << m_optDesc;
	qDebug() << "ThreadFetchItem::run() - searching for description: " << m_optDesc;
	matchers.push_back( std::tr1::shared_ptr<TextMatcher>(new TextMatcher(m_query.toLatin1().data()) ));

	if (m_optName )
    	extractors.push_back(std::tr1::shared_ptr<NameExtractor>( new NameExtractor(m_env.get()) )	);
	if (m_optDesc )
    	extractors.push_back(std::tr1::shared_ptr<DescriptionExtractor>( new DescriptionExtractor(m_env.get())));

	std::set<QualifiedPackageName> pkgs;

// 	RepositoryName ins("installed");
// 	std::tr1::shared_ptr< Repository > vdb;
// 	if (!m_env->package_database()->has_repository_named(ins))
// 	{
// 		qFatal("cannot find database of installed packages!");
// 	}

// 	QVariantMap tasks = m_box->tasks();
// 	vdb = m_env->package_database()->fetch_repository(ins);

// 	for (IndirectIterator<PackageDatabase::RepositoryIterator, const Repository>
// 			r((*m_env).package_database()->begin_repositories()), r_end((*m_env).package_database()->end_repositories()) ;
// 			r != r_end ; ++r)
// 	{
// 		qDebug() << "ThreadFetchItem::run() - repo name:" << stringify(r->name()).c_str();
// 		qDebug() << "ThreadFetchItem::run() - repo format:" << stringify(r->format()).c_str() << "\n";
// 		if (r->format() == "vdb" || r->format() == "installed_virtuals" || r->format() == "virtuals")
// 			continue;
// 		std::tr1::shared_ptr<const CategoryNamePartCollection> cat_names(r->category_names());
// 		for (CategoryNamePartCollection::Iterator c(cat_names->begin()), c_end(cat_names->end()) ;
//                 c != c_end ; ++c)
// 		{
// 			std::tr1::shared_ptr<const QualifiedPackageNameCollection> pkg_names(r->package_names(*c));
// 			for (QualifiedPackageNameCollection::Iterator p(pkg_names->begin()), p_end(pkg_names->end()) ;p != p_end ; ++p)
// 			{
// 				std::tr1::shared_ptr<const PackageDatabaseEntryCollection>
// 					entries( (*m_env).package_database()->query(
// 								query::Package(*p), qo_order_by_version)),
// 					preferred_entries( (*m_env).package_database()->query(query::Package(*p) &
// 								query::InstalledAtRoot( (*m_env).root()), qo_order_by_version));
//
// 				if (entries->empty())
// 					continue;
// 				if (preferred_entries->empty())
// 					preferred_entries = entries;
//
// 				PackageDatabaseEntry display_entry(*preferred_entries->last());
// 				for (PackageDatabaseEntryCollection::Iterator i(preferred_entries->begin()),
// 						i_end(preferred_entries->end()) ; i != i_end ; ++i)
// 					if (! (*m_env).mask_reasons(*i).any())
// 						display_entry = *i;
//
// 				bool match(false);
// 				for (std::list<std::tr1::shared_ptr<Extractor> >::const_iterator x(extractors.begin()),
// 						x_end(extractors.end()) ; x != x_end && ! match ; ++x)
// 				{
// 					std::string xx((**x)(display_entry));
// 					for (std::list<std::tr1::shared_ptr<Matcher> >::const_iterator m(matchers.begin()),
// 							m_end(matchers.end()) ; m != m_end && ! match ; ++m)
// 						if ((**m)(xx))
// 							match = true;
// 				}
//
//
// 				if (! match)
// 					continue;
//
// 				PackageDepSpec dps(stringify(*p),pds_pm_eapi_0_strict);
//
// 				std::tr1::shared_ptr< const VersionSpecCollection > instVer = vdb->version_specs(*p);
// 				std::tr1::shared_ptr< const VersionSpecCollection > repVer = r->version_specs(*p);
// 				Item* pack=0;
//
//
// 				if (dps.version_requirements_ptr() || dps.slot_ptr() || dps.use_requirements_ptr() || dps.repository_ptr())
// 				{
// 					pack = new Item(QList<QVariant>() << tasks << stringify(*dps.package_name_part_ptr()).c_str() << stringify(*dps.category_name_part_ptr()).c_str() << stringify(*dps.repository_ptr()).c_str() << Qt::Unchecked,0,Item::it_package);
// 				}
// 				else
// 				{
// 					pack = new Item(QList<QVariant>() << tasks << stringify(display_entry.name.package).c_str()<< stringify(display_entry.name.category).c_str() << stringify(r->name()).c_str() << Qt::Unchecked,0,Item::it_package) ;
// 				}
//
// 				VersionSpecCollection::Iterator version( repVer->begin() );
//
// 				while (version != repVer->end() )
// 				{
// 					Item* ver = new Item(QList<QVariant>() << tasks <<stringify(*version).c_str() << "" << "" << vdb->has_version(display_entry.name,*version),pack,Item::it_version);
//
// 					if ( m_env->mask_reasons(display_entry ).any() )
// 					{
// 						ver->setStatus(Item::is_masked);
// 					}
// 					else
// 					{
// 						std::tr1::shared_ptr< const  KeywordNameCollection > keys = r->version_metadata(*p,*version)->ebuild_interface->keywords();
// // 						ver->setStatus(static_cast<Item::ItemStatus>(m_keywords->classifyKeywords(keys)));
// 					}
//
// 					pack->appendChild(ver);
// 					++version;
// 				}
//
//
// 				if ( repVer->size() == instVer->size() )
// 					pack->setData(Item::io_installed,Qt::Checked);
// 				else if (instVer->size() > 0)
// 					pack->setData(Item::io_installed,Qt::Checked);
//
// 				emit itemResult(pack);
// 			}
//         }
// 	}
}
