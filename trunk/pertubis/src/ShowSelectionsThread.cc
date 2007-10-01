
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
    Item* root = new Item();
    for (TaskBox::Iterator tStart(m_main->taskbox()->taskBegin()),
         tEnd(m_main->taskbox()->taskEnd());
         tStart != tEnd;
         ++tStart)
    {
        for (std::list<paludis::tr1::shared_ptr<const paludis::PackageID> >::const_iterator idStart( (*tStart)->data().begin()),
             idEnd((*tStart)->data().end());
             idStart != idEnd;
             ++idStart)
        {
            Item* p_item = makePackageItem(*idStart,
                            m_main->taskbox()->selectionData(*idStart),
                            QString::fromStdString(paludis::stringify((*idStart)->name().package)),
                            QString::fromStdString(paludis::stringify((*idStart)->name().category)),
                            QString::fromStdString(paludis::stringify((*idStart)->repository()->name())),
                            false,
                            Item::is_stable,
                            Item::ur_parent,
                            root,
                            "");
            Item* v_item = makeVersionItem(
                                *idStart,
                                m_main->taskbox()->selectionData(*idStart),
                                QString::fromStdString(paludis::stringify((*idStart)->version()).c_str()),
                                false,
                                Item::is_stable,
                                Item::ur_child,
                                p_item,
                                "");
            p_item->appendChild(v_item);
            root->appendChild(p_item);
        }
    }
    emit sendRoot(root);
}
