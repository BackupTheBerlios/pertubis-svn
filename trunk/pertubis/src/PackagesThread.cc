
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

#include "PackagesThread.hh"
#include "RepositoryListModel.hh"
#include "PaludisUtils.hh"
#include "Package.hh"
#include "TaskBox.hh"
#include "Task.hh"
#include <QList>
#include <QMutexLocker>

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
    ThreadBase::lock();
    using namespace paludis;
    CategoryNamePart cat(m_query.toLatin1().data());
    const tr1::shared_ptr< const PackageIDSequence > packageIds(
        m_env->package_database()->query(
                query::Category(cat) &
                query::SupportsAction<InstallAction>(),
                qo_order_by_version));
    int count(0);
    int maskedVersionCount(0);
    QualifiedPackageName old_qpn("OLD/OLD");
    Package* p_item(0);
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
                    p_item->setPackageState(Package::ps_masked);
                maskedVersionCount=0;

                QStringList tmp(pReasons.toList());
                pReasons.clear();
                if (p_item->data(Package::po_installed).toInt() != Qt::Unchecked &&
                    hasVersionChange(m_env,p_item->ID()))
                    {
                        p_item->setData(Package::po_change,"version change");
                        emit changeInCat(QString::fromStdString(stringify((*vstart)->name().category)));
                    }

                p_item->setData(Package::po_mask_reasons,tmp.join(", "));
            }

            p_item = makePackagePackage(*vstart,
                                    m_taskbox->tasks(),
                                    QString::fromStdString(stringify((*vstart)->name().package)),
                                    QString::fromStdString(stringify((*vstart)->name().category)),
                                    Qt::Unchecked,
                                    Package::ps_stable,
                                    0,
                                    "");

            old_qpn = (*vstart)->name();
            ++count;
            emit addPackage(p_item);
        }

        QStringList vReasons;
        for (paludis::PackageID::MasksConstIterator m((*vstart)->begin_masks()), m_end((*vstart)->end_masks()) ;
                m != m_end ; ++m)
        {
            vReasons.push_back(stringify((*m)->description()).c_str());
        }
        pReasons.unite(vReasons.toSet());

        Package* v_item = makeVersionPackage(*vstart,
                                    m_taskbox->tasks(),
                                    stringify((*vstart)->version()).c_str(),
                                    QString::fromStdString(stringify((*vstart)->repository()->name())),
                                    ( installed(m_env,*vstart) ? Qt::Checked : Qt::Unchecked),
                                    (vReasons.isEmpty() ? Package::ps_stable : Package::ps_masked),
                                    p_item,
                                    vReasons.join(", "));
        m_taskbox->setTasksInPackage(v_item);
        if (v_item->data(Package::po_installed).toInt() != Qt::Unchecked)
        {
            p_item->setData(Package::po_installed,Qt::Checked);
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
    ThreadBase::unlock();
    emit finished(count);
}
