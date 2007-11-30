
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_INSTALL_TASK_H
#define _PERTUBIS_ENTRY_PROTECTOR_INSTALL_TASK_H 1

#include "Task.hh"

namespace pertubis
{
    class Install;

    /*! \brief stores the PackageIDs of an Item if it is about to be installed and changes the items' visual state
    * \ingroup Selection
    */
    class InstallTask : public Task
    {
        Q_OBJECT

    public:

        /// @name Constructors
        ///@{


        /// std constructor
        InstallTask(QObject* pobject,
                    QAction* myaction,
                    QString tname);
        ///@}

        /// @name Content modification
        ///@{

            /// returns if its not masked
        bool available(Item* item) const;

        /*! @brief makes the actual changes in this object and in the Item object when a user visually changes the selection state
         * @param item the Item to be changed
         * @param mystate the new selection state for this Task
        */
        bool changeStates(Item* item, int mystate);
        ///@}

        /// @name Content modification
        ///@{

        /// starts m_task
        void startTask(const paludis::tr1::shared_ptr<paludis::Environment>& env,MessageOutput* output);
            ///@}

    signals:

        /// says "I and the paludis install tasks are finished"
        void finished();

    private slots:

        /// forwards a QThread::finished() signal
        void slotFinished();

    private:

        /// the paludis install task
        Install* m_task;
    };
}

#endif
