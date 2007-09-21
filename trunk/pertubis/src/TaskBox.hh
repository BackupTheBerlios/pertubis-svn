
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
#define _PERTUBIS_ENTRY_PROTECTOR_TASKBOX_H

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

        TaskBox(QObject* pobject) : QObject(pobject) {}
        int addTask(Task* task);
        Task* task(int taskid) const;
        bool hasTask(int taskid) const;
        QVariantList tasks() const;
        QVariantList selectionData(paludis::tr1::shared_ptr<const paludis::PackageID> id);
        void setItemTasks(Item* item);
        QVector<Task*>::const_iterator taskBegin() { return QVector<Task*>::iterator(m_tasks.constBegin());}
        QVector<Task*>::const_iterator taskEnd() { return QVector<Task*>::iterator(m_tasks.constEnd());}
        void run() {}

    public slots:
        void slotTaskChanged(paludis::tr1::shared_ptr<const paludis::PackageID> id, int taskid, bool state);

    private:
        QVector<Task*>    m_tasks;
        QMap<QString,int> m_index;
    };
}

#endif
