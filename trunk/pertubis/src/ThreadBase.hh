
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_THREAD_BASE_H
#define _PERTUBIS_ENTRY_PROTECTOR_THREAD_BASE_H 1

#include <QThread>
#include <QMutex>

#include <paludis/util/tr1_memory.hh>
#include <paludis/environment-fwd.hh>
#include <paludis/package_id-fwd.hh>

namespace pertubis
{
    class TaskBox;

    /*! \brief provides the paludis infrastructure we need for the threads
     *
     * \ingroup PaludisAdapter
     * \ingroup Threads
     */
    class ThreadBase : public QThread
    {
        Q_OBJECT

    public:

        ///\name Constructors
        ///\{

        /// std constructor
        ThreadBase(QObject* parent,
                    const paludis::tr1::shared_ptr<paludis::Environment>&  env,
                    TaskBox* box);
        ///\}

        /// tries to lock the mutex and returns the result
        static bool tryLock();

        /// tries to lock the mutex and returns the result
        static void lock();

        /// tries to lock the mutex and returns the result
        static void unlock();

        /// request cancel thread execution
        void stopExec() { m_stopExec=true;}

    protected:

        /// the environment
        paludis::tr1::shared_ptr<paludis::Environment>  m_env;

        /// ptr to TaskBox
        TaskBox*                                        m_taskbox;

        /// execution will be stopped, if true
        bool                                            m_stopExec;

        static QMutex m_paludisAccess;
    };
}

#endif
