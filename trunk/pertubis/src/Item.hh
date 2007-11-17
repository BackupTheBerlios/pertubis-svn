
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
            enum ItemType { it_category,it_package,it_version};

            /*! \brief In which direction we want model updates
            * \param ur_nothing no update
            * \param ur_node update only this node
            * \param ur_parent update this parent node and all child nodes ( down )
            * \param ur_child update from parent and all child nodes, including this child node ( up )
            * \param ur_both update parent of this node, all childs and the node itself ( up and down )
            */
            enum UpdateRange { ur_nothing, ur_node, ur_parent, ur_child, ur_both};

            Item();

            Item(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                 const QList<QVariant> &dats,
                 ItemState mystate,
                 UpdateRange ur,
                 Item* parent);

            Item(const QList<QVariant> &dats,
                 ItemState mystate,
                 UpdateRange ur,
                 Item* pitem);

            virtual ~Item();

            virtual void appendChild(Item *child);
            virtual void prependChild(Item *child);
            virtual void setTaskState(int taskid, Qt::CheckState state);

            virtual void setData(int column, QVariant data);
            virtual void setParent(Item* pitem);
            virtual void setState(ItemState s);
            virtual void setBestChild(Item* item);

            virtual Item* bestChild() const;
            virtual bool available() const;
            virtual ItemState state() const;
            virtual Item *child(int row) const;
            virtual int indexOf(Item* item) const;
            virtual int childCount() const;
            virtual int columnCount() const;
            virtual QVariant data(int column) const;
            virtual int row() const;
            virtual UpdateRange updateRange() const;
            virtual Item *parent() const;
            virtual paludis::tr1::shared_ptr<const paludis::PackageID> ID();
            QList<Item*>::iterator childBegin();
            QList<Item*>::iterator childEnd();

        protected:

            QList<QVariant>     m_data;
            QList<Item*>        m_children;
            paludis::tr1::shared_ptr<const paludis::PackageID> m_id;
            Item*               m_parent;
            Item*               m_bestChild;
            ItemState           m_state;
            UpdateRange         m_ur;
    };


    QString stateDescription(Item::ItemState status);
    QString status(Item::ItemState status);

    /*! \brief helps us creating a PackageItem
     *
     */

    inline Item* makePackageItem(paludis::tr1::shared_ptr<const paludis::PackageID> id,
                                QVariantList selections,
                                QString pack,
                                QString cat,
                                Qt::CheckState isInstalled,
                                Item::ItemState mystate,
                                Item::UpdateRange ur,
                                Item* pitem,
                                QString mask_reasons)
    {
        QVariantList list;
        list <<
            QVariant(selections) <<  // io_selected
            pack << // io_package
            cat <<  // io_category
            "" << // io_repository
            QVariant(static_cast<int>(isInstalled)) <<  // io_installed
            mask_reasons << // io_mask_reasons
            "";  // io_change
        return new Item(id,list,mystate,ur,pitem);
    }

    /*! \brief helps us creating a VersionItem
    *
    */
    inline Item* makeVersionItem(paludis::tr1::shared_ptr<const paludis::PackageID> id,
        QVariantList selections,
        QString version,
        QString rep,
        Qt::CheckState isInstalled,
        Item::ItemState mystate,
        Item::UpdateRange ur,
        Item* pitem,
        QString mask_reasons)
    {
        QVariantList list;
        list <<
            QVariant(selections) << // io_selected
            version << // io_package
            "" << // io_category
            rep << // io_repository
            isInstalled <<  // io_installed
            mask_reasons <<  // io_mask_reasons
            ""; // io_change
        return new Item(id,list,mystate,ur,pitem);
    }
}

QDebug operator<<(QDebug dbg, const pertubis::Item &c);


#endif
