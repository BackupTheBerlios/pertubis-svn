
/* Copyright (C) 2007 Stefan Koegl <hotshelf@users.berlios.de>
*
* This file is part of the pertubis frontend for paludis package manager.
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
#include <paludis/util/tr1_memory.hh>

namespace paludis
{
    class PackageID;
}


namespace pertubis
{
    /*! \brief package information store
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

            enum ItemType { it_nothing, it_parent, it_child, it_node_full,it_node_only, };

            typedef QList<Item*>::const_iterator ConstIterator;
            typedef QList<Item*>::iterator Iterator;

            Item();

            Item(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                 const QList<QVariant> &dats,
                 ItemState mystate,
                 ItemType itype,
                 Item* parent);

            virtual ~Item();

             ///\name Content modification
            ///\{

            void appendChild(Item *child); // test
            void prependChild(Item *child);  // test
            void setTaskState(int taskid, Qt::CheckState state);  // test

            void setData(int column, QVariant data);  // test
            void setParent(Item* pitem);  // test
            void setState(ItemState s);  // test
            void setBestChild(Item* item);  // test
            void setID(const paludis::tr1::shared_ptr<const paludis::PackageID>& id) { m_id = id;} // test
            ///\}

             ///\name Content information
            ///\{


            Item* bestChild() const;  // test
            bool available() const;  // test
            ItemState state() const;  // test
            Item *child(int row) const;  // test
            int indexOf(Item* item) const;
            int childCount() const;  // test
            int columnCount() const;
            QVariant data(int column) const;
            int row() const;
            ItemType itemType() const;
            Item *parent() const;

            /*! \brief when it has a best child, it returns the bestChilds' PackageID, otherwise it returns its' PackageID
             */
            paludis::tr1::shared_ptr<const paludis::PackageID> ID(); // test

            ///\}


            ///\name Iteration
            ///\{
            ConstIterator constChildBegin();

            Iterator childBegin();

            ConstIterator constChildEnd();

            Iterator childEnd();
            ///\}

        protected:

            QList<QVariant>     m_data;
            QList<Item*>        m_children;
            paludis::tr1::shared_ptr<const paludis::PackageID> m_id;
            Item*               m_parent;
            Item*               m_bestChild;
            ItemState           m_state;
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
}

QDebug operator<<(QDebug dbg, const pertubis::Item &c);

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


#endif
