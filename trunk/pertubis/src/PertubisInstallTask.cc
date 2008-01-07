
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

#include "PertubisInstallTask.hh"
#include "Package.hh"
#include "Selections.hh"
#include "SelectionModel-fwd.hh"

#include <paludis/dep_spec.hh>
#include <paludis/query.hh>
#include <paludis/util/sequence.hh>

#include <QDebug>
#include <QString>
#include <QStringList>


static pertubis::Package*  makeNodePackage(
    paludis::tr1::shared_ptr<const paludis::PackageID> id,
    Qt::CheckState install,
    Qt::CheckState deinstall,
    const QString & pack,
    const QString & cat,
    const QString & version,
    const QString & repository)
{
    using namespace pertubis;
    QVector<QVariant> data(6);
    data[pho_install] = install;
    data[pho_deinstall] = deinstall;
    data[pho_package] = pack;
    data[pho_category] = cat;
    data[pho_repository] = version ;
    data[pho_installed] = repository;
    return new Package(id,data,ps_stable,pt_node_only,0);
}

pertubis::PertubisInstallTask::PertubisInstallTask(QObject* pobj,
                        paludis::Environment* env,
                        const paludis::DepListOptions & options,
                        paludis::tr1::shared_ptr<const paludis::DestinationsSet> destinations,
                        Selections* install,
                        Selections* deinstall) :
                        QObject(pobj),
                        paludis::InstallTask(env, options, destinations),
                        m_install(install),
                        m_deinstall(deinstall),
                        m_firstpass(true)
{
    std::fill_n( m_counts, static_cast<int>(last_count), 0);
}

void pertubis::PertubisInstallTask::on_installed_paludis()
{
}

void pertubis::PertubisInstallTask::on_display_merge_list_entry(const paludis::DepListEntry& e)
{
    using namespace paludis;
    DisplayMode m;

    do
    {
        switch (e.kind)
        {
            case dlk_provided:
            case dlk_virtual:
            case dlk_already_installed:
//                 if (! want_full_install_reasons())
                    return;
//                 m = unimportant_entry;
//                 continue;

            case dlk_package:
            case dlk_subpackage:
                m = normal_entry;
                continue;

            case dlk_suggested:
                m = suggested_entry;
                continue;

            case dlk_masked:
            case dlk_block:
                m = error_entry;
                continue;

            case last_dlk:
                break;
        }

        throw InternalError(PALUDIS_HERE, "Bad d.kind");
    } while (false);

    makePackage(e, m);
}

void pertubis::PertubisInstallTask::makePackage(const paludis::DepListEntry& e, DisplayMode m)
{
    using namespace paludis;
    qDebug() << "on_display_merge_list_entry()";
    QVector<QVariant> vdata(spho_last);
    QStringList tags;
    Package* node(new Package(e.package_id,QVector<QVariant>(spho_last),ps_stable,pt_parent,0));
    node->setData(spho_install,m_install->hasEntry(e.package_id));
    node->setData(spho_deinstall,Qt::Unchecked);
    node->setData(spho_package,QString::fromStdString(stringify(e.package_id->name().package)));
    node->setData(spho_category,QString::fromStdString(stringify(e.package_id->name().category)));
    tr1::shared_ptr<RepositoryName> repo;
    if (e.destination)
        repo.reset(new RepositoryName(e.destination->name()));
    tr1::shared_ptr<const PackageIDSequence> existing_repo(environment()->package_database()->
            query(query::Matches(repo ?
                        make_package_dep_spec().package(e.package_id->name()).repository(*repo) :
            make_package_dep_spec().package(e.package_id->name())),
                                  qo_order_by_version));

    tr1::shared_ptr<const PackageIDSequence> existing_slot_repo(environment()->package_database()->
            query(query::Matches(repo ?
                        make_package_dep_spec().package(e.package_id->name()).slot(e.package_id->slot()).repository(*repo) :
            make_package_dep_spec().package(e.package_id->name()).slot(e.package_id->slot())),
                                  qo_order_by_version));

    switch (m)
    {
        case normal_entry:
            if (m_firstpass)
                return;

            if ( existing_repo->empty())
            {
                node->setData(spho_new_version,QString::fromStdString(stringify(e.package_id->version())));
                node->setData(spho_change_sign, "N");
            }
            else if ( existing_slot_repo->empty())
            {
                node->setData(spho_change_sign,"S");
                node->setData(spho_new_version,QString::fromStdString(stringify(e.package_id->version())));
            }
            else if ( (*existing_slot_repo->last())->version() < e.package_id->version())
            {
                node->setData(spho_old_version,QString::fromStdString(stringify((*existing_slot_repo->last())->version())));
                node->setData(spho_change_sign, "U");
                node->setData(spho_new_version,QString::fromStdString(stringify(e.package_id->version())));
            }
            else if ((*existing_slot_repo->last())->version() > e.package_id->version())
            {
                node->setData(spho_old_version, QString::fromStdString(stringify((*existing_slot_repo->last())->version())));
                node->setData(spho_change_sign,"D");
                node->setData(spho_new_version, QString::fromStdString(stringify(e.package_id->version())));
            }
            else
            {
                node->setData(spho_change_sign, QString("R "));
                node->setData(spho_new_version, QString::fromStdString(stringify(e.package_id->version())));
            }

            node->setData(spho_repository, QString::fromStdString(stringify(e.package_id->repository()->name())));
            emit appendPackage(node);

            break;

        case error_entry:

            if (dlk_block == e.kind)
            {
                if (m_firstpass)
                {
                    m_deinstall->addEntry(e.package_id);
                    return;
                }
//                 node->setData(spho_deinstall,Qt::Checked);
                for (Set<DepTagEntry>::ConstIterator
                     tag(e.tags->begin()),
                         tag_end(e.tags->end()) ;
                         tag != tag_end ; ++tag)
                {
                    if (tag->tag->category() != "dependency")
                        continue;
                    tags.push_back(QString::fromStdString(tag->tag->short_text()));
                }

                vdata[spho_install] = Qt::Unchecked;
                vdata[spho_deinstall] = Qt::Unchecked;
                vdata[spho_installed] = Qt::Unchecked;
                vdata[spho_package] = tr("block");
                vdata[spho_category] = tags.join(", ");
                node->appendChild(new Package(e.package_id,vdata,ps_masked,pt_child,node));
            }
            break;
        default:
            ;
    }
}

void pertubis::PertubisInstallTask::display_merge_list_post_counts()
{
    QString text;
    text.append(tr("Total") + " " + count<max_count>());

    if (count<max_count>())
    {
        if (count<new_count>())
        {
            text.append(tr("new") + " " + count<new_count>() );
        }
        if (count<upgrade_count>())
        {
            text.append(tr("upgrade(s)") + " " + count<upgrade_count>() );
        }
        if (count<downgrade_count>())
        {
            text.append(tr("downgrade(s)") + " " + count<downgrade_count>() );
        }
        if (count<new_slot_count>())
        {
            text.append(tr("new slot(s)") + " " + count<new_slot_count>() );
        }
        if (count<rebuild_count>())
        {
            text.append(tr("rebuild(s)") + " " + count<rebuild_count>() );
        }
    }

    if (count<error_count>())
        text.append(tr("error(s)") + " " + count<error_count>() );

    if (count<suggested_count>())
        text.append(tr("suggestion(s)") + " " + count<suggested_count>() );
    emit depListResult(text);
}
