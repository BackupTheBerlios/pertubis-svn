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
#include <QtGlobal>
#include <QTextEdit>
#include <paludis/syncer.hh>
#include <iomanip>
#include <iostream>
#include <string>

void pertubis::OurSyncTask::on_sync_all_pre()
{
}

void pertubis::OurSyncTask::on_sync_pre(const paludis::RepositoryName & r)
{
    m_output->append(QString::fromStdString(color(stringify(r),"magenta")));
}

void pertubis::OurSyncTask::on_sync_post(const paludis::RepositoryName &)
{
}

void pertubis::OurSyncTask::on_sync_skip(const paludis::RepositoryName & r)
{
    m_output->append(QString("Sync %1 skipped").arg(paludis::stringify(r).c_str()));
}

void pertubis::OurSyncTask::on_sync_succeed(const paludis::RepositoryName & r)
{
    m_output->append(QString("Sync %1 completed").arg(paludis::stringify(r).c_str()));
}

void pertubis::OurSyncTask::on_sync_fail(const paludis::RepositoryName & r, const paludis::SyncFailedError & e)
{
    _return_code |= 1;
    qWarning("Sync error: * %s %s",e.backtrace("\n  * ").c_str(), e.message().c_str());
}

void pertubis::OurSyncTask::on_sync_all_post()
{
    m_output->append("");
}
