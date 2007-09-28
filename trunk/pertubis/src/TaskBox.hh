
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_TASKBOX_H
#define _PERTUBIS_ENTRY_PROTECTOR_TASKBOX_H 1

#include <QVector>
#include <QVariant>
#include <QObject>
#include <paludis/util/tr1_memory.hh>

namespace paludis
{
    class PackageID;
}

namespace pertubis
{
    class Task;
    class Item;

    /*! \brief manages the tasks pertubis knows about
    *
    */
    class TaskBox : public QObject
    {
        Q_OBJECT
    public:

        typedef QVector<Task*>::iterator Iterator;

        TaskBox(QObject* pobject) : QObject(pobject) {}
        int addTask(Task* task);
        Task* task(int taskid) const;
        bool hasTask(int taskid) const;

        /*! \brief returns a container format needed for Items with all values set to false
        *
        * Use this funtion if you want to contruct an empty but sane Item,
        * and fill it later with information. One of the drawbacks of the current Item implementation
        * is the need for the correct value count in this selection data container.
        * \see selectionData()
        */
        QVariantList tasks() const;

        /*! \brief returns a container format needed for Items with appropriate values
        *
        * Use this funtion if you want to contruct an Item with correct selection formation.
        * This function is more expensive than tasks!
        * \see tasks()
        */
        QVariantList selectionData(paludis::tr1::shared_ptr<const paludis::PackageID> id);

        /*! \brief sets the (visual) setting for each stored Task in the Item given
        *
        */
        void setItemTasks(Item* item);

        Iterator taskBegin();

        Iterator taskEnd();

        void run() {}

    public slots:
        void slotTaskChanged(paludis::tr1::shared_ptr<const paludis::PackageID> id, int taskid, bool state);

    private:
        QVector<Task*>    m_tasks;
        QMap<QString,int> m_index;
    };

    inline TaskBox::Iterator TaskBox::taskBegin() { return Iterator(m_tasks.begin());}
    inline TaskBox::Iterator TaskBox::taskEnd() { return Iterator(m_tasks.end());}
}

#endif
