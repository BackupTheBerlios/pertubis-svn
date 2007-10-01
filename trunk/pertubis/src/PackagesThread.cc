
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


#include "PackagesThread.hh"
#include "DatabaseView.hh"
#include "RepositoryListModel.hh"
#include "Item.hh"
#include "TaskBox.hh"
#include "Task.hh"

#include <libwrapiter/libwrapiter_forward_iterator.hh>
#include <paludis/environment.hh>
#include <paludis/mask.hh>
#include <paludis/package_database.hh>

#include <paludis/package_id.hh>
#include <paludis/query.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/sequence.hh>
#include <paludis/util/set.hh>
#include <paludis/util/stringify.hh>

pertubis::PackagesThread::PackagesThread( QObject* pobject,
                                                    DatabaseView* main) : ThreadBase(pobject,main)
{
}

pertubis::PackagesThread::~PackagesThread()
{
}

void pertubis::PackagesThread::searchPackages(QString str)
{
    m_query = str;
    start();
}


void pertubis::PackagesThread::run()
{
    using namespace paludis;

    Item* root = new Item();
    for (IndirectIterator<PackageDatabase::RepositoryConstIterator, const Repository>
        r(m_main->getEnv()->package_database()->begin_repositories()), r_end(m_main->getEnv()->package_database()->end_repositories()) ;
        r != r_end ; ++r)
    {
        if (r->format() == "vdb" ||
            r->format() == "installed_virtuals")
            continue;
        std::tr1::shared_ptr<const CategoryNamePartSet> cat_names(r->category_names());

        for (CategoryNamePartSet::ConstIterator c(cat_names->begin()), c_end(cat_names->end()) ;c != c_end ; ++c)
        {
            CategoryNamePart cat(m_query.toLatin1().data());

            if (cat != *c )
                continue;

            paludis::tr1::shared_ptr<const QualifiedPackageNameSet> pkg_names(r->package_names(*c));
            for (QualifiedPackageNameSet::ConstIterator p(pkg_names->begin()), p_end(pkg_names->end());
                 p != p_end;
                 ++p)
            {
                tr1::shared_ptr<const PackageIDSequence> versionIds(r->package_ids(*p));
                Item* p_item = makePackageItem(*versionIds->last(),
                                            m_main->taskbox()->tasks(),
                                            stringify(p->package).c_str(),
                                            stringify(p->category).c_str(),
                                            stringify(r->name()).c_str(),
                                            false,
                                            Item::is_stable,
                                            Item::ur_parent,
                                            root,
                                            "");

                root->appendChild(p_item);

                int mp=0;
                int ip=0;
                for (PackageIDSequence::ConstIterator vstart(versionIds->begin()),vend(versionIds->end());
                     vstart != vend;
                     ++vstart)
                {
                    QString reasons;
                    for (paludis::PackageID::MasksConstIterator m((*vstart)->begin_masks()), m_end((*vstart)->end_masks()) ;
                         m != m_end ; ++m)
                    {
                        reasons.append(stringify((*m)->description()).c_str());
                    }

                    Item* v_item = makeVersionItem(*vstart,
                                                m_main->taskbox()->tasks(),
                                                stringify((*vstart)->version()).c_str(),
                                                ( (installed(*vstart) ) ? Qt::Checked : Qt::Unchecked),
                                                Item::is_stable,
                                                Item::ur_child,
                                                p_item,
                                                reasons);
                    if (v_item->data(Item::io_installed).toInt() != 0)
                        ++ip;
                    p_item->appendChild(v_item);
                    m_main->taskbox()->setItemTasks(v_item);
                    if (! ( (*vstart)->begin_masks()  == (*vstart)->end_masks() ) )
                    {
                        v_item->setState(Item::is_masked);
                        ++mp;
                    }
                    else
                        p_item->setBestChild(v_item);
                }

                if ( ip > 0 )
                    p_item->setData(Item::io_installed,Qt::Checked);
                if (mp == p_item->childCount())
                    p_item->setState(Item::is_masked);
            }
        }
    }

    emit packagesResult(root);
}
