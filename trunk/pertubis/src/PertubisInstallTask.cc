
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
#include "Selections.hh"
#include "Package.hh"
#include "FormatterUtils.hh"
#include "MessageOutput.hh"

#include <algorithm>

#include <paludis/hook.hh>
#include <paludis/util/indirect_iterator.hh>
#include <paludis/util/indirect_iterator-impl.hh>
#include <paludis/util/sequence.hh>
#include <paludis/util/set.hh>
#include <paludis/util/set-impl.hh>
#include <paludis/util/system.hh>
#include <paludis/util/tr1_functional.hh>
#include <paludis/util/wrapped_forward_iterator.hh>
#include <paludis/version_operator.hh>
#include <paludis/version_requirements.hh>

#include <QObject>
#include <QDebug>
#include <QString>
#include <QStringList>


namespace pertubis
{
    pertubis::Package*  makeNodePackage(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                                        Qt::CheckState install,
                                        Qt::CheckState deinstall,
                                        const QString & pack,
                                        const QString & cat,
                                        const QString & version,
                                        const QString & repository)
                                        {
                                            QVector<QVariant> data(6);
                                            data[po_install] = install;
                                            data[po_deinstall] = deinstall;
                                            data[po_package] = pack;
                                            data[po_category] = cat;
                                            data[po_repository] = version ;
                                            data[po_installed] = repository;
                                            return new Package(id,data,ps_stable,pt_node_only,0);
                                        }
}


void pertubis::PertubisInstallTask::on_clean_fail(const paludis::DepListEntry &,
                                      const paludis::PackageID& c,
                                      const int /*x*/,
                                      const int /*y*/,
                                      const int /*s*/,
                                      const int /*f*/)
{
    emit sendMessage(QString("(%1 Failed cleaning").arg(QString::fromStdString(stringify(c))));
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
                        m_deinstall(deinstall)
{
    std::fill_n( m_counts, static_cast<int>(last_count), 0);
}

void pertubis::PertubisInstallTask::on_installed_paludis()
{
}

void pertubis::PertubisInstallTask::on_display_merge_list_pre()
{
    qDebug() << "on_display_merge_list_pre()";
//     emit sendMessage(QString::fromStdString(header(color(std::string("These packages will be installed:"),"green"))));
}

void pertubis::PertubisInstallTask::on_display_merge_list_entry(const paludis::DepListEntry& e)
{
    qDebug() << "on_display_merge_list_entry()";
    if ( paludis::dlk_already_installed != e.kind )
    {
        QStringList tags;
        if (paludis::dlk_block == e.kind)
        {
            m_deinstall->addEntry(e.package_id);
            for (paludis::Set<paludis::DepTagEntry>::ConstIterator
                 tag(e.tags->begin()),
                     tag_end(e.tags->end()) ;
                     tag != tag_end ; ++tag)
            {
                if (tag->tag->category() != "dependency")
                    continue;
                tags.push_back(QString::fromStdString(tag->tag->short_text()));
            }
        }
        else
            m_install->addEntry(e.package_id);

        Package* node(makeNodePackage(e.package_id,
                    m_install->hasEntry(e.package_id),
                    m_deinstall->hasEntry(e.package_id),
                    QString::fromStdString(paludis::stringify(e.package_id->name().package)),
                    QString::fromStdString(paludis::stringify(e.package_id->name().category)),
                    QString::fromStdString(paludis::stringify(e.package_id->version())),
                    QString::fromStdString(paludis::stringify(e.package_id->repository()->name()))));

        if (paludis::dlk_block == e.kind)
        {
            QVector<QVariant> data(4);
            data[0] = Qt::Unchecked;
            data[1] = Qt::Unchecked;
            data[2] = tr("block");
            data[3] = tags.join(", ");
            Package* blockTag (new Package(e.package_id,data,ps_masked,pt_child,node));
            node->appendChild(blockTag);
        }

        node->setData(po_install,m_install->hasEntry(e.package_id));
        node->setData(po_deinstall,m_deinstall->hasEntry(e.package_id));
        emit appendPackage(node);
    }
}

void pertubis::PertubisInstallTask::on_build_deplist_pre()
{
    qDebug() << "on_build_deplist_pre()";
//     emit sendMessage("Building dependency list");
}

void pertubis::PertubisInstallTask::on_build_cleanlist_pre(const paludis::DepListEntry & d)
{
    qDebug() << "on_build_cleanlist_pre()";
    emit sendMessage(QString::fromStdString(header(color(std::string("Cleaning stale versions after installing ") + paludis::stringify(*d.package_id),"green"))));
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

void pertubis::PertubisInstallTask::on_clean_all_pre(const paludis::DepListEntry& /*d*/,
                                         const paludis::PackageIDSequence & c)
{
    qDebug() << "on_clean_all_pre()";
    using namespace paludis::tr1::placeholders;
    std::for_each(paludis::indirect_iterator(c.begin()), indirect_iterator(c.end()),
                  paludis::tr1::bind(paludis::tr1::mem_fn(&PertubisInstallTask::display_one_clean_all_pre_list_entry), this, _1));
}

void pertubis::PertubisInstallTask::on_no_clean_needed(const paludis::DepListEntry &)
{
    qDebug() << "on_no_clean_needed()";
    emit sendMessage(QString::fromStdString(header(color(std::string("No cleaning required"),std::string("green")))));
}

void pertubis::PertubisInstallTask::display_one_clean_all_pre_list_entry(const paludis::PackageID & c)
{
    emit sendMessage(QString::fromStdString(color(paludis::stringify(c),"blue")));
}
