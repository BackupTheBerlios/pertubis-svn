
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

#include <QList>
#include <QDebug>
#include <QString>
#include <QVariant>
#include <QPair>
#include <paludis/util/attributes.hh>
#include <paludis/util/tr1_memory.hh>
#include <paludis/package_id-fwd.hh>


/** \enum pertubis::ReportPackage::ReportOrder
 * The order of information in pertubis::ReportPackage::m_data and column order in the package view
 */

/** \enum pertubis::ReportPackage::ReportOrder* pertubis::ReportPackage::ro_deinstall
 * QVariant<int>. add to deinstall task
 */

/** \enum pertubis::ReportPackage::ReportOrder* pertubis::ReportPackage::ro_package
 * QVariant<QString>. paludis::ReportPackagePartName
 */

/** \enum pertubis::ReportPackage::ReportOrder* pertubis::ReportPackage::ro_category
 * QVariant<QString>. paludis::CategoryPartName
 */

/** \enum pertubis::ReportPackage::ReportOrder* pertubis::ReportPackage::ro_version
 * QVariant<QString>. paludis::CategoryPartName
 */

/** \enum pertubis::ReportPackage::ReportOrder* pertubis::ReportPackage::ro_last
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
    class ReportPackage
    {
        public:

            enum ReportOrder
            {
                ro_deinstall,
                ro_package,
                ro_category,
                ro_version,
                ro_last
            };

            /// defines a const iterator used for iterating over all child Packages
            typedef QList<ReportPackage*>::const_iterator ConstReportPackageIterator;

            /// defines a const iterator used for iterating over all child Packages
            typedef QList<ReportPackage*>::iterator ReportPackageIterator;

            /// constructor
            ReportPackage();

            /// that's the usual way to create an ReportPackage object
            ReportPackage(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                 const QList<QVariant> &dats, bool isTag=false);

            /// destructor
            ~ReportPackage();

             ///@name Content modification
            ///@{

            /// appends a new child ReportPackage
            void addTag(ReportPackage* package);

            /** sets a displayable value in column
             *
             * accepts only columns less than columnCount()
             */
            void setData(int column, QVariant data);

            /// sets a new parent node / Package object
            void setParent(ReportPackage* pitem);  // test

            /// sets a new ReportPackageID
            void setID(const paludis::tr1::shared_ptr<const paludis::PackageID>& id) { m_id = id;} // test

            ///@}
            ///@name Content information
            ///@{

             /// returns the child ReportPackage of this Package object with number "row" if present, and 0 if not
            ReportPackage *child(int row) const PALUDIS_ATTRIBUTE((warn_unused_result));  // test

            /// returns the total number of displayable data in this ReportPackage object
            int columnCount() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns the displayable data in "column"
            QVariant data(int column) const PALUDIS_ATTRIBUTE((warn_unused_result));

             /// returns the row index of this Package object in parent if parent is valid, else -1
            int row() const PALUDIS_ATTRIBUTE((warn_unused_result));

             /// returns the row index of this Package object in parent if parent is valid, else -1
            int tagCount() const PALUDIS_ATTRIBUTE((warn_unused_result));

            bool isTag() const { return m_isTag;}

            /// get the index/position of the child Package
            int indexOf(ReportPackage* child) const PALUDIS_ATTRIBUTE((warn_unused_result));

            /** \brief returns a PackageID
             */
            paludis::tr1::shared_ptr<const paludis::PackageID> ID() const PALUDIS_ATTRIBUTE((warn_unused_result)); // test

            /// returns the parent ReportPackage object of this Package object if present. Returns 0 if not
            ReportPackage *parent() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns const iterator of the first child
            ConstReportPackageIterator constChildBegin() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns const iterator of the end child ( the iterator after the last child )
            ConstReportPackageIterator constChildEnd() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns iterator of the first child
            ReportPackageIterator childBegin() PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns iterator of the end child ( the iterator after the last child )
            ReportPackageIterator childEnd() PALUDIS_ATTRIBUTE((warn_unused_result));
            ///@}


        protected:

            /// the data to are to be rendered (shown)
            QList<QVariant> m_data;

            /// problem information storage
            QList<ReportPackage*> m_tags;

            /// the ReportPackageId accociated with this ReportPackage. If this ReportPackage object is an parent object, and has an bestChild, it returns the bestChilds' ReportPackageID
            paludis::tr1::shared_ptr<const paludis::PackageID> m_id;

            /// the parent ReportPackage or node
            ReportPackage* m_parent;

            bool            m_isTag;
    };

    inline int ReportPackage::indexOf(ReportPackage* item) const
    {
        return m_tags.indexOf(item);
    }

    inline int ReportPackage::row() const
    {
        if (m_parent)
            return m_parent->indexOf( const_cast< ReportPackage* >(this) );
        return 0;
    }

    inline int ReportPackage::tagCount() const
    {
        return m_tags.count();
    }

    inline ReportPackage::ConstReportPackageIterator ReportPackage::constChildBegin() const
    {
        return m_tags.begin();
    }

    inline ReportPackage::ReportPackageIterator ReportPackage::childBegin()
    {
        return m_tags.begin();
    }

    inline ReportPackage::ConstReportPackageIterator ReportPackage::constChildEnd() const
    {
        return m_tags.end();
    }

    inline ReportPackage::ReportPackageIterator ReportPackage::childEnd()
    {
        return m_tags.end();
    }

    inline ReportPackage* ReportPackage::child(int rindex) const
    {
        return m_tags.value(rindex,0);
    }
}

QDebug operator<<(QDebug dbg, const pertubis::ReportPackage &c);

#endif
