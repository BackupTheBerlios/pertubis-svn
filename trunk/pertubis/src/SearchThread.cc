
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

#include "description_extractor.hh"
#include "name_extractor.hh"
#include "Item.hh"
#include "RegexMatcher.hh"
#include "PaludisUtils.hh"
#include "TaskBox.hh"
#include "text_matcher.hh"
#include "SearchThread.hh"

#include <paludis/action.hh>
#include <paludis/environment.hh>
#include <paludis/mask.hh>
#include <paludis/metadata_key.hh>
#include <paludis/package_database.hh>
#include <paludis/package_id.hh>
#include <paludis/query.hh>
#include <paludis/util/indirect_iterator.hh>
#include <paludis/util/make_shared_ptr.hh>
#include <paludis/util/indirect_iterator-impl.hh>
#include <paludis/util/sequence.hh>
#include <paludis/util/set.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/wrapped_forward_iterator.hh>
#include <QDebug>
#include <QVariant>
#include <QStringList>
#include <set>

struct Matches
{
    typedef bool result;

    const std::list<paludis::tr1::shared_ptr<pertubis::Matcher> > & matchers;
    const std::list<paludis::tr1::shared_ptr<pertubis::Extractor> > & extractors;

    Matches(
            const std::list<paludis::tr1::shared_ptr<pertubis::Matcher> > & m,
            const std::list<paludis::tr1::shared_ptr<pertubis::Extractor> > & e) :
            matchers(m),
            extractors(e)
    {
    }

    bool operator() (const paludis::PackageID & id) const
    {
        for (std::list<paludis::tr1::shared_ptr<pertubis::Extractor> >::const_iterator e(extractors.begin()), e_end(extractors.end()) ;
            e != e_end ; ++e)
            for (std::list<paludis::tr1::shared_ptr<pertubis::Matcher> >::const_iterator m(matchers.begin()), m_end(matchers.end()) ;
                m != m_end ; ++m)
                if ((**e)(**m, id))
                    return true;

        return false;
    }
};

paludis::tr1::shared_ptr<const paludis::PackageID> fetch_id(
    const paludis::Environment & env,
    const paludis::tr1::shared_ptr<const paludis::Repository> & r,
    const paludis::QualifiedPackageName & q,
    const paludis::tr1::function<bool (const paludis::PackageID &)> & m)
{
    paludis::tr1::shared_ptr<const paludis::PackageIDSequence> ids(r->package_ids(q));
    if (ids->empty())
        return paludis::tr1::shared_ptr<const paludis::PackageID>();
    else
    {
        std::list<paludis::tr1::shared_ptr<const paludis::PackageID> > sids(ids->begin(), ids->end());
        paludis::PackageIDComparator c(env.package_database().get());
        sids.sort(paludis::tr1::ref(c));

        for (std::list<paludis::tr1::shared_ptr<const paludis::PackageID> >::const_reverse_iterator i(sids.rbegin()), i_end(sids.rend()) ;
             i != i_end ; ++i)
            if (m(**i))
                return *i;

        return paludis::tr1::shared_ptr<const paludis::PackageID>();
    }
}

void set_id(
            const paludis::Environment & env,
            const std::list<paludis::tr1::shared_ptr<const paludis::Repository> > & repos,
            std::pair<const paludis::QualifiedPackageName, paludis::tr1::shared_ptr<const paludis::PackageID> > & q,
            const paludis::tr1::function<bool (const paludis::PackageID &)> & m)
{
    paludis::tr1::shared_ptr<const paludis::PackageID> best_id;
    for (std::list<paludis::tr1::shared_ptr<const paludis::Repository> >::const_iterator r(repos.begin()), r_end(repos.end()) ;
         r != r_end ; ++r)
    {
        paludis::tr1::shared_ptr<const paludis::PackageID> id(fetch_id(env, *r, q.first, m));
        if (id)
        {
            if (best_id)
            {
                paludis::PackageIDComparator c(env.package_database().get());
                if (c(best_id, id))
                    best_id = id;
            }
            else
                best_id = id;
        }
    }

    q.second = best_id;
}

pertubis::SearchThread::SearchThread(QObject* pobj,
                                     paludis::tr1::shared_ptr<paludis::Environment>  env,
                                     TaskBox* box) : ThreadBase(pobj,env,box)
{
}

void pertubis::SearchThread::start(const QString& str, bool name, bool desc)
{
    m_query = str;
    m_optName = name;
    m_optDesc = desc;
    m_stopExec = false;
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
        extractors.push_back(tr1::shared_ptr<NameExtractor>( new NameExtractor(m_env.get()) )    );
    if (m_optDesc )
        extractors.push_back(tr1::shared_ptr<DescriptionExtractor>( new DescriptionExtractor(m_env.get())));
    std::list<tr1::shared_ptr<const Repository> > repos;
    for (PackageDatabase::RepositoryConstIterator r(m_env->package_database()->begin_repositories()),
         r_end(m_env->package_database()->end_repositories()) ;
         r != r_end ; ++r)
    {
        if (! (*r)->some_ids_might_support_action(paludis::SupportsActionTest<paludis::InstallAction>()))
            continue;
        qDebug() << "take repository" << stringify((*r)->name()).c_str() << "into account";
        repos.push_back(*r);
    }

    if (m_stopExec)
        return;
    std::set<CategoryNamePart> cats;
    for (std::list<tr1::shared_ptr<const paludis::Repository> >::const_iterator r(repos.begin()), r_end(repos.end()) ;
         r != r_end ; ++r)
    {
        tr1::shared_ptr<const paludis::CategoryNamePartSet> c((*r)->category_names());
        std::copy(c->begin(), c->end(), std::inserter(cats, cats.begin()));
    }


    if (m_stopExec)
        return;
    std::map<QualifiedPackageName, tr1::shared_ptr<const PackageID> > ids;
    for (std::list<tr1::shared_ptr<const Repository> >::const_iterator r(repos.begin()), r_end(repos.end()) ;
         r != r_end ; ++r)
    {
        if (m_stopExec)
            return;
        for (std::set<CategoryNamePart>::const_iterator c(cats.begin()), c_end(cats.end()) ;
             c != c_end ; ++c)
        {
            tr1::shared_ptr<const QualifiedPackageNameSet> q((*r)->package_names(*c));
            for (QualifiedPackageNameSet::ConstIterator i(q->begin()), i_end(q->end()) ;
                i != i_end ; ++i)
            {
                ids.insert(std::make_pair(*i, tr1::shared_ptr<const PackageID>()));
            }
        }
    }


    if (m_stopExec)
        return;

    Matches matches(matchers,extractors);

    std::for_each(ids.begin(), ids.end(), tr1::bind(&set_id, tr1::cref(*m_env), tr1::cref(repos), tr1::placeholders::_1, matches));


    if (m_stopExec)
        return;

    for (std::map<QualifiedPackageName, tr1::shared_ptr<const PackageID> >::const_iterator
         i(ids.begin()), i_end(ids.end()) ; i != i_end ; ++i)
    {
        if (! i->second)
            continue;
        if (m_stopExec)
            return;

        const tr1::shared_ptr< const PackageIDSequence > versionIds(
                m_env->package_database()->query(
                query::Matches(PackageDepSpec(make_shared_ptr(new QualifiedPackageName(i->first)))) &
                query::SupportsAction<InstallAction>(),
                qo_order_by_version));
        if (versionIds->empty())
            continue;

        QVariantList tasks(m_taskbox->tasks());
        Item* p_item = makePackageItem(*versionIds->last(),
                                tasks,
                                QString::fromStdString(stringify(i->first.package)),
                                QString::fromStdString(stringify(i->first.category)),
                                Qt::Unchecked,
                                Item::is_stable,
                                0,
                                "");
        QSet<QString> pReasons;
        int mp=0;
        int ip=0;

        Qt::CheckState piState(Qt::Unchecked);
        for (PackageIDSequence::ReverseConstIterator vstart(versionIds->rbegin()),vend(versionIds->rend());
            vstart != vend; ++vstart)
        {
            QSet<QString> vReasons;
            for (paludis::PackageID::MasksConstIterator m((*vstart)->begin_masks()), m_end((*vstart)->end_masks()) ;
                    m != m_end ; ++m)
            {
                vReasons.insert(QString::fromStdString(stringify((*m)->description())));
            }
            QStringList tmp(vReasons.toList());
            Qt::CheckState iState(installed(m_env,*vstart) ? Qt::Checked : Qt::Unchecked);
            Item* v_item = makeVersionItem(*vstart,
                                            tasks,
                                            QString::fromStdString(stringify((*vstart)->version())),
                                            QString::fromStdString(stringify((*vstart)->repository()->name())),
                                            iState,
                                            (tmp.isEmpty() ? Item::is_stable : Item::is_masked),
                                            p_item,
                                            tmp.join(", "));
            pReasons.unite(vReasons);
            if (iState == Qt::Checked)
            {
                piState = Qt::Checked;
            }
            if (v_item->state() == Item::is_masked)
            {
                ++mp;
            }
            else if (p_item->bestChild() == 0)
                p_item->setBestChild(v_item);

            p_item->prependChild(v_item);
        }
        QStringList ptmp(pReasons.toList());
        if (piState != Qt::Unchecked &&
            hasVersionChange(m_env,p_item->ID()))
        {
            p_item->setData(Item::io_change,tr("version change"));
            emit changeInCat(QString::fromStdString(stringify(p_item->ID()->name().category)));
        }
        p_item->setData(Item::io_mask_reasons,ptmp.join(", "));

        if ( ip > 0 )
            p_item->setData(Item::io_installed,Qt::Checked);
        if (mp == p_item->childCount())
            p_item->setItemState(Item::is_masked);
        emit itemResult(p_item);
        p_item->setData(Item::io_installed,piState);
        ++count;
    }

    emit finished(count);
}
