
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

/*! \enum pertubis::Item::ItemState
 * An Item can be stable, unstable and masked. This is a higher level view on packages
 */

/*! \enum pertubis::Item::ItemState pertubis::Item::is_stable
 * means this package (version) is stable
 */

/*! \enum pertubis::Item::ItemState pertubis::Item::is_unstable
 * means this package (version) is unstable
 */

/*! \enum pertubis::Item::ItemState pertubis::Item::is_masked
 * means this package (version) is masked
 */

/*! \enum pertubis::Item::ItemOrder
 * The order of information in pertubis::Item::m_data and column order in the package view
 */

/*! \enum pertubis::Item::ItemOrder* pertubis::Item::io_selected
 * QVariant<QVariantList>. Stores the task selection states
 */

/*! \enum pertubis::Item::ItemOrder* pertubis::Item::io_package
 * QVariant<QString>. paludis::PackagePartName
 */

/*! \enum pertubis::Item::ItemOrder* pertubis::Item::io_category
 * QVariant<QString>. paludis::CategoryPartName
 */

/*! \enum pertubis::Item::ItemOrder* pertubis::Item::io_repository
 * QVariant<QString>. paludis::RepositoryName
 */

/*! \enum pertubis::Item::ItemOrder* pertubis::Item::io_installed
 * QVariant<bool>. true if the package is installed
 */

/*! \enum pertubis::Item::ItemOrder* pertubis::Item::io_mask_reasons
 * QVariant<QString>. paludis::MaskReasons
 */

/*! \enum pertubis::Item::ItemOrder* pertubis::Item::io_change
 * QVariant<QString>. shows if an packages best version is not the installed version, e.g. upgrade or downgrade
 */

/*! \enum pertubis::Item::ItemType
 * The package overview uses this Item class for storing the information about the package it represents. In which direction we want model updates
 */

/*! \enum pertubis::Item::ItemType* pertubis::Item::it_nothing
 * This value is only used when the default constructor was called
 */

/*! \enum pertubis::Item::ItemType* pertubis::Item::it_parent
 * update this parent node and all child nodes ( down )
 */

/*! \enum pertubis::Item::ItemType* pertubis::Item::it_child
 * update from parent and all child nodes, including this child node ( up )
 */

/*! \enum pertubis::Item::ItemType* pertubis::Item::it_node_full
 * The item can have childs, and an parent Item.
 * when manipulated parent of this node, all childs and the node itself will be updated ( up and down )
 */

/*! \enum pertubis::Item::ItemType* pertubis::Item::it_node_only
 * The item will have any childs, and maybe any parent.
 * when manipulated only this node will be updated
 */

namespace pertubis
{
    /*! \brief package information / metadata storage
     *
     * \ingroup ItemModelClass
     * The package overview uses this Item class for storing the information about the package it represents.
     * Items are node classes, and pertubis can use many hundreds of it as a tree structures.
     * We are showing the packages of a category as a tree with a "meta item" or package item with version items or child items
     */
    class Item : public QObject
    {
        Q_OBJECT

        public:

            enum ItemState { is_stable, is_unstable, is_masked };

            enum ItemOrder
            {
                io_selected,
                io_package,
                io_category,
                io_repository,
                io_installed,
                io_mask_reasons,
                io_change
            };

            enum ItemType
            {
                it_nothing,
                it_parent,
                it_child,
                it_node_full,
                it_node_only,
            };

            /// defines a const iterator used for iterating over all child Items
            typedef QList<Item*>::const_iterator ConstIterator;
            /// defines a const iterator used for iterating over all child Items
            typedef QList<Item*>::iterator Iterator;

            ///@name Constructors
            ///@{

            /// default constructor
            explicit Item();

            /// copy constructor
            explicit Item(const Item& other);

            /// that's the usual way to create an Item object
            explicit Item(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                 const QList<QVariant> &dats,
                 ItemState mystate,
                 ItemType itype,
                 Item* parent);
            ///@}

            /// destructor
            ~Item();

             ///@name Content modification
            ///@{

            /// appends a new child Item
            void appendChild(Item *child); // test

            /// prepends a new child Item
            void prependChild(Item *child);  // test

            /*! @brief
             *
             * This is a convenience method for setting the selection state for one Task. To set all selection states you can use Item::setData()
             */
            void setTaskState(int taskid, Qt::CheckState state);  // test

            /// sets a new displayable value in column
            void setData(int column, QVariant data);  // test

            /// sets a new parent node / Item object
            void setParent(Item* pitem);  // test


            /// changes the ItemState of this Item object
            void setItemState(ItemState s);  // test

            /// sets the best child Item object
            void setBestChild(Item* item);  // test

            /// sets a new PackageID
            void setID(const paludis::tr1::shared_ptr<const paludis::PackageID>& id) { m_id = id;} // test
            ///@}

            ///@name Content information
            ///@{


            /// returns the best child Item object if present, else returns 0
            Item* bestChild() const PALUDIS_ATTRIBUTE((warn_unused_result));  // test

            /// returns true if the Items' ItemState is not Item::is_masked
            bool available() const PALUDIS_ATTRIBUTE((warn_unused_result));  // test

            /// returns the state of the Item
            ItemState state() const PALUDIS_ATTRIBUTE((warn_unused_result));  // test

            /// returns the child Item of this Item object with number "row" if present, and 0 if not
            Item *child(int row) const PALUDIS_ATTRIBUTE((warn_unused_result));  // test

            /// get the index/position of the child Item
            int indexOf(Item* child) const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns the total number of childs of this Item object
            int childCount() const PALUDIS_ATTRIBUTE((warn_unused_result));  // test

            /// returns the total number of displayable data in this Item object
            int columnCount() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns the displayable data in "column"
            QVariant data(int column) const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns the row index of this Item object in parent if parent is valid, else -1
            int row() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns the type of this Item object
            ItemType itemType() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns the parent Item object of this Item object if present. Returns 0 if not
            Item *parent() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /*! \brief when it has a best child, it returns the bestChilds' PackageID, otherwise it returns its' PackageID
             */
            paludis::tr1::shared_ptr<const paludis::PackageID> ID() const PALUDIS_ATTRIBUTE((warn_unused_result)); // test

            ///@}


            ///@name Iteration
            ///@{

            /// returns const iterator of the first child
            ConstIterator constChildBegin() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns const iterator of the end child ( the iterator after the last child )
            ConstIterator constChildEnd() const PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns iterator of the first child
            Iterator childBegin() PALUDIS_ATTRIBUTE((warn_unused_result));

            /// returns iterator of the end child ( the iterator after the last child )
            Iterator childEnd() PALUDIS_ATTRIBUTE((warn_unused_result));
            ///@}

        protected:

            /// the data to are to be rendered (shown)
            QList<QVariant>     m_data;

            /// children storage
            QList<Item*>        m_children;

            /// the PackageId accociated with this Item. If this Item object is an parent object, and has an bestChild, it returns the bestChilds' PackageID
            paludis::tr1::shared_ptr<const paludis::PackageID> m_id;

            /// the parent Item or node
            Item*               m_parent;

            /// a best child could be the Item with the highest unmasked version number
            Item*               m_bestChild;

            /// the state of this Item object has
            ItemState           m_state;

            /// the type of the Item
            ItemType            m_itemType;
    };


//     QString stateDescription(Item::ItemState status);

    /*! \brief helps us creating a PackageItem
     * \ingroup ItemModelClass
     *
     */
    Item* makePackageItem(
        paludis::tr1::shared_ptr<const paludis::PackageID> id,
        QVariantList selections,
        QString pack,
        QString cat,
        Qt::CheckState isInstalled,
        Item::ItemState mystate,
        Item* pitem,
        QString mask_reasons);

    /*! \brief helps us creating a VersionItem
     * \ingroup ItemModelClass
     *
     */
    Item* makeVersionItem(
        paludis::tr1::shared_ptr<const paludis::PackageID> id,
        QVariantList selections,
        QString version,
        QString rep,
        Qt::CheckState isInstalled,
        Item::ItemState mystate,
        Item* pitem,
        QString mask_reasons);

    inline Item::ItemType pertubis::Item::itemType() const
    {
        return m_itemType;
    }

    inline Item* pertubis::Item::bestChild() const
    {
        return m_bestChild;
    }

    inline int pertubis::Item::indexOf(Item* item) const
    {
        return m_children.indexOf(item);
    }

    inline int Item::row() const
    {
        if (m_parent)
            return m_parent->indexOf( const_cast< Item* >(this) );
        return 0;
    }

    inline Item::ConstIterator pertubis::Item::constChildBegin() const
    {
        return m_children.begin();
    }

    inline Item::Iterator pertubis::Item::childBegin()
    {
        return m_children.begin();
    }

    inline Item::ConstIterator pertubis::Item::constChildEnd() const
    {
        return m_children.end();
    }

    inline Item::Iterator pertubis::Item::childEnd()
    {
        return m_children.end();
    }
}

QDebug operator<<(QDebug dbg, const pertubis::Item &c);

#endif
