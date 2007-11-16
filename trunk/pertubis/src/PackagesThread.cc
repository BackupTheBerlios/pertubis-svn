
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

#include <paludis/util/wrapped_forward_iterator.hh>
#include <paludis/util/wrapped_forward_iterator-impl.hh>
#include <paludis/environment.hh>
#include <paludis/mask.hh>
#include <paludis/package_database.hh>

#include <paludis/package_id.hh>
#include <paludis/action.hh>
#include <paludis/query.hh>
#include <paludis/util/indirect_iterator.hh>
#include <paludis/util/indirect_iterator-impl.hh>
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
    QMap<QString,Item*> map;
    TaskBox* box(m_main->taskbox());
    CategoryNamePart cat(m_query.toLatin1().data());
    const tr1::shared_ptr< const PackageIDSequence > packageIds(
    m_main->getEnv()->package_database()->query(
                query::Category(cat),
                qo_order_by_version));

    int maskedPackageCount=0;
    int installedPackageCount=0;
    for (PackageIDSequence::ConstIterator vstart(packageIds->begin()),vend(packageIds->end());
         vstart != vend;
         ++vstart)
    {

        if ( (*vstart)->repository()->format() == "virtuals" ||
            (*vstart)->repository()->format() == "vdb" ||
            (*vstart)->repository()->format() == "installed_virtuals")
            continue;

        // get the package name once
        QString name(QString::fromStdString(stringify((*vstart)->name().package)));
        QString repo(QString::fromStdString(stringify((*vstart)->repository()->name())));
        Item* p_item=0;

        // test the presence of a package item node with the name
        if (map.constEnd() == map.find(name) )
        {
            p_item = makePackageItem(*packageIds->last(),
                                    box->tasks(),
                                    name,
                                    QString::fromStdString(stringify((*vstart)->name().category)),
                                    repo,
                                    false,
                                    Item::is_stable,
                                    Item::ur_parent,
                                    root,
                                    "");
            map.insert(name,p_item);
            root->appendChild(p_item);
        }
        else
        {
            p_item = map.find(name).value();
        }

        QString reasons;
        for (paludis::PackageID::MasksConstIterator m((*vstart)->begin_masks()), m_end((*vstart)->end_masks()) ;
                m != m_end ; ++m)
        {
            reasons.append(stringify((*m)->description()).c_str());
        }

        Item* v_item = makeVersionItem(*vstart,
                                    box->tasks(),
                                    stringify((*vstart)->version()).c_str(),
                                    repo,
                                    /*( ((*vstart)->supports_action(UninstallAction()) ? Qt::Checked : Qt::Unchecked)*/ 0,
                                    Item::is_stable,
                                    Item::ur_child,
                                    p_item,
                                    reasons);
        if (v_item->data(Item::io_installed).toInt() != 0)
            ++installedPackageCount;
        p_item->appendChild(v_item);
        box->setItemTasks(v_item);
        if (! ( (*vstart)->begin_masks()  == (*vstart)->end_masks() ) )
        {
            v_item->setState(Item::is_masked);
            ++maskedPackageCount;
        }
        else
            p_item->setBestChild(v_item);
    }

//     if ( installedPackageCount > 0 )
//         p_item->setData(Item::io_installed,Qt::Checked);
//     if (maskedPackageCount == p_item->childCount())
//         p_item->setState(Item::is_masked);

    emit packagesResult(root);
}
