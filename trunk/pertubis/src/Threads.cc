
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

#include "Threads.hh"

#include "KeywordManager.hh"
#include <paludis/util/log.hh>

#include <paludis/environments/environment_maker.hh>
#include <paludis/environment.hh>
#include <paludis/package_database.hh>
#include <paludis/dep_spec.hh>
#include <paludis/query.hh>
#include <QDebug>

#include <paludis/repositories/gentoo/ebuild.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/stringify.hh>


#include "pcre_matcher.hh"
#include "text_matcher.hh"
#include "description_extractor.hh"
#include "name_extractor.hh"

#include <set>

void pertubis::PackageThread::fetchCategories()
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

pertubis::PackageThread::PackageThread(QObject* parent) : QThread(parent)
{
	paludis::Log::get_instance()->set_log_level(paludis::ll_silent);
	paludis::Log::get_instance()->set_program_name("pertubis");
	m_env = paludis::EnvironmentMaker::get_instance()->make_from_spec("");
	m_keywords = new KeywordManager();
}

void pertubis::PackageThread::searchDetails(QString cn,QString pn,QString ver,QString rep,Item::ItemStatus status)
{
	m_txt1 = cn;
	m_txt2 = pn;
	m_txt3 = ver;
	m_txt4 = rep;
	m_status=status;
	m_state=fs_details;
	start();
}

void pertubis::PackageThread::searchItem(QString str,bool name,bool desc)
{
	m_txt1 = str;
	m_optName = name;
	m_optDesc = desc;
	m_state=fs_item;
 	start();
}

void pertubis::PackageThread::searchPackages(QString str)
{
	m_txt1 = str;
	m_state=fs_package;
	start();
}

void pertubis::PackageThread::searchCategories()
{
	m_state=fs_categories;
	start();
}

void pertubis::PackageThread::fetchItem()
{
	using namespace paludis;
	std::list<std::tr1::shared_ptr<Matcher> > matchers;
	std::list<std::tr1::shared_ptr<Extractor> > extractors;

	matchers.push_back( std::tr1::shared_ptr<TextMatcher>(new TextMatcher(m_txt1.toLatin1().data()) ));

	if (m_optName )
    	extractors.push_back(std::tr1::shared_ptr<NameExtractor>( new NameExtractor(m_env) )	);
	if (m_optDesc )
    	extractors.push_back(std::tr1::shared_ptr<DescriptionExtractor>( new DescriptionExtractor(m_env)));

	std::set<QualifiedPackageName> pkgs;

	RepositoryName ins("installed");
	std::tr1::shared_ptr< Repository > vdb;
	if (!m_env->package_database()->has_repository_named(ins))
	{
		qFatal("cannot find database of installed packages!");
	}

	vdb = m_env->package_database()->fetch_repository(ins);

	for (IndirectIterator<PackageDatabase::RepositoryIterator, const Repository>
			r((*m_env).package_database()->begin_repositories()), r_end((*m_env).package_database()->end_repositories()) ;
			r != r_end ; ++r)
	{
		if (r->format() == "vdb" || r->format() == "installed_virtuals")
			continue;
		std::tr1::shared_ptr<const CategoryNamePartCollection> cat_names(r->category_names());
		for (CategoryNamePartCollection::Iterator c(cat_names->begin()), c_end(cat_names->end()) ;
                c != c_end ; ++c)
		{
			std::tr1::shared_ptr<const QualifiedPackageNameCollection> pkg_names(r->package_names(*c));
			for (QualifiedPackageNameCollection::Iterator p(pkg_names->begin()), p_end(pkg_names->end()) ;p != p_end ; ++p)
			{
				std::tr1::shared_ptr<const PackageDatabaseEntryCollection>
					entries( (*m_env).package_database()->query(
								query::Package(*p), qo_order_by_version)),
					preferred_entries( (*m_env).package_database()->query(query::Package(*p) &
								query::InstalledAtRoot( (*m_env).root()), qo_order_by_version));

				if (entries->empty())
					continue;
				if (preferred_entries->empty())
					preferred_entries = entries;

				PackageDatabaseEntry display_entry(*preferred_entries->last());
				for (PackageDatabaseEntryCollection::Iterator i(preferred_entries->begin()),
						i_end(preferred_entries->end()) ; i != i_end ; ++i)
					if (! (*m_env).mask_reasons(*i).any())
						display_entry = *i;

				bool match(false);
				for (std::list<std::tr1::shared_ptr<Extractor> >::const_iterator x(extractors.begin()),
						x_end(extractors.end()) ; x != x_end && ! match ; ++x)
				{
					std::string xx((**x)(display_entry));
					for (std::list<std::tr1::shared_ptr<Matcher> >::const_iterator m(matchers.begin()),
							m_end(matchers.end()) ; m != m_end && ! match ; ++m)
						if ((**m)(xx))
							match = true;
				}

				if (! match)
					continue;

				PackageDepSpec dps(stringify(*p),pds_pm_eapi_0_strict);

				std::tr1::shared_ptr< const VersionSpecCollection > instVer = vdb->version_specs(*p);
				std::tr1::shared_ptr< const VersionSpecCollection > repVer = r->version_specs(*p);
				Item* pack=0;
				QVariantMap tasks;
				tasks.insert(tr("install"),false);
				tasks.insert(tr("deinstall"),false);
				if (dps.version_requirements_ptr() || dps.slot_ptr() || dps.use_requirements_ptr() || dps.repository_ptr())
				{
					pack = new Item(QList<QVariant>() << tasks << stringify(*dps.package_name_part_ptr()).c_str() << stringify(*dps.category_name_part_ptr()).c_str() << stringify(*dps.repository_ptr()).c_str() << Qt::Unchecked,0,Item::it_package);
				}
				else
				{
					pack = new Item(QList<QVariant>() << tasks << stringify(display_entry.name.package).c_str()<< stringify(display_entry.name.category).c_str() << stringify(r->name()).c_str() << Qt::Unchecked,0,Item::it_package) ;
				}
				VersionSpecCollection::Iterator version( repVer->begin() );

				while (version != repVer->end() )
				{
					Item* ver = new Item(QList<QVariant>() << tasks <<stringify(*version).c_str() << "" << "" << vdb->has_version(display_entry.name,*version),pack,Item::it_version);

					if ( m_env->mask_reasons(display_entry ).any() )
					{
						ver->m_status = Item::is_masked;
					}
					else
					{
						std::tr1::shared_ptr< const  KeywordNameCollection > keys = r->version_metadata(*p,*version)->ebuild_interface->keywords();
						ver->m_status = (Item::ItemStatus) m_keywords->classifyKeywords(keys);
					}
					pack->appendChild(ver);
					++version;
				}
				if ( repVer->size() == instVer->size() )
					pack->setData(Item::io_installed,Qt::Checked);
				else if (instVer->size() > 0)
					pack->setData(Item::io_installed,Qt::Checked);

				emit itemResult(pack);
			}
        }
	}
	emit searchItemFinished();
}

void pertubis::PackageThread::fetchPackages()
{
	using namespace paludis;
	QVariantMap tasks;
	tasks.insert(tr("install"),false);
	tasks.insert(tr("deinstall"),false);
	Item* root = new Item(QList<QVariant>() <<
			tasks <<
			"" <<
			"" <<
			"" <<
			false,
			0,
	  		Item::it_category);
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
			CategoryNamePart cat(m_txt1.toLatin1().data());

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
				while (version != repVer->end() )
				{
					PackageDatabaseEntry entry(*p,*version,r->name());
					Item* ver = new Item(QList<QVariant>() <<
							tasks <<
							stringify(*version).c_str() <<
							"" <<
							"" <<
							( vdb->has_version(*p,*version) ? Qt::Checked : Qt::Unchecked),
							pack,
		  					Item::it_version);
					if ( m_env->mask_reasons(PackageDatabaseEntry(*p,*version,r->name())).any() )
					{
						ver->m_status = Item::is_masked;
						++mp;
					}
					else
					{
						std::tr1::shared_ptr< const  KeywordNameCollection > keys = r->version_metadata(*p,*version)->ebuild_interface->keywords();
						ver->m_status = (Item::ItemStatus) m_keywords->classifyKeywords(keys);
					}

					pack->appendChild(ver);
					++version;
				}
				if ( repVer->size() == instVer->size() )
					pack->setData(Item::io_installed,Qt::Checked);
				else if (instVer->size() > 0)
					pack->setData(Item::io_installed,Qt::Checked);
				if (mp==pack->childCount())
					pack->m_status=Item::is_masked;
				++p;
			}
		}
	}
	emit packagesResult(root);
	emit searchPackagesFinished();
}

void pertubis::PackageThread::fetchDetails()
{
	qDebug() << "PackageDetails::fetchDetails() - start";
	using namespace paludis;
QString tpl(
"<html>"
"<body bgcolor=\"#aaaaaa\" text=\"black\">\n"
"	<table border=\"0\" summary=\"\" width=\"100%\" height=\"100%\" cellpadding=\"15\">\n"
"			<colgroup>\n"
"				<col width=\"50%\">\n"
"				<col width=\"50%\">\n"
"			</colgroup>\n"
"			<tr>\n"
"   			<th colspan=\"2\" align=\"left\">%1-%2 ( %3 ) </th>\n"
"			</tr>\n"
"			<tbody >\n"
"				<tr>\n"
"					<td>Description</td>\n"
"					<td>%4</td>\n"
"				</tr>\n"
"				<tr>\n"
"					<td>HomePage</td>\n"
"					<td><a href=\"%5\">%6</a></td>\n"
"				</tr>\n"
"				<tr>\n"
"					<td>Use Flags</td>\n"
"					<td>%7</td>\n"
"				</tr>\n"
"				<tr>\n"
"					<td>Keywords</td>\n"
"					<td>%8</td>\n"
"				</tr>\n"
"				<tr>\n"
"					<td>Provides</td>\n"
"					<td>%9</td>\n"
"				</tr>\n"
"				<tr>\n"
"					<td>License</td>\n"
"					<td>%10</td>\n"
"				</tr>\n"
"			</tbody>\n"
"		</table>\n"
" </body>\n"
"</html>\n");

	CategoryNamePart c(m_txt1.toLatin1().data());

	PackageNamePart p(m_txt2.toLatin1().data());

	RepositoryName n(m_txt4.toLatin1().data());
	QString output;
	if (m_env->package_database()->has_repository_named(n))
	{
		std::tr1::shared_ptr< Repository > rep = m_env->package_database()->fetch_repository(n);
		QualifiedPackageName qpn(c,p);

		std::tr1::shared_ptr< const VersionMetadata > data = rep->version_metadata(qpn, VersionSpec(m_txt3.toLatin1().data()) );
		output = tpl.arg(stringify(qpn).c_str() ).arg(m_txt3).arg(status( (Item::ItemStatus) m_status)).arg(data->description.c_str()).arg(data->homepage.c_str()).arg(data->homepage.c_str()).arg(stringify(data->ebuild_interface->iuse).c_str()).arg(data->ebuild_interface->keywords_string.c_str()).arg(data->ebuild_interface->provide_string.c_str()).arg("");
	}
	emit detailsResult(output);
}

void pertubis::PackageThread::run()
{
	switch (m_state)
	{
		case fs_item:
			fetchItem();
			break;
		case fs_package:
			fetchPackages();
			break;
		case fs_details:
			fetchDetails();
			break;
		case fs_categories:
			fetchCategories();
			break;
		default:
			break;
	}
}
