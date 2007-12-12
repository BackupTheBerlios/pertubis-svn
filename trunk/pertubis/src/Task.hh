
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_TASK_H
#define _PERTUBIS_ENTRY_PROTECTOR_TASK_H 1

#include <list>
#include <QObject>
#include <QString>
#include <paludis/util/tr1_memory.hh>
#include <paludis/util/set.hh>
#include <paludis/util/set-impl.hh>
#include <paludis/util/operators.hh>
#include <paludis/package_id-fwd.hh>
#include <paludis/package_id.hh>
#include <paludis/environment-fwd.hh>

class QAction;

namespace pertubis
{
    class Package;
    class TaskBox;
    class Settings;
    class MessageOutput;

//     class IDCompare
//     {
//         public:
//             bool operator() (const paludis::tr1::shared_ptr<const paludis::PackageID>& a,
//                           const paludis::tr1::shared_ptr<const paludis::PackageID>& b) const;
//     };

    /*! \brief Holds PackageIDs for concrete task classes. This is an abstract base class
     * \see InstallTask, DeinstallTask
     * \ingroup Selection
     */
    class Task : public QObject
    {

        friend class TaskBox;
        Q_OBJECT

    public:

        /// defines a const iterator
        typedef paludis::PackageIDSet::ConstIterator Iterator;

        ///@name Constructors
        ///@{

        /// creates a Task object
        Task(QObject* pobject,
            QAction* action,
            QString name);
        ///@}

        ///\name Content modification
        ///\{

        /*! \brief Use this class if you want to process an items' selection change and its side effects on items' relatives (The UpdateRange )
         * \param item The item to process
         * \param mystate one of the values of Qt::CheckRole
         * We have to process every change here since only the task exactly knows how to deal with it. The task must be able to ask the item for the UpdateRange
         */
        virtual bool changeStates(Package* item, int mystate)=0;

        /*! \brief actually change
         *
         */
        void changeEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id, bool mystate);

        virtual void clear();

        /*! \brief returns the number of selected Packages for this task
         *
         */
        int itemCount() const { return m_data.size();}

        /*! \brief sets the action this task is referring to
         *
         */
        void setAction(QAction* myaction) {m_action = myaction;}

        /*! \brief sets Qt::Checked or Qt::Unchecked as result of hasEntry(item)
         *
         */
        void fillAction(Package* item);

        /*! \brief adds a selection specified by PackageID
         *
         */
        void addEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id);

        /*! \brief deletes a selection specified by PackageID
         *
         */
        void deleteEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id);

        /*! \brief starts the corresponding paludis task
         *
         */
        virtual void startTask(const paludis::tr1::shared_ptr<paludis::Environment>& env, Settings*,MessageOutput* output)=0;

        ///\}

        ///\name Content information
        ///\{

        /*! \brief returns the action connected with this task
        *
        */
        QAction* action() const {return m_action;}

        /*! \brief returns a reference to the data
        *
        */
        paludis::PackageIDSet& data() { return m_data;}

        /*! \brief returns the unique integer key for this task
        *
        */
        int taskid() const { return m_taskid;}

        /*! \brief checks if this package is already selected for this task
        *
        */
        bool hasEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id) const;

        /*! \brief returns the literal name of this task
        *
        */
        QString name() const { return m_name;}

        virtual bool available(Package* item) const = 0;

        ///\}


    protected:

        /// stores the PackageID of packages that are about to be used for this task
        paludis::PackageIDSet m_data;
        /// the QAction which triggers the execution of this task
        QAction*        m_action;
        /// the translated name of this task
        QString         m_name;

        /// this internal task number of this task are used for mapping the selection data in an Package to the correct task
        int             m_taskid;

    private:

        /*! \brief only for internal use
         */
        void setTaskid(int id);
    };

//     inline bool operator==(const Task& a,const Task& b) { return a.name() == b.name();}

}

#endif
