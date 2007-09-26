
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
#include "description_extractor.hh"
#include "name_extractor.hh"
#include "PackageItem.hh"
#include "pcre_matcher.hh"
#include "TaskBox.hh"
#include "text_matcher.hh"
#include "ThreadFetchItem.hh"
#include "VersionItem.hh"
#include <libwrapiter/libwrapiter_forward_iterator.hh>
#include <paludis/dep_spec.hh>
#include <paludis/environment.hh>
#include <paludis/package_database.hh>
#include <paludis/package_id.hh>
#include <paludis/query.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/sequence.hh>
#include <paludis/util/set.hh>
#include <paludis/util/stringify.hh>
#include <QDebug>
#include <QVariant>
#include <set>

pertubis::ThreadFetchItem::ThreadFetchItem(QObject* pobj,
                                           DatabaseView* main) : ThreadBase(pobj,main)
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
    std::list<tr1::shared_ptr<Matcher> > matchers;
    std::list<tr1::shared_ptr<Extractor> > extractors;
    qDebug() << "ThreadFetchItem::run() - query string:" << m_query;
    qDebug() << "ThreadFetchItem::run() - searching for name:" << m_optName << m_optDesc;
    qDebug() << "ThreadFetchItem::run() - searching for description: " << m_optDesc;
    matchers.push_back( tr1::shared_ptr<TextMatcher>(new TextMatcher(m_query.toLatin1().data()) ));

    if (m_optName )
        extractors.push_back(tr1::shared_ptr<NameExtractor>( new NameExtractor(m_main->getEnv().get()) )    );
    if (m_optDesc )
        extractors.push_back(tr1::shared_ptr<DescriptionExtractor>( new DescriptionExtractor(m_main->getEnv().get())));

    for (IndirectIterator<PackageDatabase::RepositoryConstIterator, const Repository>
            r(m_main->getEnv()->package_database()->begin_repositories()), r_end(m_main->getEnv()->package_database()->end_repositories()) ;
            r != r_end ; ++r)
    {
        if (r->format() == "vdb" || r->format() == "installed_virtuals" || r->format() == "virtuals")
            continue;
        tr1::shared_ptr<const CategoryNamePartSet> cat_names(r->category_names());
        for (CategoryNamePartSet::ConstIterator c(cat_names->begin()), c_end(cat_names->end()) ;
                c != c_end ; ++c)
        {
            tr1::shared_ptr<const QualifiedPackageNameSet> pkg_names(r->package_names(*c));
            for (QualifiedPackageNameSet::ConstIterator p(pkg_names->begin()), p_end(pkg_names->end());
                 p != p_end; ++p)
            {
                tr1::shared_ptr<const PackageIDSequence> version_ids(r->package_ids(*p));

                if (version_ids->empty())
                    continue;

                tr1::shared_ptr<const PackageID> display_entry(*version_ids->begin());
                for (PackageIDSequence::ConstIterator i(version_ids->begin()),
                        i_end(version_ids->end()) ; i != i_end ; ++i)
                    if (! (*i)->masked())
                        display_entry = *i;

                bool match(false);
                for (std::list<tr1::shared_ptr<Extractor> >::const_iterator x(extractors.begin()),
                        x_end(extractors.end()) ; x != x_end && ! match ; ++x)
                {
                    std::string xx((**x)(*display_entry));
                    for (std::list<tr1::shared_ptr<Matcher> >::const_iterator m(matchers.begin()),
                            m_end(matchers.end()) ; m != m_end && ! match ; ++m)
                        if ((**m)(xx))
                            match = true;
                }

                if (! match)
                    continue;

                PackageDepSpec dps(stringify(*p),pds_pm_eapi_0_strict);

                Item* p_item=0;
                QVariantList tasks(m_main->taskbox()->tasks());
                if (dps.version_requirements_ptr() || dps.slot_ptr() || dps.use_requirements_ptr() || dps.repository_ptr())
                {
                    p_item = new PackageItem(display_entry, QList<QVariant>() << QVariant(tasks) << stringify(*dps.package_name_part_ptr()).c_str() << stringify(*dps.category_name_part_ptr()).c_str() << stringify(*dps.repository_ptr()).c_str() << Qt::Unchecked);
                }
                else
                {
                    p_item = new PackageItem(display_entry,QList<QVariant>() << QVariant(tasks) << stringify(display_entry->name().package).c_str()<< stringify(display_entry->name().category).c_str() << stringify(display_entry->repository()->name()).c_str() << Qt::Unchecked) ;
                }

                tr1::shared_ptr<const PackageIDSequence> versionIds(r->package_ids(*p));
                int mp=0;
                int ip=0;
                for (PackageIDSequence::ConstIterator vstart(versionIds->begin()),vend(versionIds->end());
                    vstart != vend; ++vstart)
                {
                    Item* v_item = makeVersionItem(*vstart,
                            m_main->taskbox()->selectionData(*vstart),
                            stringify((*vstart)->version()).c_str());

                    if (! ( (*vstart)->begin_masks()  == (*vstart)->end_masks() ) )
                    {
                        v_item->setState(Item::is_masked);
                        ++mp;
                    }
                    else
                        p_item->setBestChild(v_item);

                    p_item->appendChild(v_item);
                }

                if ( ip > 0 )
                    p_item->setData(Item::io_installed,Qt::Checked);
                if (mp == p_item->childCount())
                    p_item->setState(Item::is_masked);
                emit itemResult(p_item);
            }
        }
    }
}
