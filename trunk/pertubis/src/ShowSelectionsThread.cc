
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

#include "ShowSelectionsThread.hh"
#include "TaskBox.hh"
#include "Task.hh"
#include "Package.hh"
#include "PaludisUtils.hh"
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
    int count(0);
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
                    ( installed(m_env,*idStart) ? Qt::PartiallyChecked : Qt::Unchecked) <<
                    "" <<
                    "";

            qDebug() << "pertubis::ShowSelectionsThread::run() - 1";
            Package* node = new Package(*idStart,list,Package::ps_stable,Package::pt_node_only,0);
            qDebug() << "pertubis::ShowSelectionsThread::run() - 2";
            m_taskbox->setTasksInPackage(node);
            qDebug() << "pertubis::ShowSelectionsThread::run() - 3";
            emit appendPackage(node);
            ++count;
        }
    }
    emit finished(count);
    qDebug() << "pertubis::ShowSelectionsThread::run() - done";
}
