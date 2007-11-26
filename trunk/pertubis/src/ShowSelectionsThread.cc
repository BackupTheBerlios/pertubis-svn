
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
#include "Task.hh"
#include "Item.hh"
#include <QDebug>
#include <QVector>
#include <QVariant>
#include <paludis/util/stringify.hh>
#include <paludis/util/tr1_memory.hh>
#include <paludis/package_id.hh>
#include <paludis/name.hh>
#include <paludis/repository.hh>

void pertubis::ShowSelectionsThread::run()
{
    qDebug() << "pertubis::ShowSelectionsThread::run()";
    for (TaskBox::Iterator tStart(m_taskbox->taskBegin()),
         tEnd(m_taskbox->taskEnd());
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
                    QVariant(m_taskbox->tasks()) <<
                    QString("%1-%2").arg(QString::fromStdString(paludis::stringify((*idStart)->name().package))).arg(QString::fromStdString(paludis::stringify((*idStart)->version()))) <<
                    QString::fromStdString(paludis::stringify((*idStart)->name().category)) <<
                    QString::fromStdString(paludis::stringify((*idStart)->repository()->name())) <<
                    ( installed(*idStart) ? Qt::PartiallyChecked : Qt::Unchecked) <<
                    "" <<
                    "";

            qDebug() << "pertubis::ShowSelectionsThread::run() - 1";
            Item* pitem=0;
            try
            {
                pitem = new Item(*idStart,list,Item::is_stable,Item::ur_node,0);
            }
            catch(std::bad_alloc)
            {
                qDebug() << "pertubis::ShowSelectionsThread::run() - new error";
                continue;
            }
            qDebug() << "pertubis::ShowSelectionsThread::run()" << *pitem;
            qDebug() << "pertubis::ShowSelectionsThread::run() - 2";
            m_taskbox->setTasksInItem(pitem);
            qDebug() << "pertubis::ShowSelectionsThread::run() - 3";

            emit appendPackage(pitem);
        }
    }
    qDebug() << "pertubis::ShowSelectionsThread::run() - done";
}
