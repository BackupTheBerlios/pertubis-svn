
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_DEINSTALL_TASK_H
#define _PERTUBIS_ENTRY_PROTECTOR_DEINSTALL_TASK_H 1

#include "Task.hh"

namespace pertubis
{
    class Package;
    class PackageDeinstallTask;

    /*! \brief installs packages
     * \ingroup Selection
     * This threaded class controlls all aspects and steps of installing packages
     * \todo this thread blocks the main application thread and gui. Find the issue!
     */
    class DeinstallTask : public Task
    {
        Q_OBJECT

        public:

            ///@name Constructors
            ///@{

            /// std constructor
            DeinstallTask(QObject* pobject,
                        QAction* myaction,
                        QString tname) : Task(pobject,myaction,tname),m_task(0) {}

            ///@}

            /// @name Content information
            ///@{

            /// returns if its not masked
            bool available(Package* item) const;

            /*! @brief makes the actual changes in this object and in the Package object when a user visually changes the selection state
             * @param item the Package to be changed
             * @param mystate the new selection state for this Task
             */
            bool changeStates(Package* item, int mystate);
            ///@}

            /// @name Content modification
            ///@{

            /// starts m_task
            void startTask(const paludis::tr1::shared_ptr<paludis::Environment>& env, Settings* settings, MessageOutput* output);
            ///@}

        signals:

            /// says "I and the paludis install tasks are finished"
            void finished();

        private:

            PackageDeinstallTask* m_task;
    };
}

#endif
