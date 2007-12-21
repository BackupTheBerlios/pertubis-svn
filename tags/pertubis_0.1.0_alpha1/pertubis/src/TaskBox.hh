
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_TASKBOX_H
#define _PERTUBIS_ENTRY_PROTECTOR_TASKBOX_H 1

#include <QVector>
#include <QVariant>
#include <QObject>
#include <paludis/util/tr1_memory.hh>
#include <paludis/package_id-fwd.hh>
#include <paludis/environment-fwd.hh>

namespace pertubis
{
    class Task;
    class Package;
    class MessageOutput;
    class Settings;

    /*! \brief manages the tasks pertubis knows about
    *
    * \ingroup Selection
    */
    class TaskBox : public QObject
    {
        Q_OBJECT
    public:

        /// defines a iterator used for iterating over all Tasks
        typedef QVector<Task*>::iterator Iterator;
        /// defines a const iterator used for iterating over all Tasks
        typedef QVector<Task*>::const_iterator ConstIterator;

        ///\name Constructors
        ///\{

        /// constructs a TaskBox object
        TaskBox(QObject* pobject) : QObject(pobject) {}
        ///\}

        ///\name Content information
        ///\{

        /// returns the task with task id, if the id is out of bounds it returns 0
        Task* task(int taskid) const PALUDIS_ATTRIBUTE((warn_unused_result));

        /// returns the task with task id, if the id is out of bounds it returns 0
        Task* task(QString name) const PALUDIS_ATTRIBUTE((warn_unused_result));

        /// returns true if TaskBox has Task with task id
        bool hasTask(int taskid) const PALUDIS_ATTRIBUTE((warn_unused_result));

        /*! \brief returns a container format needed for Items with all values set to false
         *
         * Use this funtion if you want to contruct an empty but sane Item,
         * and fill it later with information. One of the drawbacks of the current Item implementation
         * is the need for the correct value count in this selection data container.
         * \see selectionData()
         */
        QVariantList tasks() const PALUDIS_ATTRIBUTE((warn_unused_result));

        /*! \brief returns a container format needed for Items with appropriate values
         *
         * Use this funtion if you want to contruct an Item with correct selection information.
         * This function is more expensive than tasks!
         * \see tasks()
         */
        QVariantList selectionData(paludis::tr1::shared_ptr<const paludis::PackageID> id) const PALUDIS_ATTRIBUTE((warn_unused_result));
        ///\}

        ///\name Content modification
        ///\{

        /// adds a Task to this container class and returns the task id of Task
        int addTask(Task* task);

        /*! \brief sets each Task selection status for the given Package
        *
        */
        void setTasksInPackage(Package* package);

         /*! \brief starts all known tasks
          *
          */
        void startAllTasks(const paludis::tr1::shared_ptr<paludis::Environment>& env,Settings* settings, MessageOutput* output);

         ///\name Iteration
        ///\{

        /// returns iterator of the first Task
        Iterator taskBegin() PALUDIS_ATTRIBUTE((warn_unused_result));
        /// returns iterator of the end Task ( the element after the last Task )
        Iterator taskEnd() PALUDIS_ATTRIBUTE((warn_unused_result));

        /// returns const iterator of the first Task
        ConstIterator constTaskBegin() const PALUDIS_ATTRIBUTE((warn_unused_result));
        /// returns const iterator of the end Task ( the element after the last Task )
        ConstIterator constTaskEnd() const PALUDIS_ATTRIBUTE((warn_unused_result));
        ///\}

        /// QThread overloaded
        void run() {}

    public slots:

        /*! \brief add or delete the id from the task
         */
        void slotTaskChanged(paludis::tr1::shared_ptr<const paludis::PackageID> id, int taskid, bool state);

        /*! \brief clear all task data, but not the tasks
         */
        void slotClear();

    private:
        /// all tasks are stored here
        QVector<Task*>    m_tasks;
        /// map task name to task
        QMap<QString,Task*> m_index;
    };

    inline TaskBox::Iterator TaskBox::taskBegin() { return Iterator(m_tasks.begin());}
    inline TaskBox::Iterator TaskBox::taskEnd() { return Iterator(m_tasks.end());}
    inline TaskBox::ConstIterator TaskBox::constTaskBegin() const { return Iterator(m_tasks.constBegin());}
    inline TaskBox::ConstIterator TaskBox::constTaskEnd() const { return Iterator(m_tasks.constEnd());}
}

#endif
