
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
#include <paludis/util/system.hh>
#include <paludis/util/sequence.hh>
#include <paludis/version_operator.hh>
#include <paludis/version_requirements.hh>
#include <paludis/util/indirect_iterator.hh>
#include <paludis/util/indirect_iterator-impl.hh>
#include <paludis/util/set.hh>
#include <paludis/util/set-impl.hh>
#include <paludis/util/tr1_functional.hh>
#include <paludis/package_id.hh>
#include <paludis/hook.hh>
#include <paludis/util/wrapped_forward_iterator.hh>

pertubis::Install::Install(QObject* pobj,
                        paludis::Environment* env,
                        const paludis::DepListOptions & options,
                        paludis::tr1::shared_ptr<const paludis::DestinationsSet> destinations) :
                        QThread(pobj),
                        paludis::InstallTask(env, options, destinations)
{
}

void pertubis::Install::run()
{
    execute();
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

//     std::string resume_command(make_resume_command(*packages_not_yet_installed_successfully()));
    emit sendMessage(color(QString("Paludis has just upgraded Paludis"),QString("green")));
//     execl("/bin/sh", "sh", "-c", resume_command.c_str(), static_cast<const char *>(0));
}

void pertubis::Install::on_display_merge_list_pre()
{
    emit sendMessage(QString::fromStdString(header(color(std::string("These packages will be installed:"),"green"))));
}

void pertubis::Install::on_build_deplist_pre()
{
    emit sendMessage("Building dependency list");
}

void pertubis::Install::on_build_cleanlist_pre(const paludis::DepListEntry & d)
{
    emit sendMessage(QString::fromStdString(header(color(std::string("Cleaning stale versions after installing ") + paludis::stringify(*d.package_id),"green"))));
}

void pertubis::Install::on_clean_all_pre(const paludis::DepListEntry& /*d*/,
                                         const paludis::PackageIDSequence & c)
{
    using namespace paludis::tr1::placeholders;
    std::for_each(paludis::indirect_iterator(c.begin()), indirect_iterator(c.end()),
                  paludis::tr1::bind(paludis::tr1::mem_fn(&Install::display_one_clean_all_pre_list_entry), this, _1));

//     display_clean_all_pre_list_end(d, c);
}

void pertubis::Install::on_no_clean_needed(const paludis::DepListEntry &)
{
    emit sendMessage(QString::fromStdString(header(color(std::string("No cleaning required"),std::string("green")))));
}

void pertubis::Install::on_clean_fail(const paludis::DepListEntry &,
                                     const paludis::PackageID& c,
                                     const int /*x*/,
                                     const int /*y*/,
                                     const int /*s*/,
                                     const int /*f*/)
{
    emit sendMessage(QString("(%1 Failed cleaning").arg(QString::fromStdString(stringify(c))));
}

paludis::HookResult pertubis::Install::perform_hook(const paludis::Hook & hook) const
{
    std::string resume_command(make_resume_command(*packages_not_yet_installed_successfully()));
    if (resume_command.empty())
        return InstallTask::perform_hook(hook);
    return InstallTask::perform_hook(hook("RESUME_COMMAND", resume_command));
}

void pertubis::Install::display_one_clean_all_pre_list_entry(const paludis::PackageID & c)
{
    emit sendMessage(QString::fromStdString(color(paludis::stringify(c),"blue")));
}

std::string pertubis::Install::make_resume_command(const paludis::PackageIDSequence& s) const
{
    using namespace paludis;
    std::string resume_command = environment()->paludis_command()
            + " --" + QObject::tr("Install following packages").toLocal8Bit().data();

    for (PackageIDSequence::ConstIterator i(s.begin()), i_end(s.end()) ;
         i != i_end ; ++i)
        resume_command = resume_command + " '=" + stringify(**i) + "'";

    return resume_command;
}

void pertubis::Install::show_resume_command() const
{
}

