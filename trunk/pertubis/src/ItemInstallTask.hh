
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
#define _PERTUBIS_ENTRY_PROTECTOR_INSTALL_TASK_H

#include <paludis/tasks/install_task.hh>

std::string make_resume_command(const Environment * const env, const InstallTask & task, const PackageIDSequence & s)
{
    std::string resume_command = env->paludis_command()
            + " --" + CommandLine::get_instance()->dl_deps_default.long_name() + " discard --"
            + CommandLine::get_instance()->a_install.long_name();

    for (PackageIDSequence::Iterator i(s.begin()), i_end(s.end()) ;
         i != i_end ; ++i)
        resume_command = resume_command + " '=" + stringify(**i) + "'";

    if (CommandLine::get_instance()->a_add_to_world_spec.specified())
        resume_command = resume_command + " --" + CommandLine::get_instance()->a_add_to_world_spec.long_name()
                + " '" + CommandLine::get_instance()->a_add_to_world_spec.argument() + "'";
    else if (! CommandLine::get_instance()->a_preserve_world.specified())
    {
        if (task.had_set_targets())
            resume_command = resume_command + " --" + CommandLine::get_instance()->a_add_to_world_spec.long_name()
                    + " '( )'";
        else
            resume_command = resume_command + " --" + CommandLine::get_instance()->a_add_to_world_spec.long_name()
                    + " '( " + join(task.begin_targets(), task.end_targets(), " ") + " )'";
    }

    if (CommandLine::get_instance()->a_destinations.specified())
        for (args::StringSetArg::Iterator i(CommandLine::get_instance()->a_destinations.begin_args()),
             i_end(CommandLine::get_instance()->a_destinations.end_args()) ;
             i != i_end ; ++i)
            resume_command = resume_command + " --" + CommandLine::get_instance()->a_destinations.long_name()
                    + " '" + *i + "'";

    return resume_command;
}

namespace pertubis
{
    class Install :
            public ConsoleInstallTask
    {
        private:
            tr1::shared_ptr<Environment>    _env;
            DatabaseView*                   _mainview;
            bool                            _pretend;

        public:
            OurInstallTask(tr1::shared_ptr<Environment> env, const DepListOptions & options,
                        tr1::shared_ptr<const DestinationsSet> destinations) :
                ConsoleInstallTask(env.get(), options, destinations),
                                _env(env)
            {
            }

            virtual bool want_full_install_reasons() const
            {
                return false;
            }

            virtual bool want_tags_summary() const
            {
                // pretend = true/false
                return _pretend;
            }

            virtual bool want_install_reasons() const
            {
                return false;
            }

            virtual bool want_unchanged_use_flags() const
            {
                return true;
            }

            virtual bool want_changed_use_flags() const
            {
                return false;
            }

            virtual bool want_new_use_flags() const
            {
                return false;
            }

            virtual bool want_use_summary() const
            {
                return false;
            }

            virtual void on_installed_paludis()
            {
                std::string r(stringify(_env->root()));
                std::string exec_mode(getenv_with_default("PALUDIS_EXEC_PALUDIS", ""));

                if ("always" != exec_mode)
                {
                    if ("never" == exec_mode)
                        return;
                    else if (! (r.empty() || r == "/"))
                        return;
                }

                std::string resume_command(make_resume_command(_env.get(), *this, *packages_not_yet_installed_successfully()));

                output_heading("Paludis has just upgraded Paludis");
                output_starred_item("Using '" + resume_command + "' to start a new Paludis instance...");
                output_endl();

                execl("/bin/sh", "sh", "-c", resume_command.c_str(), static_cast<const char *>(0));
            }

            virtual HookResult perform_hook(const Hook & hook) const
            {
                return ConsoleInstallTask::perform_hook(hook("RESUME_COMMAND", make_resume_command(
                        _env.get(), *this, *packages_not_yet_installed_successfully())));
            }

            void show_resume_command() const
            {
                if (CommandLine::get_instance()->a_fetch.specified() ||
                    CommandLine::get_instance()->a_pretend.specified())
                    return;

                const tr1::shared_ptr<const PackageIDSequence> p(packages_not_yet_installed_successfully());
                if (! p->empty())
                {
                    std::string resume_command(make_resume_command(environment(), *this, *p));

                    if (CommandLine::get_instance()->a_resume_command_template.specified())
                    {
                        std::string file_name(CommandLine::get_instance()->a_resume_command_template.argument());
                        int fd;
                        if (std::string::npos == file_name.find("XXXXXX"))
                            fd = open(file_name.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
                        else
                        {
                            char * resume_template = strdup(file_name.c_str());
                            fd = mkstemp(resume_template);
                            file_name = resume_template;
                            std::free(resume_template);
                        }

                        if (-1 != fd)
                        {
                                ::fchmod(fd, 0644);
                                FDOutputStream resume_command_file(fd);
                                resume_command_file << resume_command << endl;

                                if (resume_command_file)
                                {
                                    cerr << endl;
                                    cerr << "Resume command saved to file: " << file_name;
                                    cerr << endl;
                                }
                                else
                                {
                                    cerr << "Resume command NOT saved to file: " << file_name << " due to error "
                                            << strerror(errno) << endl;
                                    cerr << "Resume command: " << file_name << endl;
                                }
                        }
                        else
                        {
                            cerr << "Resume command NOT saved to file: " << file_name << " due to error "
                                    << strerror(errno) << endl;
                            cerr << "Resume command: " << file_name << endl;
                        }
                    }
                    else
                    {
                        cerr << endl;
                        cerr << "Resume command: " << resume_command << endl;
                    }
                }
            }
    };
}

#endif
