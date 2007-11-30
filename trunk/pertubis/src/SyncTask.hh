
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
*
* This file is part of the pertubis frontend for paludis package manager.
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SYNC_TASK_H
#define _PERTUBIS_ENTRY_PROTECTOR_SYNC_TASK_H 1

#include <QThread>
#include <paludis/sync_task.hh>
#include <paludis/util/tr1_memory.hh>

namespace pertubis
{
    /*! \brief syncing repositories
     * \ingroup PaludisAdapter
     * \ingroup Thread
     */
    class PertubisSyncTask :
        public QThread,
        public paludis::SyncTask
    {
        Q_OBJECT

        public:

            ///\name Constructors
            ///\{

            /// constructs a PertubisSyncTask object
            PertubisSyncTask(paludis::tr1::shared_ptr<paludis::Environment> env, QObject* pobj) :
                QThread(pobj),
                SyncTask(env.get(),true),
                m_return_code(0)
            {
            }
            ///@}

            /// std destructor
            ~PertubisSyncTask() {}

            ///@name Callback methods
            ///@{
            /// overloaded from paludis::SyncTask
            virtual void on_sync_all_pre();
            virtual void on_sync_pre(const paludis::RepositoryName &);
            virtual void on_sync_post(const paludis::RepositoryName &);
            virtual void on_sync_skip(const paludis::RepositoryName &);
            virtual void on_sync_fail(const paludis::RepositoryName &, const paludis::SyncFailedError &);
            virtual void on_sync_status(int, int, int) {}
            virtual void on_sync_succeed(const paludis::RepositoryName &);
            virtual void on_sync_all_post();
            ///@}

            /// returns the return code of paludis
            int return_code() const  { return m_return_code; }

        protected:

            /// overloaded from QThread
            void run() { execute();}

        signals:

            /// sends a status message from paludis api to the main thread
            void sendMessage(QString message);

        private:

            /// the actual return code
            int             m_return_code;
    };
}

#endif
