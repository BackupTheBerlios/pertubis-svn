
/* Copyright (C) 2007-2008 Stefan Koegl
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_TASK_QUEUE_H
#define _PERTUBIS_ENTRY_PROTECTOR_TASK_QUEUE_H 1

#include <QObject>

namespace pertubis
{

    class MainWindow;

    /** \brief handles and times the execution of tasks, which interface with the paludis api
     *
     *
     * \ingroup PaludisAdapter
     * \ingroup Threads
     * \warning intermediate class since starting from qt4.4 there will be a thread handler
     * \see ThreadBase, CategoryThread, DetailsThread, PackagesThread, SearchThread, SetThread
     */
    class TaskQueue : public QObject
    {
        Q_OBJECT

        public:

            /// std constructor
            TaskQueue(QObject* main);

            /// std destructor
            ~TaskQueue();

            /** \brief adds a task
             *
             * adds a task \p thread to the queue, and starts it if \p start is true
             * \see dequeue()
             */
            void enqueue(QThread* thread, bool start=false);

            /** \brief removes the actual task from the queue and deletes it
             *
             *
             * \see enqueue()
             */
            void dequeue();

            /** \brief starts the next task in queue if present, if not it emits finished()
             *
             *
             * \see running() , finished()
             */
            void next();


            /** \brief returns true if a thread in the queue and is running
             *
             */
            bool running();

        signals:

            /// will be emitted if the queue has no more running tasks
            void finished();

        public slots:

            /** handles the lifetime of a task
             *
             * \see next() , dequeue()
             */
            void process();

        private:

            struct TaskQueuePrivate;
            TaskQueuePrivate* const      m_imp;
    };
}

#endif
