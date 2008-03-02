
/* Copyright (C) 2007-2008 Stefan Koegl
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
#include "PackageModel.hh"
#include "Package.hh"
#include "Selections.hh"
#include "make_package.hh"

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

#include <QList>

using namespace pertubis;

PackagesThread::PackagesThread(QObject* pobject,
    const paludis::tr1::shared_ptr<paludis::Environment>&  myenv,
    Selections* install,
    Selections* deinstall) :
    ThreadBase(pobject,myenv),
    m_install(install),
    m_deinstall(deinstall)
{
}

PackagesThread::~PackagesThread()
{
    qDebug() << "~PackagesThread()";
}

void
PackagesThread::setup(QString str)
{
    m_query = str;
}

void
PackagesThread::run()
{
    using namespace paludis;
    CategoryNamePart cat(m_query.toLatin1().data());
    const tr1::shared_ptr< const PackageIDSequence > packageIds(
        env()->package_database()->query(
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
        if (old_qpn != (*vstart)->name())
        {
            if (p_item != 0)
            {
                if (maskedVersionCount == p_item->childCount())
                    p_item->setPackageState(ps_masked);
                maskedVersionCount=0;

                QStringList tmp(pReasons.toList());
                pReasons.clear();
                p_item->setData(pho_mask_reasons,tmp.join(", "));
            }

            p_item = makePackage(*vstart,
                                m_install->hasEntry(*vstart),
                                m_deinstall->hasEntry(*vstart),
                                QString::fromStdString(stringify((*vstart)->name().package)),
                                QString::fromStdString(stringify((*vstart)->name().category)),
                                Qt::Unchecked,
                                ps_stable,
                                0,
                                "");

            old_qpn = (*vstart)->name();
            ++count;
            emit addPackage(p_item);
//             qDebug() << "emit addPackage()";
        }

        if (!execMode())
            return;
        QStringList vReasons;
        for (paludis::PackageID::MasksConstIterator m((*vstart)->begin_masks()), m_end((*vstart)->end_masks()) ;
                m != m_end ; ++m)
        {
            vReasons.push_back(QString::fromStdString(stringify((*m)->description())));
        }
        pReasons.unite(vReasons.toSet());

        bool vInstalled(installed(env(),*vstart));
        Package* v_item = makeVersionPackage(*vstart,
                                    m_install->hasEntry(*vstart),
                                    m_deinstall->hasEntry(*vstart),
                                    QString::fromStdString(stringify((*vstart)->version())),
                                    QString::fromStdString(stringify((*vstart)->repository()->name())),
                                    (vInstalled ? Qt::Checked : Qt::Unchecked),
                                    (vReasons.isEmpty() ? ps_stable : ps_masked),
                                    p_item,
                                    vReasons.join(", "));
//         m_taskbox->setTasksInPackage(v_item);
        if (v_item->data(pho_installed).toInt() != Qt::Unchecked)
        {
            p_item->setData(pho_installed,Qt::Checked);
        }

        if (!v_item->available())
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
    emit resultCount(count);
}
