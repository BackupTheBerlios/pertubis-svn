
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_REPORT_PACKAGE_H
#define _PERTUBIS_ENTRY_PROTECTOR_REPORT_PACKAGE_H 1

#include "Package.hh"
#include <QVector>
#include <QDebug>
#include <QString>
#include <QVariant>
#include <QPair>
#include <paludis/util/attributes.hh>
#include <paludis/util/tr1_memory.hh>
#include <paludis/package_id-fwd.hh>


/** \enum pertubis::ReportPackage::ReportPackageHeaderOrder
 * The order of clolumns in pertubis::ReportPackage::m_data and column order in the package view
 */

/** \enum pertubis::ReportPackage::ReportPackageHeaderOrder pertubis::ReportPackage::ro_deinstall
 * QVariant<int>. add to deinstall task
 */

/** \enum pertubis::ReportPackage::ReportPackageHeaderOrder pertubis::ReportPackage::ro_package
 * QVariant<QString>. paludis::PackagePartName
 */

/** \enum pertubis::ReportPackage::ReportPackageHeaderOrder pertubis::ReportPackage::ro_category
 * QVariant<QString>. paludis::CategoryPartName
 */

/** \enum pertubis::ReportPackage::ReportPackageHeaderOrder pertubis::ReportPackage::ro_version
 * QVariant<QString>. paludis::Version
 */

/** \enum pertubis::ReportPackage::ReportPackageHeaderOrder pertubis::ReportPackage::ro_last
 * number of named columns
 */

namespace pertubis
{
    /** \brief package information / metadata storage
     *
     * \ingroup PackageModelClass
     * The SystemReport uses this ReportPackage class for storing the items with issues found.
     *
     */

    enum TagKind
    {
        tk_normal=0,
        tk_error,
        tk_block,
        tk_masked,
        tk_glsa
    };

    enum ReportPackageHeaderOrder
    {
        rpho_deinstall=0,
        rpho_package,
        rpho_category,
        rpho_version,
        rpho_last
    };

    class ReportPackage : public Package
    {
        public:

            ReportPackage(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                const QVector<QVariant> &dats,
                TagKind mytag)  :
                Package(id,
                        dats,
                        ps_stable,
                        pt_node_only,
                        0),
                m_tag(mytag)
            {
            }

            /// destructor
            virtual ~ReportPackage();

             ///@name Content modification
            ///@{

            TagKind tag() const { return m_tag;}

        protected:

            TagKind            m_tag;
    };
}

QDebug operator<<(QDebug dbg, const pertubis::ReportPackage &c);

#endif
