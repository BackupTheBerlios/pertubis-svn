
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_DEINSTALL_TASK_H
#define _PERTUBIS_ENTRY_PROTECTOR_DEINSTALL_TASK_H

#include "Task.hh"


namespace pertubis
{

    class Item;

    class DeinstallTask : public Task
    {
        Q_OBJECT

    public:

        DeinstallTask() {}
        DeinstallTask(QObject* pobject,
                    QAction* myaction,
                      QString tname) : Task(pobject,myaction,tname) {}
        bool available(Item* item) const;
        bool changeParentStates(Item* item, bool mystate);
        bool changeChildStates(Item* item, bool mystate);
    };
}

#endif
