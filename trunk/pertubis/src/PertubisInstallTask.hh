
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_INSTALL_H
#define _PERTUBIS_ENTRY_PROTECTOR_INSTALL_H 1

#include <QObject>
#include "Package-fwd.hh"
#include <paludis/install_task.hh>

namespace pertubis
{

    class Selections;
    /*! \brief installs packages
     *
     * \ingroup PaludisAdapter
     * This class controlls all aspects and steps of installing packages
     * \todo this thread blocks the main application thread and gui. Find the issue!
     */
    class PertubisInstallTask : public QObject,
        public paludis::InstallTask
    {
        Q_OBJECT
        public:

            enum Count
            {
                max_count,
                new_count,
                upgrade_count,
                downgrade_count,
                new_slot_count,
                rebuild_count,
                error_count,
                suggested_count,
                last_count
            };

            enum DisplayMode
            {
                normal_entry,
                unimportant_entry,
                error_entry,
                suggested_entry
            };

            PertubisInstallTask(QObject* pobj,
                    paludis::Environment* env,
                    const paludis::DepListOptions & options,
                    paludis::tr1::shared_ptr<const paludis::DestinationsSet> destinations,
                    Selections* install,
                    Selections* deinstall);
            ~PertubisInstallTask() {}

            void start(bool firstpass) { m_firstpass= firstpass;execute();}

            virtual void display_merge_list_post_counts();
            virtual void display_one_clean_all_pre_list_entry(const paludis::PackageID &){}
            virtual void on_all_masked_error(const paludis::AllMaskedError&) {}
            virtual void on_ambiguous_package_name_error(const paludis::AmbiguousPackageNameError&) {}
            virtual void on_build_cleanlist_post(const paludis::DepListEntry&) {}
            virtual void on_build_cleanlist_pre(const paludis::DepListEntry&) {}
            virtual void on_build_deplist_post() {}
            virtual void on_build_deplist_pre() {}
            virtual void on_clean_all_post(const paludis::DepListEntry&, const paludis::PackageIDSequence&) {}
            virtual void on_clean_all_pre(const paludis::DepListEntry&, const paludis::PackageIDSequence&) {}
            virtual void on_clean_fail(const paludis::DepListEntry&, const paludis::PackageID&, int, int, int, int) {}
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
            virtual void on_display_merge_list_entry(const paludis::DepListEntry&);
            virtual void on_display_merge_list_post() {}
            virtual void on_display_merge_list_pre() {}
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
            virtual void on_no_clean_needed(const paludis::DepListEntry&) {}
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

            template <Count count_>
            int count() const
            {
                return m_counts[count_];
            }

            template <Count count_>
            void set_count(const int value)
            {
                m_counts[count_] = value;
            }

        signals:

            /*! \brief sending the output of the build and install process to the main thread / window
            *
            */
            void sendMessage(const QString& message);

            /*! \brief after updating paludis we must rebuild pertubis
            *
            */
            void rebuildMyself(const QString& message);

            void appendPackage(Package * node);

            void depListResult(QString);

        private:

            void makePackage(const paludis::DepListEntry&, DisplayMode m);

            std::string     m_resumeCommand;
            Selections*     m_install;
            Selections*     m_deinstall;
            int             m_counts[last_count];
            bool            m_firstpass;
    };
}

#endif

