
/* Copyright (C) 2007 Stefan Koegl
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

#include "SyncTask.hh"
#include <paludis/util/stringify.hh>
#include <paludis/syncer.hh>
#include <iostream>
#include <string>

using namespace pertubis;

using namespace pertubis;

/*! \brief syncing repositories
 * \ingroup PaludisAdapter
 * \ingroup Thread
 */
namespace pertubis
{
    class PertubisSyncTask :
        public ThreadBase,
        public paludis::SyncTask
    {
        Q_OBJECT

        public:

            ///\name Constructors
            ///\{

            /// constructs a PertubisSyncTask object
            PertubisSyncTask(paludis::tr1::shared_ptr<paludis::Environment> myenv, QObject* pobj) :
                ThreadBase(pobj,paludis::tr1::shared_ptr<paludis::Environment>()),
                           SyncTask(myenv.get(),true)
            {
            }
            ///@}

            /// std destructor
            ~PertubisSyncTask() {}

            ///@name Callback methods
            ///@{
            /// overloaded from paludis::SyncTask
            void PertubisSyncTask::on_sync_all_pre()
            {
            }

            void PertubisSyncTask::on_sync_pre(const paludis::RepositoryName & r)
            {
                std::cout << "Sync " << "\e[35;1m" << stringify(r) << "\e[0;0m" << std::endl;
            }

            void PertubisSyncTask::on_sync_post(const paludis::RepositoryName &r)
            {
                std::cout << "Sync " << "\e[35;1m" << stringify(r) << "\e[0;0m" << " finished" << std::endl;
            }

            void PertubisSyncTask::on_sync_skip(const paludis::RepositoryName & r)
            {
                std::cout << "Sync " << "\e[35;1m" << stringify(r) << "\e[0;0m" << " skipped" << std::endl;
            }

            void PertubisSyncTask::on_sync_succeed(const paludis::RepositoryName & r)
            {
                std::cout << "Sync "<< "\e[35;1m" << stringify(r) << "\e[0;0m" << " completed" << std::endl;
            }

            void PertubisSyncTask::on_sync_fail(const paludis::RepositoryName & /*r*/, const paludis::SyncFailedError & e)
            {
                std::cout << "\e[31;1m" << "Sync error " << e.backtrace("<br>  * ") << e.message() << std::endl;
            }

            void PertubisSyncTask::on_sync_all_post()
            {
                std::cout << "Sync complete" << std::endl;
            }

            ///@}

            void PertubisSyncTask::setup(const QSet<QString>& repos)
            {
                for (QSet<QString>::const_iterator rStart(repos.constBegin()),rEnd(repos.constEnd());
                     rStart != rEnd; ++rStart)
                    add_target(rStart->toStdString());
            }

        signals:

            /// sends a status message from paludis api to the main thread
            void sendMessage(QString message);


        protected:

            /// overloaded from QThread
            void PertubisSyncTask::run()
            {
                execute();
            }
    };
}
