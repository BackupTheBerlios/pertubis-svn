
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

#include "Package.hh"
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

namespace pertubis
{
    class Settings;
    class SelectionModel;
    class MessageOutput;

    /*! \brief Holds PackageIDs for concrete task classes. This is an abstract base class
     * \see InstallSelections, DeinstallSelections
     * \ingroup Selection
     */
    class Selections : public QObject
    {
        Q_OBJECT

    public:

        /// defines a const iterator
        typedef paludis::PackageIDSet::ConstIterator Iterator;

        ///@name Constructors
        ///@{

        /// creates a Selections object
        Selections(QObject* pobject,
            QString myname);
        ///@}

        virtual ~Selections() {}

        ///\name Content modification
        ///\{

        /*! \brief Use this class if you want to process an items' selection change and its side effects on items' relatives (The UpdateRange )
         * \param item The item to process
         * \param mystate one of the values of Qt::CheckRole
         * We have to process every change here since only the task exactly knows how to deal with it. The task must be able to ask the item for the UpdateRange
         */
        virtual bool changeStates(Package* item, int mystate, int column)=0;

        /*! \brief actually change
         *
         */
        void changeEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id, bool mystate);

        virtual void clear();

        /*! \brief sets the name
         *
         */
        void setName(const QString & myname) {m_name = myname;}

        /*! \brief adds a selection specified by PackageID
         *
         */
        void addEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id);

        /*! \brief deletes a selection specified by PackageID
         *
         */
        void deleteEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id);

        ///\}

        ///\name Content information
        ///\{

        /*! \brief returns the number of selected Packages for this task
         *
         */
        int entryCount() const { return m_data.size();}

        /*! \brief checks if this package is already selected for this task
        *
        */
        Qt::CheckState hasEntry(const paludis::tr1::shared_ptr<const paludis::PackageID>& id) const;

        /*! \brief returns the literal name of this task
        *
        */
        QString name() const { return m_name;}

        virtual bool available(Package* item, int column) const = 0;

        bool hasEntries() { return ! m_data.empty() ;}

        paludis::PackageIDSet::ConstIterator entriesBegin() { return m_data.begin();}
        paludis::PackageIDSet::ConstIterator entriesEnd() { return m_data.end();}

        ///\}

    protected:

        /// stores the PackageID of packages that are about to be used for this task
        paludis::PackageIDSet m_data;

        /// the translated name of this task
        QString         m_name;

    private:

        /// \brief prohibit copying
        Selections(const Selections& other);
    };

}

#endif
