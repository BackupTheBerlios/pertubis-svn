
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
#include "RepositoryListModel.hh"
#include "Item.hh"
#include "TaskBox.hh"
#include "Task.hh"
#include <QList>

#include <paludis/action.hh>
#include <paludis/environment.hh>
#include <paludis/mask.hh>
#include <paludis/metadata_key.hh>
#include <paludis/package_database.hh>
#include <paludis/package_id.hh>
#include <paludis/query.hh>
#include <paludis/util/indirect_iterator.hh>
#include <paludis/util/indirect_iterator-impl.hh>
#include <paludis/util/sequence.hh>
#include <paludis/util/set.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/wrapped_forward_iterator.hh>
#include <paludis/util/wrapped_forward_iterator-impl.hh>

pertubis::PackagesThread::PackagesThread( QObject* pobject,
                                          const paludis::tr1::shared_ptr<paludis::Environment>&  env,
                                          TaskBox* box) : ThreadBase(pobject,env,box)
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
    CategoryNamePart cat(m_query.toLatin1().data());
    const tr1::shared_ptr< const PackageIDSequence > packageIds(
        m_env->package_database()->query(
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
                pReasons.clear();
                if (p_item->data(Item::io_installed).toInt() != Qt::Unchecked &&
                    hasVersionChange(p_item->ID()))
                    {
                        p_item->setData(Item::io_change,"version change");
                        emit changeInCat(QString::fromStdString(stringify((*vstart)->name().category)));
                    }

                p_item->setData(Item::io_mask_reasons,tmp.join(", "));
            }

            p_item = makePackageItem(*vstart,
                                    m_taskbox->tasks(),
                                    QString::fromStdString(stringify((*vstart)->name().package)),
                                    QString::fromStdString(stringify((*vstart)->name().category)),
                                    Qt::Unchecked,
                                    Item::is_stable,
                                    0,
                                    "");

            old_qpn = (*vstart)->name();

            emit addPackage(p_item);
        }

        QStringList vReasons;
        for (paludis::PackageID::MasksConstIterator m((*vstart)->begin_masks()), m_end((*vstart)->end_masks()) ;
                m != m_end ; ++m)
        {
            vReasons.push_back(stringify((*m)->description()).c_str());
        }
        pReasons.unite(vReasons.toSet());

        Item* v_item = makeVersionItem(*vstart,
                                    m_taskbox->tasks(),
                                    stringify((*vstart)->version()).c_str(),
                                    QString::fromStdString(stringify((*vstart)->repository()->name())),
                                    ( installed(*vstart) ? Qt::Checked : Qt::Unchecked),
                                    (vReasons.isEmpty() ? Item::is_stable : Item::is_masked),
                                    p_item,
                                    vReasons.join(", "));
        m_taskbox->setTasksInItem(v_item);
        if (v_item->data(Item::io_installed).toInt() != Qt::Unchecked)
        {
            p_item->setData(Item::io_installed,Qt::Checked);
        }

        if (!v_item->available() )
        {
            ++maskedVersionCount;
        }
        else
        {
            if (p_item->bestChild() == 0)
                p_item->setBestChild(v_item);
        }

        p_item->prependChild(v_item);
    }
}
