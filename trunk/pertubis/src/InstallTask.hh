
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

#include "Task.hh"

namespace pertubis
{
    class InstallTask : public Task
    {
        Q_OBJECT

    public:

        InstallTask() {}
        InstallTask(QObject* pobject,
                    QAction* myaction,
                      QString tname) : Task(pobject,myaction,tname) {}
        bool available(Item* item) const;
        bool changeChildStates(Item* item, bool mystate);
        bool changeParentStates(Item* item, bool mystate);
    };
}

#endif
