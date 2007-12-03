
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

#ifndef _PERTUBIS_ENTRY_PROTECTOR_SEARCH_THREAD_H
#define _PERTUBIS_ENTRY_PROTECTOR_SEARCH_THREAD_H 1

#include "ThreadBase.hh"
#include <QString>

namespace pertubis
{
    class Item;

    /*! \brief this thread is used in conjunction with SearchWindow. Returns a complete item tree.
     *
     * \ingroup PaludisAdapter
     * \ingroup Thread
     */
    class SearchThread : public ThreadBase
    {
        Q_OBJECT

    public:

        ///@name Constructors
        ///@{

        /// std constructor
        SearchThread(QObject* parent,
                     paludis::tr1::shared_ptr<paludis::Environment>  env,
                     TaskBox* box);
        ///@}


        ///@name Content modification
        ///@{

        /*! @brief fills the search with data and starts the execution
         * @param query the string to query for
         * @param useName search in paludis::PackageNamePart
         * @param useDesc paludis::PackageNamePart
         */
        void start(const QString& query,bool useName,bool useDesc);
        ///@}

    protected:

        /// overloaded from QThread
        void run();

    private:

        /// the string to search for
        QString                                 m_query;

        /// if m_query shall be compared with paludis::PackageNamePart
        bool                                    m_optName;

        /// if m_query shall be compared with PackageIDs description metadata
        bool                                    m_optDesc;

    signals:

        /// sends an new complete Item tree to the model
        void itemResult(Item* root);
        /// requests to modify the changeState of an Category
        void changeInCat(QString cat);

        /// sends the number of hits for the actual search result when finished
        void finished(int resultCount);
    };
}

#endif
