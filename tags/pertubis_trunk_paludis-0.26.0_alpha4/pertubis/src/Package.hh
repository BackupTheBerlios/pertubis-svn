
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_ITEM_H
#define _PERTUBIS_ENTRY_PROTECTOR_ITEM_H

#include <QList>
#include <QDebug>
#include <QVariant>
#include <paludis/util/attributes.hh>
#include <paludis/util/tr1_memory.hh>
#include <paludis/package_id-fwd.hh>

/*! \enum pertubis::Package::PackageState
 * An Package can be stable, unstable and masked. This is a higher level view on packages
 */

/*! \enum pertubis::Package::PackageState pertubis::Package::ps_stable
 * means this package (version) is stable
 */

/*! \enum pertubis::Package::PackageState pertubis::Package::ps_unstable
 * means this package (version) is unstable
 */

/*! \enum pertubis::Package::PackageState pertubis::Package::ps_masked
 * means this package (version) is masked
 */

/*! \enum pertubis::Package::PackageOrder
 * The order of information in pertubis::Package::m_data and column order in the package view
 */

/*! \enum pertubis::Package::PackageOrder* pertubis::Package::po_selected
 * QVariant<QVariantList>. Stores the task selection states
 */

/*! \enum pertubis::Package::PackageOrder* pertubis::Package::po_package
 * QVariant<QString>. paludis::PackagePartName
 */

/*! \enum pertubis::Package::PackageOrder* pertubis::Package::po_category
 * QVariant<QString>. paludis::CategoryPartName
 */

/*! \enum pertubis::Package::PackageOrder* pertubis::Package::po_repository
 * QVariant<QString>. paludis::RepositoryName
 */

/*! \enum pertubis::Package::PackageOrder* pertubis::Package::po_installed
 * QVariant<bool>. true if the package is installed
 */

/*! \enum pertubis::Package::PackageOrder* pertubis::Package::po_mask_reasons
 * QVariant<QString>. paludis::MaskReasons
 */

/*! \enum pertubis::Package::PackageOrder* pertubis::Package::po_change
 * QVariant<QString>. shows if an packages best version is not the installed version, e.g. upgrade or downgrade
 */

/*! \enum pertubis::Package::PackageType
 * The package overview uses this Package class for storing the information about the package it represents. In which direction we want model updates
 */

/*! \enum pertubis::Package::PackageType* pertubis::Package::pt_nothing
 * This value is only used when the default constructor was called
 */

/*! \enum pertubis::Package::PackageType* pertubis::Package::pt_parent
 * update this parent node and all child nodes ( down )
 */

/*! \enum pertubis::Package::PackageType* pertubis::Package::pt_child
 * update from parent and all child nodes, including this child node ( up )
 */

/*! \enum pertubis::Package::PackageType* pertubis::Package::pt_node_full
 * The item can have childs, and an parent Package.
 * when manipulated parent of this node, all childs and the node itself will be updated ( up and down )
 */

/*! \enum pertubis::Package::PackageType* pertubis::Package::pt_node_only
 * The item will have any childs, and maybe any parent.
 * when manipulated only this node will be updated
 */

namespace pertubis
{
    /*! \brief package information / metadata storage
     *
     * \ingroup PackageModelClass
     * The package overview uses this Package class for storing the information about the package it represents.
     * Packages are node classes, and pertubis can use many hundreds of it as a tree structures.
     * We are showing the packages of a category as a tree with a "meta item" or package item with version items or child items
     */
    class Package : public QObject
    {
        Q_OBJECT

        public:

            enum PackageState { ps_stable, ps_unstable, ps_masked };

            enum PackageOrder
            {
                po_selected,
                po_package,
                po_category,
                po_repository,
                po_installed,
                po_mask_reasons,
                po_change,
                po_last
            };

            enum PackageType
            {
                pt_nothing,
                pt_parent,
                pt_child,
                pt_node_full,
                pt_node_only,
            };

            /// defines a const iterator used for iterating over all child Packages
            typedef QList<Package*>::const_iterator ConstPackageIterator;
            /// defines a const iterator used for iterating over all child Packages
            typedef QList<Package*>::iterator PackageIterator;

            ///@name Constructors
            ///@{

            /// default constructor
            Package();

            /// copy constructor
            Package(const Package& other);

            /// that's the usual way to create an Package object
            Package(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                 const QList<QVariant> &dats,
                 PackageState mystate,
                 PackageType itype,
                 Package* parent);
            ///@}
            /// destructor
            ~Package();

             ///@name Content modification
            ///@{

            /// appends a new child Package
            void appendChild(Package *child); // test

            /// prepends a new child Package
            void prependChild(Package *child);  // test

            /*! @brief
             *
             * This is a convenience method for setting the selection state for one Task. To set all selection states you can use Package::setData()
             */
            void setTaskState(int taskid, Qt::CheckState state);  // test

            /// sets a new displayable value in column
            void setData(int column, QVariant data);  // test

            /// sets a new parent node / Package object
            void setParent(Package* pitem);  // test


            /// changes the PackageState of this Package object
            void setPackageState(PackageState s);  // test

            /// sets the best child Package object
            void setBestChild(Package* item);  // test

            /// sets a new PackageID
            void setID(const paludis::tr1::shared_ptr<const paludis::PackageID>& id) { m_id = id;} // test
            ///@}
            ///@name Content information
            ///@{

            /// returns the best child Package object if present, else returns 0
            Package* bestChild() const PALUDIS_ATTRIBUTE((warn_unused_result));  // test

            /// returns true if the Packages' PackageState is not Package::ps_masked
            bool available() const PALUDIS_ATTRIBUTE((warn_unused_result));  // test

            /// returns the state of the Package
            PackageState state() const PALUDIS_ATTRIBUTE((warn_unused_result));  // test

            /// returns the child Package of this Package object with number "row" if present, and 0 if not
            Package *child(int row) const PALUDIS_ATTRIBUTE((warn_unused_result));  // test

            /// get the index/position of the child Package
            int indexOf(Package* child) const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns the total number of childs of this Package object
            int childCount() const PALUDIS_ATTRIBUTE((warn_unused_result));  // test

            /// returns the total number of displayable data in this Package object
            int columnCount() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns the displayable data in "column"
            QVariant data(int column) const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns the row index of this Package object in parent if parent is valid, else -1
            int row() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns the type of this Package object
            PackageType itemType() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns the parent Package object of this Package object if present. Returns 0 if not
            Package *parent() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /*! \brief when it has a best child, it returns the bestChilds' PackageID, otherwise it returns its' PackageID
             */
            paludis::tr1::shared_ptr<const paludis::PackageID> ID() const PALUDIS_ATTRIBUTE((warn_unused_result)); // test
            ///@}
            ///@name Iteration
            ///@{

            /// returns const iterator of the first child
            ConstPackageIterator constChildBegin() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns const iterator of the end child ( the iterator after the last child )
            ConstPackageIterator constChildEnd() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns iterator of the first child
            PackageIterator childBegin() PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns iterator of the end child ( the iterator after the last child )
            PackageIterator childEnd() PALUDIS_ATTRIBUTE((warn_unused_result));
            ///@}

        protected:

            /// the data to are to be rendered (shown)
            QList<QVariant>     m_data;

            /// children storage
            QList<Package*>        m_children;

            /// the PackageId accociated with this Package. If this Package object is an parent object, and has an bestChild, it returns the bestChilds' PackageID
            paludis::tr1::shared_ptr<const paludis::PackageID> m_id;

            /// the parent Package or node
            Package*               m_parent;

            /// a best child could be the Package with the highest unmasked version number
            Package*               m_bestChild;

            /// the state of this Package object has
            PackageState           m_state;

            /// the type of the Package
            PackageType            m_itemType;
    };


//     QString stateDescription(Package::PackageState status);

    /*! \brief helps us creating a PackagePackage
     * \ingroup PackageModelClass
     *
     */
    Package* makePackagePackage(
        paludis::tr1::shared_ptr<const paludis::PackageID> id,
        QVariantList selections,
        QString pack,
        QString cat,
        Qt::CheckState isInstalled,
        Package::PackageState mystate,
        Package* pitem,
        QString mask_reasons);

    /*! \brief helps us creating a VersionPackage
     * \ingroup PackageModelClass
     *
     */
    Package* makeVersionPackage(
        paludis::tr1::shared_ptr<const paludis::PackageID> id,
        QVariantList selections,
        QString version,
        QString rep,
        Qt::CheckState isInstalled,
        Package::PackageState mystate,
        Package* pitem,
        QString mask_reasons);

    Package* makeRootPackage(int columns);

    inline Package::PackageType pertubis::Package::itemType() const
    {
        return m_itemType;
    }

    inline Package* pertubis::Package::bestChild() const
    {
        return m_bestChild;
    }

    inline int pertubis::Package::indexOf(Package* item) const
    {
        return m_children.indexOf(item);
    }

    inline int Package::row() const
    {
        if (m_parent)
            return m_parent->indexOf( const_cast< Package* >(this) );
        return 0;
    }

    inline Package::ConstPackageIterator pertubis::Package::constChildBegin() const
    {
        return m_children.begin();
    }

    inline Package::PackageIterator pertubis::Package::childBegin()
    {
        return m_children.begin();
    }

    inline Package::ConstPackageIterator pertubis::Package::constChildEnd() const
    {
        return m_children.end();
    }

    inline Package::PackageIterator pertubis::Package::childEnd()
    {
        return m_children.end();
    }
}

QDebug operator<<(QDebug dbg, const pertubis::Package &c);

#endif
