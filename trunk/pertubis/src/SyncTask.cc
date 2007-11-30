/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2006, 2007 Ciaran McCreesh <ciaranm@ciaranm.org>
 *
 * This file is part of the Paludis package manager. Paludis is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * Paludis is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "SyncTask.hh"
#include "FormatterUtils.hh"
#include <paludis/util/stringify.hh>
#include <paludis/syncer.hh>
#include <iomanip>
#include <iostream>
#include <string>

void pertubis::PertubisSyncTask::on_sync_all_pre()
{
}

void pertubis::PertubisSyncTask::on_sync_pre(const paludis::RepositoryName & r)
{
    sendMessage(QString::fromStdString(color(stringify(r),"magenta")));
}

void pertubis::PertubisSyncTask::on_sync_post(const paludis::RepositoryName &r)
{
    sendMessage(QString("after syncing %1").arg(QString::fromStdString(color(stringify(r),"magenta"))));
}

void pertubis::PertubisSyncTask::on_sync_skip(const paludis::RepositoryName & r)
{
    sendMessage(QString("Sync %1 skipped").arg(stringify(r).c_str()));
}

void pertubis::PertubisSyncTask::on_sync_succeed(const paludis::RepositoryName & r)
{
    sendMessage(QString("Sync %1 completed").arg(stringify(r).c_str()));
}

void pertubis::PertubisSyncTask::on_sync_fail(const paludis::RepositoryName & /*r*/, const paludis::SyncFailedError & e)
{
    m_return_code |= 1;
    qWarning("Sync error: * %s %s",e.backtrace("\n  * ").c_str(), e.message().c_str());
}

void pertubis::PertubisSyncTask::on_sync_all_post()
{
    sendMessage("");
}
