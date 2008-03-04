
/* Copyright (C) 2007-2008 Stefan Koegl
*
* This file is part of perturbis
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

using namespace perturbis;

void
SystemReport::on_report_all_pre()
{
    // qDebug() << "SystemReport::on_report_all_pre()";
}

void
SystemReport::on_report_check_package_pre(const QualifiedPackageName &)
{
    // qDebug() << "SystemReport::on_report_check_package_pre()";
}

void
SystemReport::on_report_package_success(const tr1::shared_ptr<const PackageID> &)
{
    // qDebug() << "SystemReport::on_report_package_success()";
}

void
SystemReport::run()
{
    // qDebug() << "SystemReport::run start";
    if (!execMode())
        return;
    if (!execMode())
        return;
    try
    {
        // qDebug() << "SystemReport::run() 1";
        execute();
        // qDebug() << "SystemReport::run() 2";
    }
    catch(const paludis::ConfigFileError& h)
    {
        // qDebug() << "SystemReport::run() error";
        // qDebug() << h.message().c_str() << QString(h.what());
        emit finished(0,0);
    }
    catch(...)
    {
        // qDebug() << "SystemReport::run() error";
        qFatal("uncatched error");
    }
    // qDebug() << "SystemReport::run() done";
}

void
SystemReport::on_report_package_failure_pre(const tr1::shared_ptr<const PackageID> & id)
{
    // qDebug() << "SystemReport::on_report_package_failure_pre() start";
    QVector<QVariant> data(rpho_last+1);
    data[rpho_deinstall] = Qt::Checked;
    data[rpho_package] = QString::fromStdString(paludis::stringify(id->name().package));
    data[rpho_category] = QString::fromStdString(paludis::stringify(id->name().category));
    data[rpho_version] = QString::fromStdString(paludis::stringify(id->version()));
    m_node = new ReportPackage(id,data,tk_normal);
    m_dSelections->addEntry(id);
    emit appendPackage(m_node);
    // qDebug() << "SystemReport::on_report_package_failure_pre() done";
}

void
SystemReport::on_report_package_is_masked(const tr1::shared_ptr<const PackageID> &id,
        const tr1::shared_ptr<const PackageID> & origin)
{
    // qDebug() << "SystemReport::on_report_package_is_masked() start";
    QString tmp;
    for (PackageID::MasksConstIterator m(origin->begin_masks()), m_end(origin->end_masks()) ;
         m != m_end ; ++m)
    {
        tmp+= stringify((*m)->description()).c_str();
    }
    QVector<QVariant> data(3);
    data[rpho_deinstall] = Qt::Unchecked;
    data[rpho_package] = tr("masked by");
    data[rpho_category] = tmp;
    m_node->appendChild(new ReportPackage(id,data,tk_masked));
    ++m_errorCount;
    // qDebug() << "SystemReport::on_report_package_is_masked() done";
}

void
SystemReport::on_report_package_is_vulnerable_pre(const tr1::shared_ptr<const PackageID> &)
{
    // qDebug() << "SystemReport::on_report_package_is_vulnerable_pre() done";
}

void
SystemReport::on_report_package_is_vulnerable(const tr1::shared_ptr<const PackageID> &id, const GLSADepTag & tag)
{
//     // qDebug() << "SystemReport::on_report_package_is_vulnerable() start";
    // last column will not be visible so it's a good place to put the glsa file path
    QVector<QVariant> data(rpho_last+1);
    data[rpho_deinstall] = Qt::Unchecked;
    data[rpho_package] = tr("vulnerable");
    data[rpho_category] = QString::fromStdString(tag.short_text());
    data[rpho_last] = QString::fromStdString(paludis::stringify(tag.glsa_file()));
    m_node->appendChild(new ReportPackage(id,data,tk_glsa));
//     emit notifyAboutGLSA(QString::fromStdString(tag.short_text()),QString::fromStdString(paludis::stringify(tag.glsa_file())));
    ++m_errorCount;
//     // qDebug() << "SystemReport::on_report_package_is_vulnerable() start";
}

void
SystemReport::on_report_package_is_vulnerable_post(const tr1::shared_ptr<const PackageID> &)
{
}

void
SystemReport::on_report_package_is_missing(const tr1::shared_ptr<const PackageID> &id,
        const RepositoryName & repo_name)
{
    // // qDebug() << "SystemReport::on_report_package_is_missing() start";
    QVector<QVariant> data(rpho_last+1);
    data[rpho_deinstall] = Qt::Unchecked;
    data[rpho_package] = tr("deleted in repository");
    data[rpho_category] = QString::fromStdString(paludis::stringify(repo_name));
    m_node->appendChild(new ReportPackage(id,data,tk_error));
    ++m_errorCount;
    // qDebug() << "SystemReport::on_report_package_is_missing() start";
}

void
SystemReport::on_report_package_is_unused(const tr1::shared_ptr<const PackageID> &id)
{
    // qDebug() << "SystemReport::on_report_package_is_unused() start";
    QVector<QVariant> data(rpho_last+1);
    data[rpho_deinstall] = Qt::Unchecked;
    data[rpho_package] = tr("package is unused");
    m_node->appendChild(new ReportPackage(id,data,tk_error));
    ++m_errorCount;
    // qDebug() << "SystemReport::on_report_package_is_unused() done";
}

void
SystemReport::on_report_package_failure_post(const tr1::shared_ptr<const PackageID> &)
{
}

void
SystemReport::on_report_check_package_post(const QualifiedPackageName &)
{
    ++m_totalCount;
}

void
SystemReport::on_report_all_post()
{
    // qDebug() << "SystemReport::on_report_all_post() start";
    emit finished(m_totalCount, m_errorCount);
    // qDebug() << "SystemReport::on_report_all_post() done";
}
