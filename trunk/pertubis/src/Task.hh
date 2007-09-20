
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_TASK_H
#define _PERTUBIS_ENTRY_PROTECTOR_TASK_H

#include <QString>
#include <QAction>

#include <QVector>

#include <paludis/util/tr1_memory.hh>
#include <list>

namespace paludis
{
    class PackageID;
}

namespace pertubis
{
    class Item;

    /*! \brief Holds PackageIDs for a special purpose. This is an abstract base class
    * \see InstallTask, DeinstallTask
    */
    class Task : public QObject
    {
        Q_OBJECT

    public:

        Task() : m_taskid(-1) {}
        Task(QObject* pobject,QString name);
        Task(QObject* pobject,
            QAction* action,
            QString name);

        void addEntry(paludis::tr1::shared_ptr<const paludis::PackageID*> id);

        void deleteEntry(paludis::tr1::shared_ptr<const paludis::PackageID*> id);

        /*! \brief only for internal use
        */
        void setTaskid(int id);

        /*! \brief sets the action this task is referring to
        */
        void setAction(QAction* myaction) {m_action = myaction;}

        /*! \brief sets Qt::Checked or Qt::Unchecked as result of hasEntry(item)
        */
        void fillAction(Item* item);

        QAction* action() const {return m_action;}

        std::list<paludis::tr1::shared_ptr<const paludis::PackageID> >& data() { return m_data;}

        int taskid() const { return m_taskid;}

        bool hasEntry(const paludis::PackageID* id) const;

        QString name() const { return m_name;}

        virtual bool available(Item* item) const = 0;

        /*! \brief Use this class if you want to process a child items' selection change and its side effects on items' parent (The UpdateRange )
        * \param item The item to process
        * \param mystate add or delete the item to this task
        * We have to process every change here since only the task exactly knows how to deal with it. The task must be able to ask the item for the UpdateRange
        */
        virtual bool changeChildStates(Item* item,bool mystate)=0;

        /*! \brief Use this class if you want to process a parent items' selection change and its side effects on all of its childs.
        *
        */
        virtual bool changeParentStates(Item* item,bool mystate)=0;

        /*! \brief actually change
        *
        */
        void changeEntry(const paludis::PackageID* id, bool mystate);

    protected:

        QAction*        m_action;
        QString         m_name;
        int             m_taskid;

    private:
        std::list<paludis::tr1::shared_ptr<const paludis::PackageID> > m_data;
    };

    inline bool operator!=(const Task& a,const Task& b) { return a.name() != b.name();}

    inline bool operator==(const Task& a,const Task& b) { return a.name() == b.name();}

}

#endif
