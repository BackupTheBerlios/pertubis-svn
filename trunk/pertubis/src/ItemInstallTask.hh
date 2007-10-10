
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_INSTALL_TASK_H
#define _PERTUBIS_ENTRY_PROTECTOR_INSTALL_TASK_H 1

#include <paludis/tasks/install_task.hh>
#include <paludis/util/tr1_memory.hh>

namespace pertubis
{
    class Install : public paludis::InstallTask
    {
        public:
            Install(paludis::tr1::shared_ptr<paludis::Environment> env, const paludis::DepListOptions & options,
                        paludis::tr1::shared_ptr<const paludis::DestinationsSet> destinations) :
                ConsoleInstallTask(env.get(), options, destinations),
                                m_env(env)
            {
            }

            virtual bool want_full_install_reasons() const;

            virtual bool want_tags_summary() const;

            virtual bool want_install_reasons() const;

            virtual bool want_unchanged_use_flags() const;

            virtual bool want_changed_use_flags() const;

            virtual bool want_new_use_flags() const;

            virtual bool want_use_summary() const;

            virtual void on_installed_paludis();

            virtual HookResult perform_hook(const paludis::Hook & hook) const;

            void show_resume_command() const;

        private:
            paludis::tr1::shared_ptr<paludis::Environment>      m_env;
            DatabaseView*                                       m_mainview;
            bool                                                m_pretend;
    };
}

#endif
