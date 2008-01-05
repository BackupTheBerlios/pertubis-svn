
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

#include "SyncTask.hh"
#include "FormatterUtils.hh"
#include <paludis/util/stringify.hh>
#include <paludis/syncer.hh>
#include <iomanip>
#include <iostream>
#include <QMutexLocker>
#include <string>

void pertubis::PertubisSyncTask::on_sync_all_pre()
{
}

void pertubis::PertubisSyncTask::on_sync_pre(const paludis::RepositoryName & r)
{
    sendMessage(QString("Sync %1 started").arg(QString::fromStdString(color(stringify(r),"magenta"))));
}

void pertubis::PertubisSyncTask::on_sync_post(const paludis::RepositoryName &r)
{
    sendMessage(QString("Sync %1 finished").arg(QString::fromStdString(color(stringify(r),"magenta"))));
}

void pertubis::PertubisSyncTask::on_sync_skip(const paludis::RepositoryName & r)
{
    sendMessage(QString("Sync %1 skipped").arg(QString::fromStdString(color(stringify(r),"magenta"))));
}

void pertubis::PertubisSyncTask::on_sync_succeed(const paludis::RepositoryName & r)
{
    sendMessage(QString("Sync %1 completed").arg(stringify(r).c_str()));
}

void pertubis::PertubisSyncTask::on_sync_fail(const paludis::RepositoryName & /*r*/, const paludis::SyncFailedError & e)
{
    sendMessage(color(QString("Sync error: * %1 %2\n").arg(e.backtrace("<br>  * ").c_str()).arg(e.message().c_str()),"red"));
}

void pertubis::PertubisSyncTask::on_sync_all_post()
{
    sendMessage("Sync complete");
}

void pertubis::PertubisSyncTask::start(const QSet<QString>& repos)
{
    for (QSet<QString>::const_iterator rStart(repos.constBegin()),rEnd(repos.constEnd());
         rStart != rEnd; ++rStart)
         add_target(rStart->toStdString());
    QThread::start();
}

void pertubis::PertubisSyncTask::run()
{
    QMutexLocker locker(&m_paludisAccess);
    try
    {
        execute();
    }
    catch(...)
    {
        qDebug() << "error occured while syncing";
    }
    emit finished();
}
