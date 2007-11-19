
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
#include <QList>

#include <paludis/util/wrapped_forward_iterator.hh>
#include <paludis/util/wrapped_forward_iterator-impl.hh>
#include <paludis/environment.hh>
#include <paludis/mask.hh>
#include <paludis/package_database.hh>

#include <paludis/package_id.hh>
#include <paludis/metadata_key.hh>
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

void pertubis::PackagesThread::start(QString str)
{
    m_query = str;
    QThread::start();
}

void pertubis::PackagesThread::run()
{
    using namespace paludis;

    TaskBox* box(m_main->taskbox());
    CategoryNamePart cat(m_query.toLatin1().data());
    const tr1::shared_ptr< const PackageIDSequence > packageIds(
        m_main->getEnv()->package_database()->query(
                query::Category(cat) &
                query::SupportsAction<InstallAction>(),
                qo_order_by_version));

    int maskedVersionCount=0;
    QualifiedPackageName old_qpn("OLD/OLD");
    Item* p_item=0;
    QSet<QString> pReasons;
    for (PackageIDSequence::ReverseConstIterator vstart(packageIds->rbegin()),vend(packageIds->rend());
         vstart != vend;
         ++vstart)
    {
        if (old_qpn != (*vstart)->name() )
        {
            if (p_item != 0)
            {
                if (maskedVersionCount == p_item->childCount())
                    p_item->setState(Item::is_masked);
                maskedVersionCount=0;

                QStringList tmp(pReasons.toList());
                p_item->setData(Item::io_mask_reasons,tmp.join(", "));
                pReasons.clear();
            }

            p_item = makePackageItem(*vstart,
                                    box->tasks(),
                                    QString::fromStdString(stringify((*vstart)->name().package)),
                                    QString::fromStdString(stringify((*vstart)->name().category)),
                                    Qt::Unchecked,
                                    Item::is_stable,
                                    Item::ur_parent,
                                    0,
                                    "");

            qDebug() << "pertubis::PackagesThread::run()" << *p_item;
            old_qpn = (*vstart)->name();
            emit addPackage(p_item);
        }

        QSet<QString> vReasons;
        for (paludis::PackageID::MasksConstIterator m((*vstart)->begin_masks()), m_end((*vstart)->end_masks()) ;
                m != m_end ; ++m)
        {
            vReasons.insert(stringify((*m)->description()).c_str());
        }
        pReasons.unite(vReasons);

        QStringList vstm(pReasons.toList());

        Item* v_item = makeVersionItem(*vstart,
                                    box->tasks(),
                                    stringify((*vstart)->version()).c_str(),
                                    QString::fromStdString(stringify((*vstart)->repository()->name())),
                                    ( installed(*vstart) ? Qt::Checked : Qt::Unchecked),
                                    (vReasons.isEmpty() ? Item::is_stable : Item::is_masked),
                                    Item::ur_child,
                                    p_item,
                                    vstm.join(", "));

        box->setItemTasks(v_item);
        if (v_item->data(Item::io_installed).toInt() != Qt::Unchecked)
            p_item->setData(Item::io_installed,Qt::Checked);
        if (!v_item->available() )
        {
            ++maskedVersionCount;
        }
        else
        {
            if (p_item->bestChild() == 0)
            {
                p_item->setBestChild(v_item);
            }
//             if (v_item->data(Item::io_installed).toInt() == Qt::Unchecked)
//             {
//                 v_item->setData(Item::io_change,"new version");
// //                     p_item->setData(Item::io_change,"new version");
//             }
        }
        p_item->prependChild(v_item);
    }
}
