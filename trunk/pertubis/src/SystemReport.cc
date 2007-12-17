
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

#include "SystemReport.hh"
#include "Package.hh"
#include "TaskBox.hh"
#include "Task.hh"
#include "PaludisUtils.hh"
#include "FormatterUtils.hh"
#include <paludis/mask.hh>
#include <paludis/dep_tag.hh>
#include <paludis/package_id.hh>
#include <paludis/util/visitor-impl.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/fs_entry.hh>
#include <paludis/util/config_file.hh>

void pertubis::SystemReport::on_report_all_pre()
{
}

void pertubis::SystemReport::on_report_check_package_pre(const QualifiedPackageName &)
{
}

void pertubis::SystemReport::on_report_package_success(const tr1::shared_ptr<const PackageID> &)
{
}

void pertubis::SystemReport::run()
{
    ThreadBase::lock();
    try
    {
        execute();
    }
    catch(const paludis::ConfigFileError& h)
    {
        qDebug() << h.message().c_str() << QString(h.what());
        emit finished(0,0);
    }
    ThreadBase::unlock();
}

void pertubis::SystemReport::on_report_package_failure_pre(const tr1::shared_ptr<const PackageID> & id)
{
    QVariantList list;
    list <<
            QVariant(m_taskbox->tasks()) <<
            QString("%1-%2").arg(QString::fromStdString(paludis::stringify(id->name().package))).arg(QString::fromStdString(paludis::stringify(id->version()))) <<
            QString::fromStdString(paludis::stringify(id->name().category)) <<
            "";
    m_node = new Package(id,list,Package::ps_stable,Package::pt_node_only,0);
    m_taskbox->task(tr("deinstall"))->addEntry(id);
    m_taskbox->setTasksInPackage(m_node);
    emit appendPackage(m_node);
}

void pertubis::SystemReport::on_report_package_is_masked(const tr1::shared_ptr<const PackageID> & id,
        const tr1::shared_ptr<const PackageID> & origin)
{
    QString tmp("masked by repository: ");
    for (PackageID::MasksConstIterator m(origin->begin_masks()), m_end(origin->end_masks()) ;
         m != m_end ; ++m)
    {
        tmp+= stringify( (*m)->description()).c_str();
    }

    QString old = m_node->data(rho_reasons).toString();
    if (!old.isEmpty())
        old.append(QString("\n"));
    old.append( tmp);
    m_node->setData(rho_reasons,old);
    ++_n_errors;
}

void pertubis::SystemReport::on_report_package_is_vulnerable_pre(const tr1::shared_ptr<const PackageID> &id)
{
    QString old = m_node->data(rho_reasons).toString();
    if (!old.isEmpty())
        old.append(QString("\n"));
    old.append("vulnerable:");
    m_node->setData(rho_reasons,old);
    ++_n_errors;
}

void pertubis::SystemReport::on_report_package_is_vulnerable(const tr1::shared_ptr<const PackageID> &id, const GLSADepTag & tag)
{
    QString old(m_node->data(rho_reasons).toString());
    old.append(QString(" ")+QString::fromStdString(tag.short_text()));
    m_node->setData(rho_reasons,old);
    emit notifyAboutGLSA(QString::fromStdString(tag.short_text()),QString::fromStdString(paludis::stringify(tag.glsa_file())));
    ++_n_errors;
}

void pertubis::SystemReport::on_report_package_is_vulnerable_post(const tr1::shared_ptr<const PackageID> &)
{
    QString old = m_node->data(rho_reasons).toString();
    old.append(QString("\n"));
    m_node->setData(rho_reasons,old);
}

void pertubis::SystemReport::on_report_package_is_missing(const tr1::shared_ptr<const PackageID> &id,
        const RepositoryName & repo_name)
{
    QString old(m_node->data(rho_reasons).toString());
    if (!old.isEmpty())
        old.append(QString("\n"));
    old.append(tr("no longer existing in repository `%1'").arg(stringify(repo_name).c_str()));
    m_node->setData(rho_reasons,old);
    ++_n_errors;
}

void pertubis::SystemReport::on_report_package_is_unused(const tr1::shared_ptr<const PackageID>& id)
{
    QString old(m_node->data(rho_reasons).toString());
    old.append("\nunused");
    m_node->setData(rho_reasons,old);
    ++_n_errors;
}

void pertubis::SystemReport::on_report_package_failure_post(const tr1::shared_ptr<const PackageID> &id)
{
}

void pertubis::SystemReport::on_report_check_package_post(const QualifiedPackageName &)
{
    ++_n_packages;
}

void pertubis::SystemReport::on_report_all_post()
{
    emit finished(_n_packages, _n_errors);
}
