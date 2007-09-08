
/* Copyright (C) 2007 Stefan Koegl.
*
* This file is part of pertubis.
*
* pertubis is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* pertubis is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http:*www.gnu.org/licenses/>.
*/

#ifndef _PERTUBIS_ENTRY_PROTECTOR_ITEM_H
#define _PERTUBIS_ENTRY_PROTECTOR_ITEM_H

#include <QList>
#include <QVariant>
#include <paludis/util/tr1_memory.hh>

#include "TaskBox.hh"

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
            enum ItemOrder { io_selected, io_package, io_category, io_repository, io_installed};
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

            Item(const QList<QVariant> &dats,
                 ItemState mystate);

            virtual ~Item();

            virtual void appendChild(Item *child);
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
            virtual UpdateRange updateRange() const = 0;
            virtual Item *parent() const;
            virtual paludis::tr1::shared_ptr<const paludis::PackageID> ID() const=0;
            QList<Item*>::iterator childBegin();
            QList<Item*>::iterator childEnd();

        protected:
            QList<QVariant>    m_data;
            QList<Item*>    m_children;
            Item*             m_parent;
            Item*            m_bestChild;
            ItemState        m_state;
            QVector<bool>    m_taskStates;
    };

    class RootItem : public Item
    {
        Q_OBJECT

        public:
            RootItem() : Item(QList<QVariant>() << "" << "" << "" << "" << "",Item::is_stable) {}
            UpdateRange updateRange() const;
            paludis::tr1::shared_ptr<const paludis::PackageID> ID() const;
    };

    QString status(Item::ItemState status);
}

#endif
