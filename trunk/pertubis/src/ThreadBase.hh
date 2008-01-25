
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

#include <paludis/util/tr1_memory.hh>
#include <paludis/environment-fwd.hh>
#include <paludis/package_id-fwd.hh>

namespace pertubis
{

    /*! \brief provides the paludis infrastructure we need for the threads
     *
     * \ingroup PaludisAdapter
     * \ingroup Threads
     * \see TaskQueue, ThreadBase, CategoryThread, DetailsThread, PackagesThread, SearchThread, SetThread
     */
    class ThreadBasePrivate;

    class ThreadBase : public QThread
    {
        Q_OBJECT

    public:

        ///\name Constructors
        ///\{

        /// std constructor
        ThreadBase(QObject* pobj,
            const paludis::tr1::shared_ptr<paludis::Environment>&  env);
        ///\}

        /// destructor
        ~ThreadBase();

        paludis::tr1::shared_ptr<paludis::Environment> env();

        /** \brief returns true if this thread should be running/executing.
         *
         * It's not the real state of the thread, which can be polled by QTHread::isRunning() or QTHread::isFinished().
         * It's only a hint for a QThread:.run() implementation, which should be stopped.
         * \code
         * MyThread : public ThreadBase
         * ...
         * void run()
         * {
         *      while (execMode() )
         *      {
         *          // process
         *      }
         * }
         * \endcode
         * \see setExecMode()
         */
        bool execMode();

        /** \brief start or stop the thread
         *
         * It depends on the actual implementation, if this is used.
         * If possible ask as mch as possible in QThread::run() possible to use this inconjunction with execMode()
         * \see execMode()
         */
        void setExecMode(bool mode);

    private:
        ThreadBasePrivate* const m_imp;
    };
}

#endif
