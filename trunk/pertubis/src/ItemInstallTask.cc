
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
#include <paludis/package_id.hh>
#include <paludis/hook.hh>
#include <libwrapiter/libwrapiter_forward_iterator.hh>

pertubis::Install::Install(QObject* pobj,
                        DatabaseView* main,
                        const paludis::DepListOptions & options,
                        paludis::tr1::shared_ptr<const paludis::DestinationsSet> destinations) :
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
// pretend = true/false
    return m_pretend;
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

// void pertubis::Install::add_target(const paludis::tr1::shared_ptr<paludis::PackageID>& s)
// {
//     for (paludis::PackageIDSet::ConstIterator i(s.begin()), i_end(s.end());
//          i != i_end ; ++i)
//     {
//
//         paludis::tr1::shared_ptr<paludis::VersionRequirements> vReq(new paludis::VersionRequirements());
//         vReq->push_back(paludis::VersionRequirement(paludis::VersionOperator(paludis::vo_equal),(*i)->version()));
//       paludis::tr1::shared_ptr<paludis::PackageDepSpec> spec(
//              new paludis::PackageDepSpec(i->get()->canonical_form(paludis::idcf_full),pds_pm_eapi_0_strict));
//                                      paludis::tr1::shared_ptr<paludis::QualifiedPackageName>(new paludis::QualifiedPackageName((*i)->name())),
//                 paludis::tr1::shared_ptr<paludis::CategoryNamePart>(),
//                 paludis::tr1::shared_ptr<paludis::PackageNamePart>(),
//                 vReq,
//                 paludis::vr_and,
//                 paludis::tr1::shared_ptr<paludis::SlotName>(new paludis::SlotName((*i)->slot()))));
//         spec->set_tag(paludis::tr1::shared_ptr<const paludis::DepTag>(new paludis::TargetDepTag));
//         _imp->targets->add(paludis::tr1::shared_ptr<paludis::TreeLeaf<paludis::SetSpecTree, paludis::PackageDepSpec> >(
//                            new paludis::TreeLeaf<paludis::SetSpecTree, paludis::PackageDepSpec>(spec)));
//     }
// }
