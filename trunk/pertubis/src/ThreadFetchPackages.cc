
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

// #include "ThreadKeywordManager.hh"
#include "TaskBox.hh"
#include <paludis/util/log.hh>
#include <paludis/util/set.hh>
#include <paludis/util/sequence.hh>

#include <paludis/environment.hh>

#include <paludis/package_database.hh>
#include <paludis/package_id.hh>
#include <paludis/dep_spec.hh>
#include <paludis/query.hh>
#include <QDebug>
#include <QVariant>
#include <paludis/mask.hh>
#include <paludis/metadata_key.hh>

#include <libwrapiter/libwrapiter_forward_iterator.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/stringify.hh>

#include "pcre_matcher.hh"
#include "PackageItem.hh"
#include "text_matcher.hh"
#include "description_extractor.hh"
#include "name_extractor.hh"

#include <set>
#include <map>

pertubis::ThreadFetchPackages::ThreadFetchPackages(QObject* pobject,
		paludis::tr1::shared_ptr<paludis::Environment> env,
		ThreadKeywordManager* keywords,
		TaskBox* box) : ThreadBase(pobject,env),
		m_keywords(keywords),
		m_box(box)
{
}

void pertubis::ThreadFetchPackages::searchPackages(QString str)
{
	qDebug() << "ThreadFetchPackages::searchPackages() - 1";
	m_query = str;
	qDebug() << "ThreadFetchPackages::searchPackages() - 2";
	start();
	qDebug() << "ThreadFetchPackages::searchPackages() - 3";
}

void pertubis::ThreadFetchPackages::run()
{
	using namespace paludis;
	qDebug() << "ThreadFetchPackages::run() - 1";
	QVariantMap p_data = m_box->tasks();
	qDebug() << "ThreadFetchPackages::run() - 2";
	Item* root = new Item(QList<QVariant>() <<
			p_data <<
			"" <<
			"" <<
			"" <<
			false,
			0,
	  		Item::it_category);
// 	qDebug() << "ThreadFetchPackages::run() - 3";

	for (IndirectIterator<PackageDatabase::RepositoryIterator, const Repository>
		r(m_env->package_database()->begin_repositories()), r_end(m_env->package_database()->end_repositories()) ;
		r != r_end ; ++r)
    {
// 		qDebug() << "ThreadFetchPackages::run() - 4";
		if (r->format() == "vdb" || r->format() == "installed_virtuals")
			continue;
        std::tr1::shared_ptr<const CategoryNamePartSet> cat_names(r->category_names());
		for (CategoryNamePartSet::Iterator c(cat_names->begin()), c_end(cat_names->end()) ;c != c_end ; ++c)
        {
			CategoryNamePart cat(m_query.toLatin1().data());

			if (cat != *c )
				continue;

			std::tr1::shared_ptr<const QualifiedPackageNameSet> pkg_names(r->package_names(*c));
			QualifiedPackageNameSet::Iterator p(pkg_names->begin()), p_end(pkg_names->end());
			while (	p != p_end)
			{
				tr1::shared_ptr<const PackageIDSequence> packIds(
            	r->package_ids(*p));

				Item* p_item = new Item(QList<QVariant>() <<
						p_data <<
						stringify(p->package).c_str() <<
						stringify(p->category).c_str() <<
						stringify(r->name()).c_str() <<
						Qt::Unchecked,
	  					root,
						Item::it_package);
				root->appendChild(p_item);
				int mp=0;
				int ip=0;
				PackageIDSequence::Iterator vstart(packIds->begin()),vend(packIds->end());
				while (vstart != vend )
				{
					tr1::shared_ptr<const PackageIDSequence> ci(
					m_env->package_database()->query(
						query::InstalledAtRoot(m_env->root()) &
						query::Matches(PackageDepSpec(
								tr1::shared_ptr<QualifiedPackageName>(new QualifiedPackageName( (*vstart)->name())),
								tr1::shared_ptr<CategoryNamePart>(),
								tr1::shared_ptr<PackageNamePart>(),
								tr1::shared_ptr<VersionRequirements>(make_equal_to_version_requirements( (*vstart)->version())),
								vr_and,
								tr1::shared_ptr<SlotName>(new SlotName((*vstart)->slot())))),
						qo_order_by_version));

					QVariantMap v_data = m_box->selectionData(*vstart);
					std::string mr_string;
					bool prefer_default = true;
					for (PackageID::MasksIterator mstart((*vstart)->begin_masks()), mend((*vstart)->end_masks()) ;
					mstart != mend ; ++mstart)
					{
						prefer_default = false;
						if (! mr_string.empty())
							mr_string.append(", ");

						mr_string.append((*mstart)->description());
					}
					Item* v_item = new Item(QList<QVariant>() <<
							v_data	<<
							stringify((*vstart)->version()).c_str() <<
							"" <<
							"" <<
							( ( !ci->empty() ) ? Qt::Checked : Qt::Unchecked ) <<
							mr_string.c_str(),
							p_item,
		  					Item::it_version,
							*vstart);
					if (!prefer_default)
					{
						v_item->setStatus(Item::is_masked);
						++mp;
					}
// 					else
// 					{
// 						std::tr1::shared_ptr< const  KeywordNameCollection > keys = r->version_metadata(*p,*version)->ebuild_interface->keywords();
// 						ver->setStatus(static_cast<Item::ItemStatus>(m_keywords->classifyKeywords(keys)));
// 					}
					if ( !ci->empty() )
						++ip;
					if (v_item->status() < p_item->status())
						p_item->setStatus(v_item->status());
					if (v_data[tr("install")].toBool())
						p_data[tr("install")] = true;
					if (v_data[tr("deinstall")].toBool())
						p_data[tr("deinstall")] = true;

					p_item->appendChild(v_item);
					++vstart;
				}
				if ( ip > 0 )
 					p_item->setData(Item::io_installed,Qt::Checked);
				if (mp == p_item->childCount())
					p_item->setStatus(Item::is_masked);
				++p;
			}
		}
	}
	emit packagesResult(root);
}
