
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_INSTALL_H
#define _PERTUBIS_ENTRY_PROTECTOR_INSTALL_H 1

#include <QThread>
#include <paludis/install_task.hh>
#include <paludis/environment-fwd.hh>
#include <paludis/util/tr1_memory.hh>

namespace pertubis
{
    class Install : public QThread,
        public paludis::InstallTask
    {
        Q_OBJECT
        public:
            Install(QObject* pobj,
                    paludis::Environment* env,
                    const paludis::DepListOptions & options,
                    paludis::tr1::shared_ptr<const paludis::DestinationsSet> destinations);
            ~Install() {}

            void run();
            virtual bool want_changed_use_flags() const;
            virtual bool want_full_install_reasons() const;
            virtual bool want_install_reasons() const;
            virtual bool want_new_use_flags() const;
            virtual bool want_tags_summary() const;
            virtual bool want_unchanged_use_flags() const;
            virtual bool want_use_summary() const;
            virtual void on_all_masked_error(const paludis::AllMaskedError&) {}
            virtual void on_ambiguous_package_name_error(const paludis::AmbiguousPackageNameError&) {}
            virtual void on_build_cleanlist_post(const paludis::DepListEntry&) {}
            virtual void on_build_cleanlist_pre(const paludis::DepListEntry&);
            virtual void on_build_deplist_post() {}
            virtual void on_build_deplist_pre();
            virtual void on_clean_all_post(const paludis::DepListEntry&, const paludis::PackageIDSequence&) {}
            virtual void on_clean_all_pre(const paludis::DepListEntry&, const paludis::PackageIDSequence&);
            virtual void on_clean_fail(const paludis::DepListEntry&, const paludis::PackageID&, int, int, int, int);
            virtual void on_clean_post(const paludis::DepListEntry&, const paludis::PackageID&, int, int, int, int) {}
            virtual void on_clean_pre(const paludis::DepListEntry&, const paludis::PackageID&, int, int, int, int) {}
            virtual void on_dep_list_error(const paludis::DepListError&) {}
            virtual void on_display_failure_no_summary() {}
            virtual void on_display_failure_summary_failure(const paludis::DepListEntry&) {}
            virtual void on_display_failure_summary_post() {}
            virtual void on_display_failure_summary_pre() {}
            virtual void on_display_failure_summary_skipped_dependent(const paludis::DepListEntry&, const std::tr1::shared_ptr<const paludis::PackageID>&) {}
            virtual void on_display_failure_summary_skipped_unsatisfied(const paludis::DepListEntry&, const paludis::PackageDepSpec&) {}
            virtual void on_display_failure_summary_success(const paludis::DepListEntry&) {}
            virtual void on_display_failure_summary_totals(int, int, int, int) {}
            virtual void on_display_merge_list_entry(const paludis::DepListEntry&) {}
            virtual void on_display_merge_list_post() {}
            virtual void on_display_merge_list_pre();
            virtual void on_fetch_action_error(const paludis::FetchActionError&) {}
            virtual void on_fetch_all_post() {}
            virtual void on_fetch_all_pre() {}
            virtual void on_fetch_post(const paludis::DepListEntry&, int, int, int, int) {}
            virtual void on_fetch_pre(const paludis::DepListEntry&, int, int, int, int) {}
            virtual void on_had_both_package_and_set_targets_error(const paludis::HadBothPackageAndSetTargets&) {}
            virtual void on_install_action_error(const paludis::InstallActionError&) {}
            virtual void on_install_all_post() {}
            virtual void on_install_all_pre() {}
            virtual void on_installed_paludis();
            virtual void on_install_fail(const paludis::DepListEntry&, int, int, int, int) {}
            virtual void on_install_post(const paludis::DepListEntry&, int, int, int, int) {}
            virtual void on_install_pre(const paludis::DepListEntry&, int, int, int, int) {}
            virtual void on_multiple_set_targets_specified(const paludis::MultipleSetTargetsSpecified&) {}
            virtual void on_no_clean_needed(const paludis::DepListEntry&);
            virtual void on_no_such_package_error(const paludis::NoSuchPackageError&) {}
            virtual void on_not_continuing_due_to_errors() {}
            virtual void on_preserve_world() {}
            virtual void on_skip_dependent(const paludis::DepListEntry&, const paludis::tr1::shared_ptr<const paludis::PackageID>&, int, int, int, int) {}
            virtual void on_skip_unsatisfied(const paludis::DepListEntry&, const paludis::PackageDepSpec&, int, int, int, int) {}
            virtual void on_update_world(const paludis::PackageDepSpec&) {}
            virtual void on_update_world(const paludis::SetName&) {}
            virtual void on_update_world_post() {}
            virtual void on_update_world_pre() {}
            virtual void on_update_world_skip(const paludis::PackageDepSpec&, const std::string&) {}
            virtual void on_update_world_skip(const paludis::SetName&, const std::string&) {}
            virtual void on_use_requirements_not_met_error(const paludis::UseRequirementsNotMetError&) {}

            virtual paludis::HookResult perform_hook(const paludis::Hook & hook) const;
            virtual void display_one_clean_all_pre_list_entry(const paludis::PackageID & c);

            void show_resume_command() const;
            std::string make_resume_command(const paludis::PackageIDSequence& seq) const;

        signals:


            /*! \brief sending the output of the build and install process to the main thread / window
            *
            */
            void sendMessage(const QString& message);

            /*! \brief after updating paludis we must rebuild pertubis
            *
            */
            void rebuildMyself(const QString& message);

        private:

            std::string                                         m_resumeCommand;
    };
}

#endif

