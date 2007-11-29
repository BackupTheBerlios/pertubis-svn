
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
    /*! \brief This node class holds important information for each package(-version).
    *
    */


    class Item : public QObject
    {
        Q_OBJECT

        public:

            enum ItemState { is_stable, is_unstable, is_masked };
            enum ItemOrder { io_selected, io_package, io_category, io_repository, io_installed,io_mask_reasons,io_change};

            /*! \brief In which direction we want model updates
            * \param it_nothing no update
            * \param it_parent update this parent node and all child nodes ( down )
            * \param it_child update from parent and all child nodes, including this child node ( up )
            * \param it_node_full update parent of this node, all childs and the node itself ( up and down )
            * \param it_node_only update only this node
            */
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

            void appendChild(Item *child); // test
            void prependChild(Item *child);  // test
            void setTaskState(int taskid, Qt::CheckState state);  // test

            void setData(int column, QVariant data);  // test
            void setParent(Item* pitem);  // test
            void setState(ItemState s);  // test
            void setBestChild(Item* item);  // test

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
            void setID(const paludis::tr1::shared_ptr<const paludis::PackageID>& id) { m_id = id;} // test
            paludis::tr1::shared_ptr<const paludis::PackageID> ID(); // test
            ConstIterator constChildBegin();
            Iterator childBegin();
            ConstIterator constChildEnd();
            Iterator childEnd();

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


#endif
