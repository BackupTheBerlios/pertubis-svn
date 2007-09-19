
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

#include "TaskBox.hh"
#include "Task.hh"

#include <paludis/util/set.hh>
#include <paludis/util/sequence.hh>
#include <paludis/package_id.hh>
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
#include "VersionItem.hh"
#include "text_matcher.hh"
#include "description_extractor.hh"
#include "name_extractor.hh"
#include "DatabaseView.hh"

#include <map>

pertubis::ThreadFetchPackages::ThreadFetchPackages( QObject* pobject,
                                                    DatabaseView* main) : ThreadBase(pobject,main)
{
}

pertubis::ThreadFetchPackages::~ThreadFetchPackages()
{
}

void pertubis::ThreadFetchPackages::searchPackages(QString str)
{
    m_query = str;
    start();
}

void pertubis::ThreadFetchPackages::makePackageItems() const
{
    QVector<Task*>::const_iterator tStart(m_main->taskbox()->taskBegin());
    QVector<Task*>::const_iterator tEnd(m_main->taskbox()->taskEnd());
    while (tStart != tEnd)
    {
        Item* root = new RootItem();
        std::set<const paludis::PackageID*>::const_iterator idStart((*tStart)->idsBegin());
        std::set<const paludis::PackageID*>::const_iterator idEnd((*tStart)->idsEnd());
        while (idStart != idEnd)
        {
            root->appendChild(new PackageItem(QVariantList() <<
                    QVariant(m_main->taskbox()->selectionData(*idStart)) <<
                paludis::stringify((*idStart)->name().package).c_str() <<
                paludis::stringify((*idStart)->name().category).c_str() <<
                paludis::stringify((*idStart)->repository()->name()).c_str() <<
                paludis::stringify((*idStart)->version()).c_str()));
            ++idStart;
        }
    }
}

void pertubis::ThreadFetchPackages::run()
{
    using namespace paludis;

    Item* root = new RootItem();

    for (IndirectIterator<PackageDatabase::RepositoryIterator, const Repository>
        r(m_main->getEnv()->package_database()->begin_repositories()), r_end(m_main->getEnv()->package_database()->end_repositories()) ;
        r != r_end ; ++r)
    {
        if (r->format() == "vdb" || r->format() == "installed_virtuals")
            continue;
        std::tr1::shared_ptr<const CategoryNamePartSet> cat_names(r->category_names());

        for (CategoryNamePartSet::Iterator c(cat_names->begin()), c_end(cat_names->end()) ;c != c_end ; ++c)
        {
            CategoryNamePart cat(m_query.toLatin1().data());

            if (cat != *c )
                continue;

            paludis::tr1::shared_ptr<const QualifiedPackageNameSet> pkg_names(r->package_names(*c));
            QualifiedPackageNameSet::Iterator p(pkg_names->begin()), p_end(pkg_names->end());
            while (    p != p_end)
            {
                QList<QVariant> data;
                data << QVariant(m_main->taskbox()->tasks()) <<
                    stringify(p->package).c_str() <<
                    stringify(p->category).c_str() <<
                    stringify(r->name()).c_str() <<
                    Qt::Unchecked;
                Item* p_item = new PackageItem(data);
                root->appendChild(p_item);

                int mp=0;
                int ip=0;

                tr1::shared_ptr<const PackageIDSequence> versionIds(r->package_ids(*p));
                PackageIDSequence::Iterator vstart(versionIds->begin()),vend(versionIds->end());
                while (vstart != vend )
                {
                    QList<QVariant> vdata;
                    QVariantList vtasks(m_main->taskbox()->selectionData(vstart->get()));
                    vdata << QVariant() <<
                    stringify((*vstart)->version()).c_str() <<
                    stringify(p->category).c_str() <<
                    stringify(r->name()).c_str() <<
                    (m_main->taskbox()->task(m_main->tidInstall())->hasEntry(vstart->get()) ? Qt::Checked : Qt::Unchecked );
                    Item* v_item = new VersionItem(*vstart,vdata);

                    if (! ( (*vstart)->begin_masks()  == (*vstart)->end_masks() ) )
                    {
                        v_item->setState(Item::is_masked);
                        ++mp;
                    }
                    else
                        p_item->setBestChild(v_item);

                    p_item->appendChild(v_item);
                    ++vstart;
                }

                if ( ip > 0 )
                    p_item->setData(Item::io_installed,Qt::Checked);
                if (mp == p_item->childCount())
                    p_item->setState(Item::is_masked);
                ++p;
            }
        }
    }

    emit packagesResult(root);
}
