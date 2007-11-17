
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

#include "ShowSelectionsThread.hh"
#include "TaskBox.hh"
#include "DatabaseView.hh"
#include "Task.hh"
#include "Item.hh"

#include <QVector>
#include <QVariant>
#include <paludis/util/stringify.hh>
#include <paludis/util/tr1_memory.hh>
#include <paludis/package_id.hh>
#include <paludis/name.hh>
#include <paludis/repository.hh>

void pertubis::ShowSelectionsThread::run()
{
    for (TaskBox::Iterator tStart(m_main->taskbox()->taskBegin()),
         tEnd(m_main->taskbox()->taskEnd());
         tStart != tEnd;
         ++tStart)
    {
        for (Task::Iterator idStart( (*tStart)->data().begin()),
             idEnd((*tStart)->data().end());
             idStart != idEnd;
             ++idStart)
        {
            QString repo(QString::fromStdString(paludis::stringify((*idStart)->repository()->name())));
            QVariantList list;
            list <<
                    QVariant(m_main->taskbox()->selectionData(*idStart)) <<
                    QString::fromStdString(paludis::stringify((*idStart)->name().package)).append("-"+QString::fromStdString(paludis::stringify((*idStart)->version()))) <<
                    QString::fromStdString(paludis::stringify((*idStart)->name().category)) <<
                    QString::fromStdString(paludis::stringify((*idStart)->repository()->name())) <<
                    ( installed(*idStart) ? Qt::Checked : Qt::Unchecked) <<
                    "" <<
                    "";
            emit appendPackage(new Item(*idStart,list,Item::is_stable,Item::ur_node,0));
        }
    }
}
