
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
#include "Item.hh"
#include "RegexMatcher.hh"
#include "TaskBox.hh"
#include "text_matcher.hh"
#include "SearchThread.hh"

#include <paludis/environment.hh>
#include <paludis/package_database.hh>
#include <paludis/package_id.hh>
#include <paludis/metadata_key.hh>
#include <paludis/query.hh>
#include <paludis/mask.hh>
#include <paludis/util/indirect_iterator.hh>
#include <paludis/util/indirect_iterator-impl.hh>
#include <paludis/util/sequence.hh>
#include <paludis/util/set.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/wrapped_forward_iterator.hh>
#include <QDebug>
#include <QVariant>
#include <set>

pertubis::SearchThread::SearchThread(QObject* pobj,
                                           DatabaseView* main) : ThreadBase(pobj,main)
{
}

void pertubis::SearchThread::start(const QString& str, bool name, bool desc)
{
    m_query = str;
    m_optName = name;
    m_optDesc = desc;
    QThread::start();
}

void pertubis::SearchThread::run()
{
    using namespace paludis;
    std::list<tr1::shared_ptr<Matcher> > matchers;
    std::list<tr1::shared_ptr<Extractor> > extractors;
    qDebug() << "SearchThread::run() - query string:" << m_query;
    qDebug() << "SearchThread::run() - searching for name:" << m_optName << m_optDesc;
    qDebug() << "SearchThread::run() - searching for description: " << m_optDesc;
    matchers.push_back( tr1::shared_ptr<TextMatcher>(new TextMatcher(m_query.toLatin1().data()) ));
    int count(0);
    if (m_optName )
        extractors.push_back(tr1::shared_ptr<NameExtractor>( new NameExtractor(m_main->getEnv().get()) )    );
    if (m_optDesc )
        extractors.push_back(tr1::shared_ptr<DescriptionExtractor>( new DescriptionExtractor(m_main->getEnv().get())));

    qDebug() << "1";
    for (IndirectIterator<PackageDatabase::RepositoryConstIterator, const Repository>
            r(m_main->getEnv()->package_database()->begin_repositories()), r_end(m_main->getEnv()->package_database()->end_repositories()) ;
            r != r_end ; ++r)
    {
//         qDebug() << "1 a" << stringify(r->format()).c_str();
        if (r->format_key()->value() != "ebuild")
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
                qDebug() << "4";
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


                qDebug() << "5";
                if (! match)
                    continue;
                qDebug() << "6";
                PackageDepSpec dps(stringify(*p),pds_pm_eapi_0_strict);
                qDebug() << "7";
                Item* p_item=0;
                QVariantList tasks(m_main->taskbox()->tasks());
                tr1::shared_ptr<const PackageIDSequence> versionIds(r->package_ids(*p));
                qDebug() << "8";
                if (dps.version_requirements_ptr() || dps.slot_ptr() || dps.use_requirements_ptr() || dps.repository_ptr())
                {
                    p_item = makePackageItem(display_entry,
                                            tasks,
                                            QString::fromStdString(stringify(*dps.package_name_part_ptr())),
                                            QString::fromStdString(stringify(*dps.category_name_part_ptr())),
                                            Qt::Unchecked,
                                            Item::is_stable,
                                            Item::ur_child,
                                            0,
                                            "");
                }
                else
                {
                    p_item = makePackageItem(*versionIds->last(),
                                            tasks,
                                            QString::fromStdString(stringify(display_entry->name().package)),
                                            QString::fromStdString(stringify(display_entry->name().category)) ,
                                            Qt::Unchecked,
                                            Item::is_stable,
                                            Item::ur_child,
                                            0,
                                            "");
                }

                qDebug() << "9";
                int mp=0;
                int ip=0;
                for (PackageIDSequence::ConstIterator vstart(versionIds->begin()),vend(versionIds->end());
                    vstart != vend; ++vstart)
                {
                    QString reasons;
                    for (paludis::PackageID::MasksConstIterator m((*vstart)->begin_masks()), m_end((*vstart)->end_masks()) ;
                         m != m_end ; ++m)
                    {
                        reasons.append(stringify((*m)->description()).c_str());
                    }
                    Item* v_item = makeVersionItem(*vstart,
                                                    m_main->taskbox()->tasks(),
                                                    QString::fromStdString(stringify((*vstart)->version())),
                                                    QString::fromStdString(stringify((*vstart)->repository()->name())),
                                                    ( installed(*vstart) ? Qt::Checked : Qt::Unchecked),
                                                    (reasons.isEmpty() ? Item::is_stable : Item::is_masked),
                                                    Item::ur_child,
                                                    p_item,
                                                    "");


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
                ++count;
            }
        }
    }
    qDebug() << "2";
    emit finished(count);
}
