
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
        QVariantList selectionData(const paludis::PackageID* id);
        QVector<Task*>::const_iterator taskBegin() { return QVector<Task*>::iterator(m_tasks.constBegin());}
        QVector<Task*>::const_iterator taskEnd() { return QVector<Task*>::iterator(m_tasks.constEnd());}
        void run() {}

    public slots:
        void slotTaskChanged(const paludis::PackageID* id, int taskid, bool state);

    private:
        QVector<Task*>    m_tasks;
    };
}

#endif
