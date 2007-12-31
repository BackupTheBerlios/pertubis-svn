
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

#include <QMutexLocker>
#include "SystemReport.hh"
#include "ReportPackage.hh"
#include "Selections.hh"
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
    QMutexLocker locker(&m_paludisAccess);
    try
    {
        execute();
    }
    catch(const paludis::ConfigFileError& h)
    {
        qDebug() << h.message().c_str() << QString(h.what());
        emit finished(0,0);
    }
    catch(...)
    {
        qFatal("uncatched error");
    }
}

void pertubis::SystemReport::on_report_package_failure_pre(const tr1::shared_ptr<const PackageID> & id)
{
    QVariantList list;
    list <<
            true <<
            QString::fromStdString(paludis::stringify(id->name().package)) <<
            QString::fromStdString(paludis::stringify(id->name().category)) <<
            QString::fromStdString(paludis::stringify(id->version()));
    m_node = new ReportPackage(id,list);
    m_dSelections->addEntry(id);
    m_node->setData(ReportPackage::ro_deinstall,m_dSelections->hasEntry(id));
    emit appendPackage(m_node);
}

void pertubis::SystemReport::on_report_package_is_masked(const tr1::shared_ptr<const PackageID> &id,
        const tr1::shared_ptr<const PackageID> & origin)
{
    QString tmp;
    for (PackageID::MasksConstIterator m(origin->begin_masks()), m_end(origin->end_masks()) ;
         m != m_end ; ++m)
    {
        tmp+= stringify( (*m)->description()).c_str();
    }
    m_node->addTag(new ReportPackage(id,QList<QVariant>() << false << tr("masked by") << tmp,true));
    ++m_errorCount;
}

void pertubis::SystemReport::on_report_package_is_vulnerable_pre(const tr1::shared_ptr<const PackageID> &)
{
}

void pertubis::SystemReport::on_report_package_is_vulnerable(const tr1::shared_ptr<const PackageID> &id, const GLSADepTag & tag)
{
    m_node->addTag(new ReportPackage(id,QList<QVariant>() << false << tr("vulnerable") << QString::fromStdString(tag.short_text()),true));
    emit notifyAboutGLSA(QString::fromStdString(tag.short_text()),QString::fromStdString(paludis::stringify(tag.glsa_file())));
    ++m_errorCount;
}

void pertubis::SystemReport::on_report_package_is_vulnerable_post(const tr1::shared_ptr<const PackageID> &)
{
}

void pertubis::SystemReport::on_report_package_is_missing(const tr1::shared_ptr<const PackageID> &id,
        const RepositoryName & repo_name)
{
    m_node->addTag(new ReportPackage(id,QList<QVariant>() << false << tr("deleted in repository") <<  QString::fromStdString(stringify(repo_name)),true));
    ++m_errorCount;
}

void pertubis::SystemReport::on_report_package_is_unused(const tr1::shared_ptr<const PackageID> &id)
{
    m_node->addTag(new ReportPackage(id,QList<QVariant>() << false << tr("package is unused"),true));
    ++m_errorCount;
}

void pertubis::SystemReport::on_report_package_failure_post(const tr1::shared_ptr<const PackageID> &)
{
}

void pertubis::SystemReport::on_report_check_package_post(const QualifiedPackageName &)
{
}

void pertubis::SystemReport::on_report_all_post()
{
    emit finished( m_totalCount, m_errorCount);
}
