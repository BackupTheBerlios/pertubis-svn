
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

#include <QObject>
#include <QDebug>
#include <QString>
#include "ItemInstallTask.hh"
#include "FormatterUtils.hh"
#include "MessageOutput.hh"
#include "DatabaseView.hh"
#include <paludis/util/system.hh>
#include <paludis/util/sequence.hh>
#include <paludis/version_operator.hh>
#include <paludis/version_requirements.hh>
#include <paludis/util/set.hh>
#include <paludis/util/set-impl.hh>
#include <paludis/util/tr1_functional.hh>
#include <paludis/package_id.hh>
#include <paludis/hook.hh>
#include <libwrapiter/libwrapiter_forward_iterator.hh>

pertubis::Install::Install(QObject* pobj,
                        DatabaseView* main,
                        const paludis::DepListOptions & options,
                        paludis::tr1::shared_ptr<const paludis::DestinationsSet> destinations) :
                        QThread(pobj),
                        paludis::InstallTask(main->getEnv().get(), options, destinations),
                        m_mainview(main)
{
}

bool pertubis::Install::want_full_install_reasons() const
{
    return false;
}

bool pertubis::Install::want_tags_summary() const
{
    return false;
}

bool pertubis::Install::want_install_reasons() const
{
    return false;
}

bool pertubis::Install::want_unchanged_use_flags() const
{
    return true;
}

bool pertubis::Install::want_changed_use_flags() const
{
    return false;
}

bool pertubis::Install::want_new_use_flags() const
{
    return false;
}

bool pertubis::Install::want_use_summary() const
{
    return false;
}

void pertubis::Install::on_installed_paludis()
{
    using namespace paludis;
    std::string r(stringify(environment()->root()));
    std::string exec_mode(getenv_with_default("PALUDIS_EXEC_PALUDIS", ""));

    if ("always" != exec_mode)
    {
        if ("never" == exec_mode)
            return;
        else if (! (r.empty() || r == "/"))
            return;
    }

    std::string resume_command(make_resume_command(*packages_not_yet_installed_successfully()));

    m_mainview->messages()->append(color(QString("Paludis has just upgraded Paludis"),QString("green")));
    execl("/bin/sh", "sh", "-c", resume_command.c_str(), static_cast<const char *>(0));
}

paludis::HookResult pertubis::Install::perform_hook(const paludis::Hook & hook) const
{
    std::string resume_command(make_resume_command(*packages_not_yet_installed_successfully()));
    if (resume_command.empty())
        return InstallTask::perform_hook(hook);
    return InstallTask::perform_hook(hook("RESUME_COMMAND", resume_command));
}

std::string pertubis::Install::make_resume_command(const paludis::PackageIDSequence& s) const
{
//     using namespace paludis;
//     std::string resume_command = environment()->paludis_command()
//             + " --" + QObject::tr("Install following packages").toLocal8Bit().data();
//
// //     resume_command = resume_command + CommandLine::get_instance()->install_args.resume_command_fragment(*this);
// //     resume_command = resume_command + CommandLine::get_instance()->dl_args.resume_command_fragment(*this);
//
//     for (PackageIDSequence::ConstIterator i(s.begin()), i_end(s.end()) ;
//          i != i_end ; ++i)
//         resume_command = resume_command + " '=" + stringify(**i) + "'";

//     return resume_command;
    return "";
}

void pertubis::Install::show_resume_command() const
{
}

void pertubis::Install::on_display_merge_list_pre()
{
    m_mainview->messages()->append(QString::fromStdString(header(color(std::string("These packages will be installed:"),"green"))));
}

void pertubis::Install::run()
{
    execute();
}

void pertubis::Install::on_build_deplist_pre()
{
    m_mainview->messages()->append("Building dependency list");
}

void pertubis::Install::on_build_cleanlist_pre(const paludis::DepListEntry & d)
{
    m_mainview->messages()->append(QString::fromStdString(header(color(std::string("Cleaning stale versions after installing ") + stringify(*d.package_id),"green"))));
}

void pertubis::Install::on_clean_all_pre(const paludis::DepListEntry & d,
                                         const paludis::PackageIDSequence & c)
{
    using namespace paludis::tr1::placeholders;
    std::for_each(indirect_iterator(c.begin()), indirect_iterator(c.end()),
                  paludis::tr1::bind(paludis::tr1::mem_fn(&Install::display_one_clean_all_pre_list_entry), this, _1));

//     display_clean_all_pre_list_end(d, c);
}

void pertubis::Install::on_display_merge_list_entry(const paludis::DepListEntry& c)
{
//     DisplayMode m;
//
//     do
//     {
//         switch (d.kind)
//         {
//             case dlk_provided:
//             case dlk_virtual:
//             case dlk_already_installed:
//                 if (! want_full_install_reasons())
//                     return;
//                 m = unimportant_entry;
//                 continue;
//
//             case dlk_package:
//             case dlk_subpackage:
//                 m = normal_entry;
//                 continue;
//
//             case dlk_suggested:
//                 m = suggested_entry;
//                 continue;
//
//             case dlk_masked:
//             case dlk_block:
//                 m = error_entry;
//                 continue;
//
//             case last_dlk:
//                 break;
//         }
//
//         throw InternalError(PALUDIS_HERE, "Bad d.kind");
//     } while (false);
//
//     tr1::shared_ptr<RepositoryName> repo;
//     if (d.destination)
//         repo.reset(new RepositoryName(d.destination->name()));
//
//     tr1::shared_ptr<const PackageIDSequence> existing_repo(environment()->package_database()->
//             query(query::Matches(PackageDepSpec(
//                   tr1::shared_ptr<QualifiedPackageName>(new QualifiedPackageName(d.package_id->name())),
//             tr1::shared_ptr<CategoryNamePart>(),
//                                               tr1::shared_ptr<PackageNamePart>(),
//                                                       tr1::shared_ptr<VersionRequirements>(),
//                                                               vr_and,
//                                                                       tr1::shared_ptr<SlotName>(),
//                                                                               repo)),
//             qo_order_by_version));
//
//     tr1::shared_ptr<const PackageIDSequence> existing_slot_repo(environment()->package_database()->
//             query(query::Matches(PackageDepSpec(
//                   tr1::shared_ptr<QualifiedPackageName>(new QualifiedPackageName(d.package_id->name())),
//             tr1::shared_ptr<CategoryNamePart>(),
//                                               tr1::shared_ptr<PackageNamePart>(),
//                                                       tr1::shared_ptr<VersionRequirements>(),
//                                                               vr_and,
//                                                                       tr1::shared_ptr<SlotName>(new SlotName(d.package_id->slot())),
//                                                                               repo)),
//             qo_order_by_version));
//
//     display_merge_list_entry_start(d, m);
//     display_merge_list_entry_package_name(d, m);
//     display_merge_list_entry_version(d, m);
//     display_merge_list_entry_repository(d, m);
//
//     if (d.package_id->virtual_for_key())
//         display_merge_list_entry_for(*d.package_id->virtual_for_key()->value(), m);
//
//     display_merge_list_entry_slot(d, m);
//
//     display_merge_list_entry_status_and_update_counts(d, existing_repo, existing_slot_repo, m);
//     display_merge_list_entry_use(d, existing_repo, existing_slot_repo, m);
//     display_merge_list_entry_tags(d, m);
//     display_merge_list_entry_end(d, m);
//
//     if (d.kind == dlk_masked)
//         display_merge_list_entry_mask_reasons(d);
}

void pertubis::Install::display_one_clean_all_pre_list_entry(const paludis::PackageID & c)
{
    m_mainview->messages()->append(QString::fromStdString(color(paludis::stringify(c),"blue")));
}
